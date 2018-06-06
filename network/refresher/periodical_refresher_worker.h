#include <QObject>
#include <QAtomicInt>
#include "models/character.h"
#include "models/eve_location.h"

namespace EM {

class PeriodicalRefresher;
class EveApi;

class PeriodicalRefresherWorker: public QObject
{
    Q_OBJECT

public:
    PeriodicalRefresherWorker(PeriodicalRefresher *parent);
    ~PeriodicalRefresherWorker() override;

public Q_SLOTS:
    // this is run in a background thread in an async Qt signal-slot connection
    void refresh();
    void initialDelayedRefresh();
    void requestCharacterMailBody(Character *ch, quint64 mailId);

    bool isNetworkActive() const;
    int  serverPlayersOnline() const;
    bool isMailDownloadInProgress() const;

Q_SIGNALS:
    void characterUpdated(Character *character);
    void mailBodyDownloaded(quint64 charId, quint64 mailId, const QString &body);


protected:
    void setNetworkActive(bool active);
    void setMailDownloadInProgress(bool active);

    /**
     * @brief check_refresh_token - check if EVE OAuth tokens need refresh, refresh if needed
     * @param ch - Character pointer whose tokens to refresh
     * @return true if refresh is not needed, ot refresh was OK. false on refresh error
     */
    bool check_refresh_token(Character *ch);

    void refresh_server_status();
    int refresh_public_data(Character *ch);
    int refresh_location(Character *ch);
    int refresh_skills(Character *ch);
    int resresh_clones(Character *ch);
    int refresh_jump_fatigue(Character *ch);
    int refresh_wallet(Character *ch);
    int refresh_mail(Character *ch);
    int refresh_notifications(Character *ch);

    // utility
    EveLocation send_location_request(quint64 locationId, const QString &locationType, const QByteArray &accessToken);
    void resolve_single_mail_recipient(MailRecipient &rcpt, const QVector<MailRecipient> &mailingLists);
    void resolve_mail_recipients(QVector<MailRecipient> &recipients, const QVector<MailRecipient> &mailingLists);
    // resolvers
    EveLocation resolve_location(quint64 locationId, const QString &locationType, const QByteArray &accessToken);
    QString resolve_corporation_name(quint64 corpId);
    QString resolve_character_name(quint64 charId);
    QString resolve_alliance_name(quint64 allyId);

    Mail requestMailBody(const Character *ch, quint64 mailId);

protected:
    PeriodicalRefresher *m_owner = nullptr;
    QAtomicInt m_active = 0;
    QAtomicInt m_server_players = 0;
    QAtomicInt m_mailDownloadInProgress = 0;
    //
    EveApi *m_api = nullptr;
};

}
