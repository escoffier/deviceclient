#include "resourcemodel.h"
#include<QModelIndex>
#include<QVector>
#include "querythread.h"

ServerResourceModel::ServerResourceModel(const QStringList &headers, const QStringList &datas,
                                         QObject *parent):
    QAbstractTableModel(parent)
{
    //initialize();
    headers_ = headers;
    if(!datas.empty())
    {
        setupModelData(datas);
    }
//    queryThread_ = new QueryThread;

//    connect(queryThread_, SIGNAL(dataReady(const rmi::server_collect_info &)),
//            this, SLOT(updaData(const rmi::server_collect_info&)));

//    connect(queryThread_, SIGNAL(dataReady(const rmi::switch_collect_info &)),
//            this, SLOT(updaData(const rmi::switch_collect_info&)));
}

ServerResourceModel::~ServerResourceModel()
{
    //delete queryThread_;
}

void ServerResourceModel::initialize()
{
//    setHeaderData(0, Qt::Horizontal, QObject::tr("CPU"));
//    setHeaderData(1, Qt::Horizontal, QObject::tr("Memory"));
    //    setHeaderData(2, Qt::Horizontal, QObject::tr("Disp"));
}

void ServerResourceModel::updateData(const QStringList &lines)
{
    beginResetModel();
    datas_.clear();
    foreach (QString line, lines)
        datas_ << line;
    endResetModel();

}

void ServerResourceModel::setupModelData(const QStringList &lines)
{
    foreach (QString line, lines)
        datas_ << line;
}

int ServerResourceModel::rowCount(const QModelIndex &parent) const
{
    return datas_.size();
}

int ServerResourceModel::columnCount(const QModelIndex &parent) const
{
    return headers_.size();
}

QVariant ServerResourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
            return QVariant();

    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::DisplayRole) {
          QString linedata =  datas_[index.row()].toString();
           //qDebug()<<"linedata: "<<linedata;
          QStringList datas = linedata.split("#");
          return datas.at(index.column());
//        QString rowCurrency = currencyAt(index.row());
//        QString columnCurrency = currencyAt(index.column());
//        if (currencyMap.value(rowCurrency) == 0.0)
//            return "####";
        //double amount = currencyMap.value(columnCurrency) / currencyMap.value(rowCurrency);
        //return QString("%1").arg(amount, 0, 'f', 4);
        //return QVariant();rn
       // return "xxx";
    }
    return QVariant();
}

QVariant ServerResourceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
     {
         return  headers_.at(section);
     }
     else
     {
         return QVariant();
     }

}

void ServerResourceModel::updaData(const rmi::server_collect_info &serverinfo)
{
    rmi::capability cap = serverinfo.top();
    std::list<rmi::user> users = serverinfo.who();
    std::list<rmi::process> processes = serverinfo.ps();

    QStringList capdata;
    capdata<<QString("%1#%2#%3").arg(cap.cpu() ).arg(cap.memory()).arg(cap.disk());
    updateData(capdata);

    QStringList userData;
    for(auto user : users)
    {
        userData<<QString("%1#%2").arg( QString::fromUtf8(user.name().c_str()) ).arg(QString::fromUtf8(user.time().c_str()));
    }
    updateData(userData);

    QStringList processData;
    for(auto process:processes)
    {
        processData<<QString("%1#%2#%3").arg(process.id()).arg( QString::fromUtf8(process.name().c_str()) ).arg( QString::fromUtf8(process.path().c_str()) );
    }
    updateData(processData);
}

void ServerResourceModel::updaData(const rmi::switch_collect_info &switchInfo)
{
    std::list<rmi::flow> flows = switchInfo.ifconfig();
    QStringList switchData;
    for(auto flow:flows)
    {
        switchData<<QString("%1#%2#%3#%4").arg(flow.in()).arg( flow.out() ).arg( flow.loss() ).arg( flow.error() );
    }

//            switchData<<"111#222#333#1";
//            switchData<<"444#555#666#1";

    updateData(switchData);
}
