#include "resourcemodel.h"
#include<QModelIndex>
#include<QVector>

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
