#include "mainwindow.h"
#include "pages.h"
#include "dialog.h"
#include <QStackedWidget>
#include <QTreeWidget>
#include <QLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QSignalMapper>
#include <QDebug>
#include <QToolButton>
#include <QToolBar>
#include <QLabel>
#include <QPixmap>
#include <QTranslator>
#include <QStatusBar>
#ifdef _MSC_VER
#include <WinSock2.h>
#endif
#include "resourepage.h"
//#include"devicetreemodel.h"
#include "treemodel.h"
#include "treeitem.h"

void MainWindow::changePage(int index)
{
    qDebug()<<"index: "<<index;
    if(index < MAXTYPE)
    {
        pagesWidget->setCurrentIndex(index);
        if(index == 0)
        {
            queryResource(true);
            #if 0
            treekWidget_->clear();
            //treekWidget_-> (u8"设备列表");
            rmi::ap * rmiClient = queryPage_->getRmiClient();
            std::list< std::string > servers;
            qDebug()<<"get_server: "<<index;
            rmiClient->get_server(servers);
            qDebug()<<"get_server ******: "<<index;
            QTreeWidgetItem *sever = new QTreeWidgetItem(QStringList(u8"服务器"), ResourcePage::ESERVERROOT );
            for(auto name : servers)
            {
                //qDebug()<< QString::fromUtf8( name.c_str() );
                //QStringList strlist;
                //QString<<
                QTreeWidgetItem *child1 = new QTreeWidgetItem(QStringList(QString::fromUtf8( name.c_str() )), ResourcePage::ESERVER);
                sever->addChild(child1);
            }
            treekWidget_->addTopLevelItem(sever);

            QTreeWidgetItem *switchitem = new QTreeWidgetItem(QStringList(u8"交换机"), ResourcePage::ESWTICHROOT );

            std::list< std::string > switchs;
            //qDebug()<<"get_server: "<<switchs;
            rmiClient->get_switch(switchs);
            for(auto name : switchs)
            {
                //qDebug()<< QString::fromUtf8( name.c_str() );
                QTreeWidgetItem *child = new QTreeWidgetItem(QStringList(QString::fromUtf8( name.c_str() )), ResourcePage::ESWTICH);
                switchitem->addChild(child);
            }
            treekWidget_->addTopLevelItem(switchitem);
#endif

        }

    }
}

void MainWindow::showServerDlg()
{
    serverDlg = new ServerDialog(this);
    if(serverDlg->exec())
    {
        server_.ip_ = serverDlg->ipEdit_->text();
        server_.port_ = serverDlg->portEdit_->text();
        server_.user_ = serverDlg->userEdit_->text();
        server_.password_ = serverDlg->pwdEdit_->text();
        treeModel_->setCentralServer(QStringList(tr(u8"192.168.2.12")));
        treeModel_->queryDevice(server_.ip_, server_.port_);
    }
}

void MainWindow::showServerDlg(bool checked)
{
    serverDlg = new ServerDialog(this);
    if(serverDlg->exec())
    {
        server_.ip_ = serverDlg->ipEdit_->text();
        server_.port_ = serverDlg->portEdit_->text();
        server_.user_ = serverDlg->userEdit_->text();
        server_.password_ = serverDlg->pwdEdit_->text();
        treeModel_->setCentralServer(QStringList(tr(u8"192.168.2.12")));
    }
}

void MainWindow::showAlarmDlg(bool checked)
{
    alarmDlg_ = new AlarmDialog(this);
    alarmDlg_->exec();

}

void MainWindow::showResorce(QTreeWidgetItem *item, int column)
{
//    ResourcePage * page = new ResourcePage(this, item->type(), 3);
//    page->exec();
    qDebug()<<"showResorce"<<item->type();
    if(item->type() == TreeItem::EROOT)
    {

    }
    else if(item->type() == TreeItem::ESERVER)
    {
        QString str(u8"服务器：");
        str.append(item->text(0));
        statusbarLabel_->setText(str);
    }
    else if(item->type() == TreeItem::ESWTICH)
    {
        QString str(u8"交换机：");
        str.append(item->text(0));
        statusbarLabel_->setText(str);
    }
    queryPage_->queryData(item, column);

}

void MainWindow::queryResource(bool checked)
{
    //if(checked)
    {
    //QTreeWidgetItem* currentItem = treekWidget_->currentItem();
        QModelIndex curIndex = treekView_->currentIndex();
        if(curIndex.isValid() )
        {
            TreeItem *item = static_cast<TreeItem*>(curIndex.internalPointer());
            qDebug()<<"Action query type: "<<item->type();
        }
//    TreeItem item = currentItem->data(0, Qt::DisplayRole).value<TreeItem>();
//    qDebug()<<item.getType();
     //queryPage_->queryData(currentItem, 0);
    }
}

