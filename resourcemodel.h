#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H
#include <QAbstractTableModel>
#include <QVariant>
#include <QVector>
#endif // RESOURCEMODEL_H
#include "ap.hpp"
class QueryThread;

class ServerResourceModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    ServerResourceModel(const QStringList &headers, const QStringList &datas,
                        QObject *parent = 0) ;
    ~ServerResourceModel();

    void initialize();
    void updateData(const QStringList &lines);
    void setupModelData(const QStringList &lines);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    //virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

public slots:
    void updaData(const rmi::server_collect_info& serverinfo);
    void updaData(const rmi::switch_collect_info& switchInfo);

private:
    QVector<QVariant> datas_;
    QStringList headers_;
    QueryThread *queryThread_;
};
