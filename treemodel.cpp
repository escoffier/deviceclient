﻿/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QDebug>
#include "treeitem.h"
#include "treemodel.h"
#include "treethread.h"
#include <QMetaType>

//! [0]
TreeModel::TreeModel(const QStringList &headers, const QStringList &datas, QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    foreach (QString header, headers)
        rootData << header;
    qRegisterMetaType<QMap<QString, QStringList> >("DataMap");
    rootItem_ = new TreeItem(rootData);
    queryThread_ = new TreeThread;
    //connect(queryThread_, SIGNAL(dataReady(const QMap<QString, QStringList>&)), this, SLOT(updateModelData(const QMap<QString, QStringList>&)));
    connect(queryThread_, SIGNAL(dataReady(const DataMap&)), this, SLOT(updateModelData(const DataMap&)));

    serverIcon_ = new QIcon(":/images/server.png");
//    if(headers.size() == datas.size())
//    {
//        QVector<QVariant> serverData;
//        //serverData<<tr(u8"192.168.2.12");
//        rootItem_->insertChildren(rootItem_->childCount(), 1, rootItem_->columnCount());
//        for(int i = 0; i < headers.size();++i)
//        {
//            serverData<<datas.at(i);
//            rootItem_->child(rootItem_->childCount() -1)->setData(TreeItem::ENONE,i, serverData[i]);
//        }
//    }


    //QVector<QVariant> serverData;
    //serverData<<tr(u8"192.168.2.12");
    //TreeItem *serverItem = new TreeItem(serverData);
    //setupModelData(data.split(QString("#")), rootItem);

}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem_;
    delete queryThread_;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
   // qDebug()<<"columnCount: "<<rootItem_->columnCount();
    return rootItem_->columnCount();
}
//! [2]

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index);
    if( role == Qt::DecorationRole && (item->type() == TreeItem::ESERVER))
        return *serverIcon_;

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();


    //TreeItem *item = getItem(index);
    //qDebug()<<"data: "<<item->data(index.column());
    return item->data(index.column());
}

//! [3]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return QAbstractItemModel::flags(index);
    //return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
//! [3]

//! [4]
TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem_;
}
//! [4]

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        //qDebug()<<"headerData section: "<<section;
         //qDebug()<<"headerData section: "<<section;
        return rootItem_->data(section);
    }

    return QVariant();
}

//! [5]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
//! [5]

//! [6]
    TreeItem *parentItem = getItem(parent);

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem_->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem_->columnCount());
    endInsertRows();

    return success;
}

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}
//! [7]

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem_->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem_->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);

    return parentItem->childCount();
}
//! [8]

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem_->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
//    QList<TreeItem*> parents;
//    QList<int> indentations;
//    parents << parent;
//    indentations << 0;

    //beginResetModel();
//    QVector<QVariant> serverData;
//    serverData<<tr(u8"192.168.2.12");;
//    rootItem_->insertChildren(rootItem_->childCount(), 1, rootItem_->columnCount());
//    rootItem_->child(rootItem_->childCount() -1)->setData(TreeItem::ENONE,0, serverData[0]);


//    //qDebug()<<"get_server: "<<index;
//    rmiClient_->get_server(servers_);
//    //qDebug()<<"get_server ******: "<<index;
//   // QTreeWidgetItem *sever = new QTreeWidgetItem(QStringList(u8"服务器"), ResourcePage::ESERVERROOT );
//    Node* serverNode = new Node(Node::ESERVERROOT, tr(u8"服务器"));
//    serverNode->parent_ = rootNode_->children_.at(0);
//    for(auto name : servers_)
//    {
//        Node *server = new Node(Node::ESERVER, QString::fromUtf8( name.c_str() ) );
//        server->parent_ = serverNode;
//        serverNode->children_.append(server);

//    }

//    Node *switchNodes = new Node(Node::ESWTICHROOT, tr(u8"交换机"));
//    switchNodes->parent_ = rootNode_->children_.at(0);
//    rmiClient_->get_switch(switchs_);

//    for(auto name : switchs_)
//    {
//        Node *sw = new Node(Node::ESERVER, QString::fromUtf8( name.c_str() ) );
//        sw->parent_ = switchNodes;
//        switchNodes->children_.append(sw);

