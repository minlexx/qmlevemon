#ifndef INVTYPE_H
#define INVTYPE_H

#include <QObject>

namespace EM {


class InvType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 typeId        READ typeId        WRITE setTypeId        NOTIFY typeIdChanged)
    Q_PROPERTY(QString typeName      READ typeName      WRITE setTypeName      NOTIFY typeNameChanged)
    Q_PROPERTY(quint64 groupId       READ groupId       WRITE setGroupId       NOTIFY groupIdChanged)
    Q_PROPERTY(QString groupName     READ groupName     WRITE setGroupName     NOTIFY groupNameChanged)
    Q_PROPERTY(quint64 categoryId    READ categoryId    WRITE setCategoryId    NOTIFY categoryIdChanged)
    Q_PROPERTY(QString categoryName  READ categoryName  WRITE setCategoryName  NOTIFY categoryNameChanged)

public:
    InvType(QObject *parent = nullptr);
    InvType(const InvType &other);
    InvType(InvType &&other);
    const InvType &operator=(const InvType &other);
    const InvType &operator=(InvType &&other);
    bool operator==(const InvType &other) const;

    bool isValid() const;

    quint64 typeId() const;
    QString typeName() const;
    quint64 groupId() const;
    QString groupName() const;
    quint64 categoryId() const;
    QString categoryName() const;

public Q_SLOTS:
    void setTypeId(quint64 typeId);
    void setTypeName(QString typeName);
    void setGroupId(quint64 groupId);
    void setGroupName(QString groupName);
    void setCategoryId(quint64 categoryId);
    void setCategoryName(QString categoryName);

Q_SIGNALS:
    void typeIdChanged(quint64 typeId);
    void typeNameChanged(QString typeName);
    void groupIdChanged(quint64 groupId);
    void groupNameChanged(QString groupName);
    void categoryIdChanged(quint64 categoryId);
    void categoryNameChanged(QString categoryName);

private:
    quint64 m_typeId = 0;
    QString m_typeName;
    quint64 m_groupId = 0;
    QString m_groupName;
    quint64 m_categoryId = 0;
    QString m_categoryName;
};


} // namespace

#endif // INVTYPE_H
