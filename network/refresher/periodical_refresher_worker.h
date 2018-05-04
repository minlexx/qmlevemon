#include <QObject>
#include <QAtomicInteger>
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
    virtual ~PeriodicalRefresherWorker();

public Q_SLOTS:
    // this is run in a background thread in an async Qt signal-slot connection
    void refresh();
    void initialDelayedRefresh();

    bool isNetworkActive() const;
    int serverPlayersOnline() const;

Q_SIGNALS:
    void characterUpdated(Character *character);


protected:
    void setNetworkActive(bool active);

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

    // utility
    EveLocation send_location_request(quint64 locationId, const QString &locationType, const QByteArray &accessToken);
    EveLocation resolve_location(quint64 locationId, const QString &locationType, const QByteArray &accessToken);
    void resolve_single_mail_recipient(MailRecipient &rcpt, const QVector<MailRecipient> &mailingLists);
    void resolve_mail_recipients(QVector<MailRecipient> &recipients, const QVector<MailRecipient> &mailingLists);

protected:
    PeriodicalRefresher *m_owner = nullptr;
    QAtomicInteger<int> m_active;
    QAtomicInteger<int> m_server_players;
    //
    EveApi *m_api = nullptr;
};

}
