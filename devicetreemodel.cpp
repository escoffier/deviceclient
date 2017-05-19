#include "devicetreemodel.h"

#include<QDebug>

DeviceTreeModel::DeviceTreeModel(QObject *parent)
    :QAbstractItemModel(parent)
{
    //datas_.append(tr(u8"192.168.2.1"));
    //rootNode_ = new Node(EROOT, tr(u8"192.168.2.1"));
    rootNode_ = new Node(Node::ENONE, tr(""));
    Node *nameNode = new Node(Node::EROOT, tr(u8"192.168.2.1"));
    nameNode->parent_ = rootNode_;
    rootNode_->children_.append(nameNode );
   // QAbstractItemModel::reset();
}

DeviceTreeModel::~DeviceTreeModel()
{

}

void DeviceTreeModel::updateDevice()
{
    beginResetModel();

    //qDebug()<<"get_server: "<<index;
    rmiClient_->get_server(servers_);
    //qDebug()<<"get_server ******: "<<index;
   // QTreeWidgetItem *sever = new QTreeWidgetItem(QStringList(u8"服务器"), ResourcePage::ESERVERROOT );
    Node* serverNode = new Node(Node::ESERVERROOT, tr(u8"服务器"));
    serverNode->parent_ = rootNode_->children_.at(0);
    for(auto name : servers_)
    {
        Node *server = new Node(Node::ESERVER, QString::fromUtf8( name.c_str() ) );
        server->parent_ = serverNode;
        serverNode->children_.append(server);

    }

    Node *switchNodes = new Node(Node::ESWTICHROOT, tr(u8"交换机"));
    switchNodes->parent_ = rootNode_->children_.at(0);
    rmiClient_->get_switch(switchs_);

    for(auto name : switchs_)
    {
        Node *sw = new Node(Node::ESERVER, QString::fromUtf8( name.c_str() ) );
        sw->parent_ = switchNodes;
        switchNodes->children_.append(sw);

    }

    endResetModel();
}

void DeviceTreeModel::setRootNode(Node *node)
{
    beginResetModel();
    delete rootNode_;
    rootNode_ = node;
    endResetModel();
}

int DeviceTreeModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int DeviceTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant DeviceTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::DisplayRole) {


        //qDebug()<<"linedata: "<<linedata;
        //QStringList datas = linedata.split("#");
        //return datas.at(index.column());

    }
    return QVariant();
}

QVariant DeviceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole && section ==0)
    {
        return tr(u8"设备列表");
    }
    return QVariant();
}

QModelIndex DeviceTreeModel::parent(const QModelIndex &child) const
{
    Node* node = nodeFromIndex(child);
    if(!node)
        return QModelIndex();
    Node* parentNode = node->parent_;
    if(!parentNode)
    {
        return QModelIndex();
    }

    Node* grandparentNode = parentNode->parent_;
    if(!grandparentNode)
        return QModelIndex();

    int row = grandparentNode->children_.indexOf(parentNode);
    //QModelIndex index = createIndex(row , 0, parentNode);
    //index.type();
    return createIndex(row , 0, parentNode);
}

QModelIndex DeviceTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!rootNode_ || row < 0 || column < 0)
    {
        return QModelIndex();
    }

    Node* parentNode = nodeFromIndex(parent);
    Node *childNode = parentNode->children_.value(row);
    if(!childNode)
        return QModelIndex();
    return createIndex(row, column, childNode);
}

Node *DeviceTreeModel::nodeFromIndex(const QModelIndex &index) const
{
    if(index.isValid())
    {
        return static_cast<Node*>(index.internalPointer());
    }
    else
    {
        return rootNode_;
    }
}

Node::Node(Node::RESTYPE type, const QString &str)
{
    type_ = type;
    str_= str;
    parent_ = 0;
}

Node::~Node()
{
    qDeleteAll(children_);
}
