#ifndef H_EM_API_PERIODICAL_REFRESHER
#define H_EM_API_PERIODICAL_REFRESHER

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(logRefresher)


namespace EM {

class Character;
class PeriodicalRefresherWorker;

class PeriodicalRefresher: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool networkActivity READ networkActivity NOTIFY networkActivityChanged)
    Q_PROPERTY(int serverPlayersOnline READ serverPlayersOnline NOTIFY serverPlayersOnlineChanged)
    Q_PROPERTY(bool isMailDownloadInProgress READ isMailDownloadInProgress NOTIFY isMailDownloadInProgressChanged)

public:
    explicit PeriodicalRefresher(QObject *parent = Q_NULLPTR);
    ~PeriodicalRefresher() override;

public Q_SLOTS:
    void setRefreshIterval(int msec);
    void stopGracefully();
    bool networkActivity() const;
    int serverPlayersOnline() const;
    void forceRefreshNow();
    void requestMailBody(Character *ch, quint64 mailId);
    bool isMailDownloadInProgress() const;

Q_SIGNALS:
    void networkActivityChanged();
    void serverPlayersOnlineChanged();
    void isMailDownloadInProgressChanged();

    void refresh();

    void doRequestMailBody(Character *ch, quint64 mailId);
    void mailBodyDownloaded(quint64 charId, quint64 mailId, const QString &body);
    void networkError(const QString &desc);

protected:
    QThread m_thread;
    QTimer m_refreshTimer;
    PeriodicalRefresherWorker *m_worker;
};


} // namespace


#endif
