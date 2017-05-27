#include "treethread.h"
#include <QMap>
#include<QDebug>
#include <QUnhandledException>
#include "rmierr.hpp"

TreeThread::TreeThread()
{
    quit_ = false;
    serverChaged_ = false;

    rmiClient_ = new rmi::ap;
}

TreeThread::~TreeThread()
{
    {
        QMutexLocker locker(&mutex_);
        quit_ = true;
        cond_.wakeOne();
    }
    wait();
}

void TreeThread::setServer(const QString &ip, const QString &port)
{
    QMutexLocker locker(&mutex_);
    ip_ = ip;
    port_ = port;

    if(!isRunning()) {

        qDebug()<<"TreeThread start";
        start();
    }
    else{

        cond_.wakeOne();
    }
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
            try
            {
                rmiClient_->start(currentIp.toUtf8().constData(), currentPort.toInt());
                std::list<std::string> devServers;
                devServers.emplace_back("server1");
                devServers.emplace_back("server2");
                //rmiClient_->get_server(devServers);

                std::list<std::string> devSwitchs;
                devSwitchs.emplace_back("switch1");
                devSwitchs.emplace_back("switch2");
                //rmiClient_->get_switch(devSwitchs);



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
//            catch (QUnhandledException &e)
//            {
//                qDebug()<<tr(u8"网络异常");
//            }
            catch(std::exception &e)
            //catch (rmi::rmierror &e)
            {
                 qDebug()<<e.what();
                 QMutexLocker locker(&mutex_);
                 cond_.wait(&mutex_, 3000);
                 continue;
            }
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
