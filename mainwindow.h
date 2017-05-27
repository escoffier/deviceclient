#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QStackedWidget;
class ServerDialog;
class QTreeWidgetItem;
class QueryPage;
class QLabel;
class QTreeWidget;
class QTreeView;
class TreeModel;
class ControlPage;
class AlarmDialog;


class Server
{
public:
    Server();
    ~Server();

    QString ip_;
    QString port_;
    QString user_;
    QString password_;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum WIDGETSTYPE
    {
        CONTROL = 0,
        INSPECTION,
        POWER,
        USERADMIN,
        EVENT,
        MAXTYPE
    };
public slots:
    void changePage(int index);
    void showServerDlg();
    void showServerDlg(bool checked);
    void showAlarmDlg(bool checked);
    void showResorce(QTreeWidgetItem *item, int column);
    void queryResource(bool);
    void queryResource(const QModelIndex &index);
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initWinSock();
    void createToolBars();
    void createActions();
    void createTreeView();

private:
     QStackedWidget *pagesWidget;
     ServerDialog *serverDlg;
     AlarmDialog * alarmDlg_;
     QueryPage * queryPage_;
     ControlPage *controlPage_;
     QLabel *statusbarLabel_;
    // QTreeWidget *treekWidget_;
     QTreeWidgetItem * treeHeader_;
     QTreeView * treeView_;
     TreeModel *treeModel_;
     QToolBar *toolBar_;
     QAction * showAction_;
     QAction *controlAction_;
     QAction *alarmAction_;
     QAction *configureAction_;
     Server server_;
};

#endif // MAINWINDOW_H
