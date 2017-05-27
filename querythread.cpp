#include "querythread.h"
#include "ap.hpp"
#include <QDebug>
QueryThread::QueryThread()
{
    rmiClient_ = new rmi::ap;
    //rmiClient_->start(serverIp_.toUtf8().constData(), serverPort_);
    //rmiClient_->start("192.168.31.150", 10001);
    threadEnd_ = false;
    connected_ = false;

    //start();
    qDebug()<<"QueryThread";
}

QueryThread::QueryThread(const QString &device, int type)
{
    deviceName_ = device;
    type_ = type;
    rmiClient_ = new rmi::ap;
    //rmiClient_->start(serverIp_.toUtf8().constData(), serverPort_);
    //rmiClient_->start("192.168.31.150", 10001);
    threadEnd_ = false;
    connected_ = false;
    //start();
}

QueryThread::~QueryThread()
{
     threadEnd_.store(true);
    {
        QMutexLocker locker(&mutex_);

        rmiClient_->stop();
        delete rmiClient_;
        waitCondition_.wakeOne();
    }
    wait();
}

void QueryThread::setDeivce(const QString &name, int type)
{

    QMutexLocker locker(&mutex_);
    deviceName_ = name;
    type_ = type;
    if(!isRunning())
        start();
    else
        waitCondition_.wakeOne();

}

void QueryThread::setServer(const QString &ip, const QString &port)
{
   QMutexLocker locker(&mutex_);
   qDebug()<<"QueryThread::setServer "<<ip_;
    ip_ = ip;
    port_ = port;
    connected_ = false;

    //服务器发送变化，停止资源占有数据的更新
    deviceName_.clear();
}

void QueryThread::run()
{

    while(!threadEnd_.load())
    {
        int curType;
        QString curName;

        {
            QMutexLocker locker(&mutex_);
            qDebug()<<"QueryThread: "<<deviceName_<<"--"<<type_;

            //bool isConnect = connected_;
            if(ip_.isEmpty() || port_.isEmpty())
            {
                waitCondition_.wait(&mutex_);
            }

            if(!connected_)
            {
                rmiClient_->start(ip_.toUtf8().constData(), port_.toInt());
                qDebug()<<"connected to server";
                connected_ = true;
            }

            //设备类型不正确或者设备名为空，阻塞线程
            if( (type_ != 4 && type_ != 5) || deviceName_.isEmpty() )
            {
                qDebug()<<"QueryThread waiting ";
                waitCondition_.wait(&mutex_);
            }

            curType = type_;
            curName = deviceName_;
        }

        if(curType == 4)
        {
            try
            {
                rmi::server_collect_info serverinfo;
                qDebug()<<"Query device: "<<curName;
                rmiClient_->get(curName.toUtf8().constData(), serverinfo);

                emit dataReady(serverinfo);
            }
            catch(std::exception &e)
            {
                qDebug()<<e.what();
            }
        }
        else if(curType == 5)
        {
            try
            {
                rmi::switch_collect_info switchInfo;
                rmiClient_->get(curName.toUtf8().constData(), switchInfo);

                emit dataReady(switchInfo);
            }
            catch(std::exception &e)
            {
                qDebug()<<e.what();
            }
        }

        //3s更新一次数据
        {
            QMutexLocker locker(&mutex_);
            waitCondition_.wait(&mutex_, 3000);
            qDebug()<<"QueryThread end thread";
        }
    }
}
