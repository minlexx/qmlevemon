#ifndef H_EM_API_PERIODICAL_REFRESHER
#define H_EM_API_PERIODICAL_REFRESHER

#include <QObject>
#include <QThread>
#include <QTimer>


namespace EM {


class PeriodicalRefresherWorker;


class PeriodicalRefresher: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool networkActivity READ networkActivity NOTIFY networkActivityChanged)
    Q_PROPERTY(int serverPlayersOnline READ serverPlayersOnline NOTIFY serverPlayersOnlineChanged)

public:
    explicit PeriodicalRefresher(QObject *parent = Q_NULLPTR);
    virtual ~PeriodicalRefresher();

public Q_SLOTS:
    bool networkActivity() const;
    int serverPlayersOnline() const;
    void forceRefreshNow();

Q_SIGNALS:
    void networkActivityChanged();
    void serverPlayersOnlineChanged();

    void refresh();

protected:
    QThread m_thread;
    QTimer m_refreshTimer;
    PeriodicalRefresherWorker *m_worker;
};


} // namespace


#endif