void MainWindow::queryResource(const QModelIndex &index)
{
    //TreeItem curitem = item->data(0, Qt::DisplayRole).value<TreeItem>();
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    qDebug()<<"Item type: "<<item->type();
    queryPage_->queryData(index);

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize( QSize( 800, 600 ));
    setWindowTitle(tr(u8"设备监控"));


     initWinSock();
     createActions();
     createToolBars();
    //setWindowFlags(Qt::FramelessWindowHint);
#if 0
    QToolBar *fileToolBar = addToolBar(tr("File"));
    //const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/1.png"));
    QAction *newAct = new QAction(QIcon(":/images/1.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    //connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    //fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);


   QSignalMapper *signalMapper = new QSignalMapper(this);

    signalMapper->setMapping(newAct, 1);

    connect(newAct, SIGNAL(triggered()), signalMapper,SLOT(map()));
    connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(changePage(int)));
#endif
   /////////////////////////////////////////////////
    //QWidget *btnWidget = new QWidget(this);
    QFrame *btnWidget = new QFrame(this);
    btnWidget->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    QPushButton *deviceCtrl = new QPushButton;
//    deviceCtrl->setIcon(QIcon(":/images/1.png"));
//    deviceCtrl->setIconSize(QSize(113,119));
//    deviceCtrl->setStyleSheet("border:none");
    deviceCtrl->setText(u8"资源监控");

    QPushButton * deviceInspection = new QPushButton;
    deviceInspection->setText(u8"设备控制");
//    deviceInspection->setPixmap(QPixmap(":/images/1.png"));

    QPushButton *serverBtn = new QPushButton;
//    serverBtn->setIcon(QIcon(":/images/1.png"));
//    serverBtn->setIconSize(QSize(113,119));
    //serverBtn->setStyleSheet("border:none");
    serverBtn->setText(u8"服务器配置");
    connect(serverBtn, SIGNAL(clicked()), this,SLOT(showServerDlg()));


//    QSignalMapper *signalMapper = new QSignalMapper(this);
//    signalMapper->setMapping(deviceCtrl, 0);
//    connect(deviceCtrl, SIGNAL(clicked()), signalMapper,SLOT(map()));


//    connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(changePage(int)));


    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->insertWidget(0, deviceCtrl);
    buttonsLayout->insertWidget(1, deviceInspection);
    buttonsLayout->insertWidget(2, serverBtn);
    buttonsLayout->addStretch(1);
    //btnWidget->setAutoFillBackground(false);
    //btnWidget->setStyleSheet("background-color:blue;");
    btnWidget->setLayout(buttonsLayout);


    QHBoxLayout *upLayout = new QHBoxLayout;
    upLayout->addWidget(btnWidget);
    ////////////////////////////////////////////////////

//   QPushButton *ButtonCtrl = new QPushButton(tr("设备监控"));
//   QPushButton *ButtonnInspection = new QPushButton(tr("设备巡检"));
//   QPushButton *ButtonPower = new QPushButton(tr("计划供电"));
//   QPushButton *ButtonUserAdmin = new QPushButton(tr("用户管理"));
//   QPushButton *ButtonEvent = new QPushButton(tr("事件记录"));

//   QSignalMapper *signalMapper = new QSignalMapper(this);
//   signalMapper->setMapping(ButtonCtrl, 0);
//   signalMapper->setMapping(ButtonnInspection, 1);
//   signalMapper->setMapping(ButtonPower, 2);
//   signalMapper->setMapping(ButtonUserAdmin, 3);
//   signalMapper->setMapping(ButtonEvent, 4);

//   connect(ButtonCtrl, SIGNAL(clicked()), signalMapper,SLOT(map()));
//   connect(ButtonnInspection, SIGNAL(clicked()), signalMapper,SLOT(map()));
//   connect(ButtonPower, SIGNAL(clicked()), signalMapper,SLOT(map()));
//   connect(ButtonUserAdmin, SIGNAL(clicked()), signalMapper,SLOT(map()));
//   connect(ButtonEvent, SIGNAL(clicked()), signalMapper,SLOT(map()));



//   connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(changePage(int)));
//   QHBoxLayout *buttonsLayout = new QHBoxLayout;
//   buttonsLayout->addStretch(1);

//   buttonsLayout->insertWidget(0, ButtonCtrl);
//   buttonsLayout->insertWidget(1,ButtonnInspection);
//   buttonsLayout->insertWidget(2, ButtonPower);
//   buttonsLayout->insertWidget(3, ButtonUserAdmin);
//   buttonsLayout->insertWidget(4, ButtonEvent);



   pagesWidget = new QStackedWidget;
   queryPage_ = new QueryPage(this);
   controlPage_ = new ControlPage(this);
   pagesWidget->addWidget(queryPage_);
   pagesWidget->addWidget(controlPage_);
   //pagesWidget->addWidget(new ControlPage);
   //pagesWidget->addWidget(new UpdatePage);


   createTreeView();
//   treekView_ = new QTreeView;
//   TreeModel *model = new TreeModel(QStringList(tr(u8"设备列表")), tr(""), this);
//   model->setupModelData(QStringList(""), 0);
//   treekView_->setModel(model);
//   connect(treekView_, SIGNAL(doubleClicked(QModelIndex)),this, SLOT());
#if 0
   treekWidget_ = new QTreeWidget;
   treeHeader_ = new QTreeWidgetItem(QStringList(u8"192.168.2.1"), QueryPage::EROOT );
   treekWidget_->setHeaderItem(treeHeader_);
   treekWidget_->setHeaderLabel(u8"设备列表");

   QTreeWidgetItem *sever = new QTreeWidgetItem(QStringList(u8"服务器"), QueryPage::ESERVERROOT );
   QTreeWidgetItem *child1 = new QTreeWidgetItem(QStringList("device1"), QueryPage::ESERVER);
   sever->addChild(child1);

   QTreeWidgetItem *switchs = new QTreeWidgetItem(QStringList(u8"交换机"), QueryPage::ESWTICHROOT);
   QTreeWidgetItem *switch1 = new QTreeWidgetItem(QStringList("switch1"), ResourcePage::ESWTICH);
   switchs->addChild(switch1);
   treeHeader_->addChild(sever);
   treeHeader_->addChild(switchs);
   treekWidget_->addTopLevelItem(treeHeader_);
//   treekWidget_->addTopLevelItem(sever);
//   treekWidget_->addTopLevelItem(switchs);

    connect(treekWidget_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this,SLOT(showResorce(QTreeWidgetItem*,int)));
   //connect(treekWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), queryPage_,SLOT(queryData(QTreeWidgetItem*,int)) );
    //connect(treeHeader_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this,SLOT(showResorce(QTreeWidgetItem*,int)));
#endif

    QHBoxLayout *widgetslayout = new QHBoxLayout;
    widgetslayout->addWidget( treekView_);
    //widgetslayout->addStretch(1);
    widgetslayout->addWidget( pagesWidget);
    widgetslayout->setStretchFactor(treekWidget_, 0);
    widgetslayout->setStretchFactor(pagesWidget, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(upLayout);
    //mainLayout->addStretch(1);
    mainLayout->addSpacing(2);
    mainLayout->addLayout(widgetslayout);
    //setLayout(mainLayout);
    QWidget* widget = new QWidget(this);
    widget->setLayout(mainLayout);

    this->setCentralWidget(widget);
    statusbarLabel_ = new QLabel;
    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}"));
    statusBar()->addWidget(statusbarLabel_);
}

MainWindow::~MainWindow()
{

}

void MainWindow::initWinSock()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return ;
    }

