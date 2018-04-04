#include <QObject>
#include <QAtomicInteger>

namespace EM {

class PeriodicalRefresher;
class Character;
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
    int refresh_wallet(Character *ch);

protected:
    PeriodicalRefresher *m_owner = nullptr;
    QAtomicInteger<int> m_active;
    QAtomicInteger<int> m_server_players;
    //
    EveApi *m_api = nullptr;
};

}
