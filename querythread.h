#ifndef QUERYTHREAD_H
#define QUERYTHREAD_H
#include <QThread>
#include "ap.hpp"
//#include <treeitem.h>
#include <QMutex>
#include<QWaitCondition>

class QueryThread : public QThread
{
    Q_OBJECT
public:
    QueryThread();
    QueryThread(const QString& device, int type);
    ~QueryThread();

    void setDeivce(const QString& name, int type);
    void setServer(const QString& ip, const QString& port);
signals:
    //void sendData(const QStringList& data, int type);
    void dataReady(const rmi::server_collect_info& serverinfo);

     void dataReady(const rmi::switch_collect_info &switchInfo);
protected:
    void run();

private:
     rmi::ap * rmiClient_;
     QString deviceName_;
     int type_;
     QString ip_;
     QString port_;
     QMutex mutex_;
     QWaitCondition waitCondition_;
     bool threadEnd_;
     bool connected_;
};
#endif // QUERYTHREAD_H