/* Confirm that the WinSock DLL supports 2.2.*/
/* Note that if the DLL supports versions greater    */
/* than 2.2 in addition to 2.2, it will still return */
/* 2.2 in wVersion since that is the version we      */
/* requested.                                        */

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return ;
    }
    else
        printf("The Winsock 2.2 dll was found okay\n");
}

void MainWindow::createToolBars()
{
    toolBar_ = addToolBar(u8"12");
    toolBar_->addAction(showAction_);
    toolBar_->addAction(controlAction_);
    toolBar_->addAction(configureAction_);
     toolBar_->addAction(alarmAction_);
}

void MainWindow::createActions()
{
    showAction_ = new QAction(u8"资源监视", this);

    controlAction_ = new QAction(u8"设备控制", this);
    configureAction_ = new QAction(u8"服务器配置", this);
    alarmAction_= new QAction(u8"告警查看", this);

    //connect(showAction_, SIGNAL(triggered(bool)), this, SLOT(queryResource(bool)) );

    connect(configureAction_, SIGNAL(triggered(bool)), this, SLOT(showServerDlg(bool) ));
    connect(alarmAction_, SIGNAL(triggered(bool)), this, SLOT(showAlarmDlg(bool) ));


    QSignalMapper *signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(showAction_, 0);
    signalMapper->setMapping(controlAction_, 1);

    connect(showAction_, SIGNAL(triggered(bool)), signalMapper,SLOT(map()));
    connect(controlAction_, SIGNAL(triggered(bool)), signalMapper,SLOT(map()));

    connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(changePage(int)));
}

void MainWindow::createTreeView()
{
    treekView_ = new QTreeView;
    treeModel_ = new TreeModel(QStringList(tr(u8"设备列表")), QStringList(/*tr(u8"192.168.2.12")*/), this);
    //treeModel_->setupModelData(QStringList(""), 0);
    treekView_->setModel(treeModel_);
    //treeModel_->setCentralServer(QStringList(tr(u8"192.168.2.12")));
    connect(treekView_, SIGNAL(doubleClicked(const QModelIndex &)),this, SLOT(queryResource(const QModelIndex &)) );
}

Server::Server()
{

}

Server::~Server()
{

}
