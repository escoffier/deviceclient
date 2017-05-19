#ifndef TREETHREAD_H
#define TREETHREAD_H
#include<QThread>
#include "ap.hpp"
#include <QMutex>
#include <QWaitCondition>

class TreeThread : public QThread
{
    Q_OBJECT
public:
    TreeThread();
    ~TreeThread();
    void setServer(const QString& ip, const QString& port);
protected:
    void run();

signals:
    void dataReady(const QMap<QString, QStringList>& ) ;

private:
    rmi::ap * rmiClient_;
    QString ip_;
    QString port_;
    QMutex mutex_;
    QWaitCondition cond_;
    bool quit_;
    bool serverChaged_;
};
#endif // TREETHREAD_H
