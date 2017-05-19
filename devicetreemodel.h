#ifndef DEVICETREEMODEL_H
#define DEVICETREEMODEL_H
#include <QAbstractItemModel>
#include "ap.hpp"

class Node
{
public:
    enum RESTYPE
    {
        ENONE = 0,
        EROOT = 1,
        ESERVERROOT = 2,
        ESWTICHROOT = 3,
        ESERVER = 4,
        ESWTICH = 5,
    };

    Node(RESTYPE type, const QString &str = "");
    ~Node();

    RESTYPE type_;
    QString str_;
    Node* parent_;
    QList<Node*> children_;

};

class DeviceTreeModel : public QAbstractItemModel
{
public:
    DeviceTreeModel(QObject *parent = 0);
    ~DeviceTreeModel();

    void updateDevice();
    void setRootNode(Node* node);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;


private:
    Node* nodeFromIndex(const QModelIndex &index) const;
    rmi::ap *rmiClient_;
    std::list< std::string > servers_;
    std::list< std::string > switchs_;
    QVector<QVariant> datas_;
    Node* rootNode_;

};
#endif // DEVICETREEMODEL_H