//    }

//    endResetModel();
   // int number = 0;


//    while (number < lines.count()) {
//        int position = 0;
//        while (position < lines[number].length()) {
//            if (lines[number].at(position) != ' ')
//                break;
//            ++position;
//        }

//        QString lineData = lines[number].mid(position).trimmed();

//        if (!lineData.isEmpty()) {
//            // Read the column data from the rest of the line.
//            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
//            QVector<QVariant> columnData;
//            for (int column = 0; column < columnStrings.count(); ++column)
//                columnData << columnStrings[column];

//            if (position > indentations.last()) {
//                // The last child of the current parent is now the new parent
//                // unless the current parent has no children.

//                if (parents.last()->childCount() > 0) {
//                    parents << parents.last()->child(parents.last()->childCount()-1);
//                    indentations << position;
//                }
//            } else {
//                while (position < indentations.last() && parents.count() > 0) {
//                    parents.pop_back();
//                    indentations.pop_back();
//                }
//            }

//            // Append a new item to the current parent's list of children.
//            TreeItem *parent = parents.last();
//            parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
//            for (int column = 0; column < columnData.size(); ++column)
//                parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
//        }

//        ++number;
    //    }
}

void TreeModel::updateModelData(const QStringList &datas)
{
    //TreeItem *centralServer =  rootItem_->child(0);
    // centralServer
}

void TreeModel::updateModelData(const QMap<QString, QStringList> &datas)
{
    beginResetModel();
    TreeItem * item = rootItem_->child(rootItem_->childCount() -1 );
    if(item->childCount() != 0)
    {
        for(int i = 0; i< item->childCount(); ++i)
        {
            item->removeChildren(i, 1);
        }
    }
    item->insertChildren(0, 2, rootItem_->columnCount());
    item->child(0)->setData(TreeItem::ESERVERROOT, 0, tr("server"));
    item->child(1)->setData(TreeItem::ESWTICHROOT, 0, tr("switch"));

//    for(int i = 0; i < rootItem_->columnCount();++i)
//    {
//        serverData<<datas.at(i);
//        rootItem_->child(rootItem_->childCount() -1)->setData(TreeItem::EROOT,i, serverData[i]);
//    }
    TreeItem * serverItem = item->child(0);
    serverItem->insertChildren(0, datas[tr("server")].size(), rootItem_->columnCount());
    int index = 0;
    QString server;
    foreach (server, datas[tr("server")] ) {
        serverItem->child(index++)->setData(TreeItem::ESERVER, 0, server);
    }

    index = 0;
    TreeItem *switchItem = item->child(1);
    switchItem->insertChildren(0, datas[tr("switch")].size(), rootItem_->columnCount());
    QString swich;
    foreach(swich, datas[tr("switch")]){
        switchItem->child(index++)->setData(TreeItem::ESWTICH, 0, swich);
    }
    endResetModel();
}

void TreeModel::setCentralServer(const QStringList &datas)
{
    beginResetModel();
    QVector<QVariant> serverData;
    //serverData<<tr(u8"192.168.2.12");
    //for(auto data : datas)
    if(rootItem_->childCount() != 0)
    {
        for(int i = 0; i< rootItem_->childCount(); ++i)
        {
         rootItem_->removeChildren(i, 1);
        }
    }
    rootItem_->insertChildren(rootItem_->childCount(), 1, rootItem_->columnCount());
    for(int i = 0; i < rootItem_->columnCount();++i)
    {
        serverData<<datas.at(i);
        rootItem_->child(rootItem_->childCount() -1)->setData(TreeItem::EROOT,i, serverData[i]);
    }
    endResetModel();
}

void TreeModel::queryDevice(const QString &ip, const QString &port)
{

//    if(rootItem_->childCount() != 0)
//    {
//        for(int i = 0; i< rootItem_->childCount(); ++i)
//        {
//         rootItem_->removeChildren(i, 1);
//        }
//    }
    queryThread_->setServer(ip, port);
}

TreeItem *TreeModel::rootItem() const
{
    return rootItem_;
}
