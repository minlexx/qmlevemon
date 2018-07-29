#ifndef COMMON_MODEL_BASE_H
#define COMMON_MODEL_BASE_H

#include <QObject>
#include <QVector>
#include <QByteArray>
#include <QHash>
#include <QAbstractListModel>
#include <QDataStream>


namespace EM {
template <class T> class CommonModelBase;
}


template <class T> QDataStream &operator<<(QDataStream &stream, const EM::CommonModelBase<T> &mdl);
template <class T> QDataStream &operator>>(QDataStream &stream, EM::CommonModelBase<T> &mdl);


namespace EM {

/**
 * Class that serves a common base for all items collections in character object.
 *
 * It encapsulated all the similar operations among them: assignemts operators,
 * copy construction, comparsion and stream save/load.
 * Also it implements rowCount() model method.
 * Model data is stored in protected member m_data of type QVector<T>.
 * Child classes only need to implement the following methods from QAbstractItemModel:
 *
 *   - QHash<int,QByteArray> roleNames() const override;
 *   - QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
 *
 * Requirements for class T: copyable, movable, assignable, copy-assignable,
 * comparable. Need to have operator==(), operator!=(), and stream load/save
 * functions defined (to/from QDataStream), i.e following should be defined:
 *
 *     QDataStream &operator<<(QDataStream &stream, const T &o);
 *     QDataStream &operator>>(QDataStream &stream, T &o);
 */
template <class T>
class CommonModelBase: public QAbstractListModel
{
public:
    CommonModelBase(QObject *parent = nullptr): QAbstractListModel(parent) {}

    CommonModelBase(const CommonModelBase<T> &other): QAbstractListModel(other.parent()) {
        (*this) = other;
    }

    CommonModelBase(CommonModelBase<T> &&other): QAbstractListModel(other.parent()) {
        (*this) = std::move(other);
    }

    CommonModelBase &operator=(const CommonModelBase<T> &other) {
        if (this == &other) return (*this);
        beginResetModel();
        m_data = other.m_data;
        endResetModel();
        return (*this);
    }

    CommonModelBase &operator=(CommonModelBase<T> &&other) {
        if (this == &other) return (*this);
        beginResetModel();
        m_data = std::move(other.m_data);
        endResetModel();
        return (*this);
    }

    bool operator==(const CommonModelBase<T> &other) const {
        return m_data == other.m_data; // compares 2 vectors
    }

    bool operator!=(const CommonModelBase<T> &other) const {
        return !((*this) == other);
    }

public:  // QAbstractItemModel overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return m_data.size();
    }

public: // for internal use, m_data getter
    QVector<T> &internalData() { return m_data; }
    const QVector<T> &internalData() const { return m_data; }

protected:
    // call this method from overriden data() method to validate index
    //      and get data at index at the same time
    const T *validateIndexAndGetData(const QModelIndex &index) const {
        if (!index.isValid()) {
            return nullptr;
        }
        int row = index.row();
        if ((row < 0) || (row >= m_data.size())) {
            return nullptr;
        }
        return &(m_data.at(row));
    }

protected:
    QVector<T> m_data;

    friend QDataStream& (::operator<< <T>)(QDataStream &stream, const EM::CommonModelBase<T> &mdl);
    friend QDataStream& (::operator>> <T>)(QDataStream &stream, EM::CommonModelBase<T> &mdl);
};


} // namespace


// stream save operator for a model - just saves model data
template <class T>
QDataStream &operator<<(QDataStream &stream, const EM::CommonModelBase<T> &mdl)
{
    stream << mdl.m_data;
    return stream;
}

// stream load operator for a model - loads model data
template <class T>
QDataStream &operator>>(QDataStream &stream, EM::CommonModelBase<T> &mdl)
{
    stream >> mdl.m_data;
    return stream;
}


#endif // COMMON_MODEL_BASE_H
