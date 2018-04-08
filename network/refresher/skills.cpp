#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include "periodical_refresher_worker.h"
#include "../periodical_refresher.h"
#include "eve_api/eve_api.h"
#include "models/character.h"
#include "models/character_skillqueue.h"
#include "models/skill_tree_model.h"
#include "models/model_manager.h"


namespace EM {


int PeriodicalRefresherWorker::refresh_skills(Character *ch) {
    if (!ch->updateTimestamps().isUpdateNeeded(UpdateTimestamps::UTST::SKILLS)) {
        qCDebug(logRefresher) << " no need to refresh skills for" << ch->toString();
        return 0;  // no update needed, too early
    }
    // check if tokens needs refresing
    if (!this->check_refresh_token(ch)) {
        return 0;
    }

    int num_updates = 0;
    QJsonObject reply;
    QJsonArray replyArr;

    // refresh character attributes
    qCDebug(logRefresher) << " refreshing attributes for" << ch->toString();
    if (m_api->get_character_attributes(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

        num_updates++;
        /* { // example reply
          "charisma": 29,
          "intelligence": 31,
          "memory": 30,
          "perception": 39,
          "willpower": 38,
          "bonus_remaps": 1,
          "last_remap_date": "2015-04-18T14:35:25Z",
          "accrued_remap_cooldown_date": "2016-04-17T14:35:25Z"
        } */
        int charisma = reply.value(QLatin1String("charisma")).toInt();
        int intelligence = reply.value(QLatin1String("intelligence")).toInt();
        int memory = reply.value(QLatin1String("memory")).toInt();
        int perception = reply.value(QLatin1String("perception")).toInt();
        int willpower = reply.value(QLatin1String("willpower")).toInt();
        int bonus_remaps = reply.value(QLatin1String("bonus_remaps")).toInt();
        QString slast_remap_date = reply.value(QLatin1String("last_remap_date")).toString();
        QString sremap_cooldown_date = reply.value(QLatin1String("accrued_remap_cooldown_date")).toString();
        QDateTime last_remap_date = QDateTime::fromString(slast_remap_date, Qt::ISODate);
        QDateTime remap_cooldown_date = QDateTime::fromString(sremap_cooldown_date, Qt::ISODate);

        if (ch->attributeCharisma() != charisma) {
            ch->setAttributeCharisma(charisma);
        }
        if (ch->attributeIntelligence() != intelligence) {
            ch->setAttributeIntelligence(intelligence);
        }
        if (ch->attributeMemory() != memory) {
            ch->setAttributeMemory(memory);
        }
        if (ch->attributePerception() != perception) {
            ch->setAttributePerception(perception);
        }
        if (ch->attributeWillpower() != willpower) {
            ch->setAttributeWillpower(willpower);
        }
        if (ch->numBonusRemaps() != bonus_remaps) {
            ch->setNumBonusRemaps(bonus_remaps);
        }
        if (ch->lastRemapDate() != last_remap_date) {
            ch->setLastRemapDate(last_remap_date);
        }
        if (ch->remapCooldownDate() != remap_cooldown_date) {
            ch->setRemapCooldownDate(remap_cooldown_date);
        }
    }
    if (QThread::currentThread()->isInterruptionRequested()) return 0;

    // refresh character skills
    SkillTreeModel *skillTree = ModelManager::instance()->skillTreeModel();
    // get ALL character's skills
    qCDebug(logRefresher) << " refreshing skills for" << ch->toString();
    if (m_api->get_character_skills(reply, ch->characterId(), ch->getAuthTokens().access_token)) {
        if (QThread::currentThread()->isInterruptionRequested()) return 0;

        bool isAlphaClone = false;
        QVector<CharacterSkill> charSkills;

        quint64 totalSp = reply.value(QLatin1String("total_sp")).toVariant().toULongLong();
        ch->setTotalSp(totalSp);

        const QJsonArray jskills = reply.value(QLatin1String("skills")).toArray();
        for (auto jskillvalue: jskills) {
            // qCDebug(logRefresher) << jskill;
            // {"active_skill_level":5, "skill_id":30547, "skillpoints_in_skill":256000, "trained_skill_level":5}
            const QJsonObject &jskill = jskillvalue.toObject();
            quint64 skill_id = jskill.value(QLatin1String("skill_id")).toVariant().toULongLong();

            // create Character skill from skill template to "inherit" attributes
            const SkillTemplate *skill_template = skillTree->findSkill(skill_id);
            CharacterSkill chSkill(skill_template);
            chSkill.setSkillId(skill_id);
            chSkill.setActiveLevel(jskill.value(QLatin1String("active_skill_level")).toInt());
            chSkill.setTrainedLevel(jskill.value(QLatin1String("trained_skill_level")).toInt());
            chSkill.setSkillPointsInSkill(jskill.value(QLatin1String("skillpoints_in_skill")).toVariant().toULongLong());

            // warning detection
            if (chSkill.activeLevel() == 0) {
                // whaat, how canthis be (it can be if skill was injected but not traiend)
                qCWarning(logRefresher) << "    We got a skill with active level = 0!" << chSkill.skillId() << chSkill.skillName();
                // qCWarning(logRefresher) << "    " << jskill;
            }

            // detect alpha clone
            // if any skill has activeLevel < trainedLevel, this is alpha
            if (chSkill.activeLevel() < chSkill.trainedLevel()) {
                qCDebug(logRefresher) << "    << Alpha clone detected >>";
                isAlphaClone = true;
            }

            // store
            charSkills.push_back(std::move(chSkill));

            // there are a lot of skills (few hundreds) so chech for exit more often
            if (QThread::currentThread()->isInterruptionRequested()) return 0;
        }

        // qCDebug(logRefresher) << Q_FUNC_INFO << "parsed skills";

        // forcefully update character's alpha clone status
        ch->setIsAlphaClone(isAlphaClone);
        ch->setSkills(charSkills);
    }

    // refresh character skillqueue
    qCDebug(logRefresher) << " refreshing skill queue for" << ch->toString();
    if (m_api->get_character_skillqueue(replyArr, ch->characterId(), ch->getAuthTokens().access_token)) {
        if (QThread::currentThread()->isInterruptionRequested()) return 0;
        // qCDebug(logRefresher) << replyArr;

        ch->clearSkillQueue();

        // this array comes ordered by queue position acsending,
        // but "queue_position" in JSON sometimes does not start at 0
        // we can count position ourselves
        int real_qpos = 0;
        for (auto jqueueItem: qAsConst(replyArr)) {
            // [
            //   {
            //     "skill_id": 20315,
            //     "finished_level": 5,
            //     "queue_position": 0,
            //     "level_end_sp": 1280000,
            //     "level_start_sp": 226275,
            //     "training_start_sp": 226275
            //     "start_date": "2018-01-28T11:42:48Z",
            //     "finish_date": "2018-02-15T18:45:55Z",
            //   },

            const QJsonObject &itemObj = jqueueItem.toObject();

            CharacterSkillQueueItem queueItem;
            queueItem.skillId = itemObj.value(QLatin1String("skill_id")).toVariant().toULongLong();
            queueItem.trainingLevel = itemObj.value(QLatin1String("finished_level")).toInt();
            queueItem.queuePosition = itemObj.value(QLatin1String("queue_position")).toInt();
            queueItem.levelStartSp = itemObj.value(QLatin1String("level_start_sp")).toVariant().toULongLong();
            queueItem.levelEndSp = itemObj.value(QLatin1String("level_end_sp")).toVariant().toULongLong();
            queueItem.trainingStartSp = itemObj.value(QLatin1String("training_start_sp")).toVariant().toULongLong();
            queueItem.startDate = QDateTime::fromString(itemObj.value(QLatin1String("start_date")).toString(), Qt::ISODate);
            queueItem.finishDate = QDateTime::fromString(itemObj.value(QLatin1String("finish_date")).toString(), Qt::ISODate);

            // force skill queue items to be numbered from zero
            if (queueItem.queuePosition != real_qpos) {
                // qCDebug(logRefresher) << "    skillqueue: fixing qpos from" << qinfo.queuePosition << " to " << real_qpos;
                queueItem.queuePosition = real_qpos;
            }

            ch->skillQueue().addItem(std::move(queueItem));

            ++real_qpos;
        }

        ch->calcSkillQueue();

        // qCDebug(logRefresher) << Q_FUNC_INFO << "parsed skillqueue";
    }

    ch->setUpdateTimestamp(UpdateTimestamps::UTST::SKILLS);
    return num_updates;
}


} // namespace EM
