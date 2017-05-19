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
    start();
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
    start();
}

QueryThread::~QueryThread()
{
    {
        QMutexLocker locker(&mutex_);
        //deviceName_.clear();
        threadEnd_ = true;
        waitCondition_.wakeOne();
    }
    wait();
}

void QueryThread::setDeivce(const QString &name, int type)
{
    QMutexLocker locker(&mutex_);
    deviceName_ = name;
    type_ = type;
    waitCondition_.wakeOne();

}

void QueryThread::setServer(const QString &ip, const QString &port)
{
    QMutexLocker locker(&mutex_);
    ip_ = ip;
    port_ = port;
    connected_ = false;
    waitCondition_.wakeOne();
}

void QueryThread::run()
{

    while(!threadEnd_)
    {
        QMutexLocker locker(&mutex_);
        qDebug()<<"QueryThread: "<<deviceName_<<"--"<<type_;
//        if(threadEnd_)
//        {
//            qDebug()<<"QueryThread end thread";
//            break;
//        }

        if(!connected_)
        {
            if(ip_.isEmpty() || port_.isEmpty())
            {
                waitCondition_.wait(&mutex_);
            }
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

        if(type_ == 4)
        {
            rmi::server_collect_info serverinfo;


            rmiClient_->get(deviceName_.toUtf8().constData(), serverinfo);

            emit dataReady(serverinfo);
        }
        else if(type_ == 5)
        {
            rmi::switch_collect_info switchInfo;
            rmiClient_->get(deviceName_.toUtf8().constData(), switchInfo);

            emit dataReady(switchInfo);
//            std::list<rmi::flow> flows = switchInfo.ifconfig();
//            QStringList switchData;
//            for(auto flow:flows)
//            {
//                switchData<<QString("%1#%2#%3#%4").arg(flow.in()).arg( flow.out() ).arg( flow.loss() ).arg( flow.error() );
//            }

//            switchData<<"111#222#333#1";
//            switchData<<"444#555#666#1";


        }
//        rmi::capability cap = serverinfo.top();
//        std::list<rmi::user> users = serverinfo.who();
//        std::list<rmi::process> processes = serverinfo.ps();

//        QStringList capdata;
//        capdata<<QString("%1#%2#%3").arg(cap.cpu() ).arg(cap.memory()).arg(cap.disk());

//        QStringList userData;
//        for(auto user : users)
//        {
//            userData<<QString("%1#%2").arg( QString::fromUtf8(user.name().c_str()) ).arg(QString::fromUtf8(user.time().c_str()));
//        }
//        //serverUserModel_->updateData(userData);

//        QStringList processData;
//        for(auto process:processes)
//        {
//            processData<<QString("%1#%2#%3").arg(process.id()).arg( QString::fromUtf8(process.name().c_str()) ).arg( QString::fromUtf8(process.path().c_str()) );
//        }

    }
     qDebug()<<"QueryThread end thread";
}
