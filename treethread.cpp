#include "treethread.h"
#include <QMap>
#include<QDebug>

TreeThread::TreeThread()
{
    quit_ = false;
    serverChaged_ = false;
}

TreeThread::~TreeThread()
{

}

void TreeThread::setServer(const QString &ip, const QString &port)
{
    QMutexLocker locker(&mutex_);
    ip_ = ip;
    port_ = port;

    if(!isRunning())
        qDebug<<"TreeThread start";
        start();
    else
        cond_.wakeOne();
}

void TreeThread::run()
{
    bool serverChaged = false;
    QString currentIp,  currentPort;

    {
        QMutexLocker locker(&mutex_);

        if( currentIp !=ip_ || currentPort != port_)
        {
            currentIp = ip_;
            currentPort = port_;
            serverChaged = true;
        }
    }

    while (!quit_) {
        if(serverChaged)
        {
            rmiClient_->start(currentIp.toUtf8().constData(), currentPort.toInt());
            std::list<std::string> devServers;
            rmiClient_->get_server(devServers);
            std::list<std::string> devSwitchs;
            rmiClient_->get_switch(devSwitchs);
            QMap<QString, QStringList> datas;

            QStringList serverList;
            for(auto s : devServers)
            {
                serverList<<QString::fromUtf8(s.c_str());
            }

            QStringList switchList;
            for(auto s : devSwitchs)
            {
                switchList<<QString::fromUtf8(s.c_str());
            }
            datas.insert(tr("server"), serverList);
            datas.insert(tr("switch"), switchList);

            emit dataReady(datas);
        }
        {
            QMutexLocker locker(&mutex_);
            cond_.wait(&mutex_);
            if( currentIp !=ip_ || currentPort != port_)
            {
                currentIp = ip_;
                currentPort = port_;
                serverChaged = true;
            }

        }

    }
}
