#ifndef EVE_LOCATION_H
#define EVE_LOCATION_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>

class QDataStream;

namespace EM { class EveLocation; }

QDataStream &operator<<(QDataStream &stream, const EM::EveLocation &loc);
QDataStream &operator>>(QDataStream &stream, EM::EveLocation &loc);
QDebug operator<<(QDebug &stream, const EM::EveLocation &loc);


namespace EM {


class EveLocation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 locationId READ locationId WRITE setLocationId NOTIFY locationIdChanged)
    Q_PROPERTY(quint64 typeId READ typeId WRITE setTypeId NOTIFY typeIdChanged)
    Q_PROPERTY(quint64 solarSystemId READ solarSystemId WRITE setSolarSystemId NOTIFY solarSystemIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(double x READ x NOTIFY coordsChanged)
    Q_PROPERTY(double y READ y NOTIFY coordsChanged)
    Q_PROPERTY(double z READ z NOTIFY coordsChanged)

public:
    EveLocation(QObject *parent = nullptr);
    EveLocation(const EveLocation &other);
    EveLocation(EveLocation &&other);
    EveLocation &operator=(const EveLocation &other);
    EveLocation &operator=(EveLocation &&other);
    bool operator==(const EveLocation &other) const;
    bool operator!=(const EveLocation &other) const;

    bool isEmpty() const;

    // fabric
    static EveLocation fromJson(const QJsonObject &jobj);
    QJsonObject toJson() const;

public Q_SLOTS:
    quint64 locationId() const;
    void setLocationId(quint64 id);

    quint64 typeId() const;
    void setTypeId(quint64 id);

    quint64 solarSystemId() const;
    void setSolarSystemId(quint64 id);

    QString name() const;
    void setName(const QString &s);

    QString type() const;
    void setType(const QString &s);

    double x() const;
    double y() const;
    double z() const;
    void setCoords(double x_, double y_, double z_);

Q_SIGNALS:
    void locationIdChanged();
    void typeIdChanged();
    void solarSystemIdChanged();
    void nameChanged();
    void typeChanged();
    void coordsChanged();

private:
    quint64 m_locationId = 0;
    quint64 m_typeId = 0;
    quint64 m_solarSystemId = 0;
    QString m_name;
    QString m_type;
    double m_x = 0.0;
    double m_y = 0.0;
    double m_z = 0.0;

    // friends
    friend QDataStream &(::operator<<)(QDataStream &stream, const EM::EveLocation &loc);
    friend QDataStream &(::operator>>)(QDataStream &stream, EM::EveLocation &loc);
    friend QDebug (::operator<<)(QDebug &stream, const EM::EveLocation &loc);
};


} // namespace

#endif // EVE_LOCATION_H
