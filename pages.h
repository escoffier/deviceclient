#ifndef PAGES_H
#define PAGES_H

#include <QWidget>
#include <QFrame>
#include <QTabWidget>
#include "ap.hpp"

class ServerResourceModel;
class QTreeWidgetItem;
class QTableView;
class QPushButton;
class QueryThread;

//class rmi::ap;
class ControlPage : public QFrame
{
    Q_OBJECT
public:
    ControlPage(QWidget *parent = 0);
    ~ControlPage();
    QPushButton * button_;
};

class QueryPage : public QTabWidget
{
    Q_OBJECT
public:
//    enum RESTYPE
//    {
//        EROOT = 1,
//        ESERVERROOT = 2,
//        ESWTICHROOT = 3,
//        ESERVER = 4,
//        ESWTICH = 5,
//    };

    QueryPage(QWidget *parent = 0);
    ~QueryPage();
    rmi::ap * getRmiClient();
    void setCentralServer(const QString &ip, const QString &port);
protected:
    void timerEvent( QTimerEvent * t);
public:

//    void showResorce(QTreeWidgetItem *item, int column);
    void queryData(QTreeWidgetItem*item ,int column);
    void queryData(QModelIndex index);
    void startQueryThread(/*const QString& ip, const QString& port*/);
public slots:
    void updaData(const rmi::server_collect_info& serverinfo);
    void updaData(const rmi::switch_collect_info& switchInfo);
private:
    void queryServerData();
    void querySwitchData();
    void setTableStyle(QTableView* table);
private:
    QTableView *capWidget_;
    QTableView *userWidget_ ;
    QTableView *processWidget_;
    QTableView *switchWidget_;
    ServerResourceModel * serverCapModel_;
    ServerResourceModel * serverProcessModel_;
    ServerResourceModel * serverUserModel_;
    ServerResourceModel * swicthModel_;
    QString  deviceName_;
    int timerid_;
    rmi::ap *rmiClient_;
    QString serverIp_;
    QString serverPort_;
    int currentDeviceType_;
    QueryThread * queryThread_;
};

class UpdatePage : public QFrame
{
public:
    UpdatePage(QWidget *parent = 0);
};


#endif
