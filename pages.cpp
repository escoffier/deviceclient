#include <QtWidgets>

#include "pages.h"
#include "resourcemodel.h"
#include "treeitem.h"
#include "treemodel.h"
#include "ap.hpp"
#include "querythread.h"

ControlPage::ControlPage(QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);


     button_ = new QPushButton(tr(u8"设备控制"), this);
//    QGroupBox *configGroup = new QGroupBox(tr("Server configuration"));

//    QLabel *serverLabel = new QLabel(tr("Server:"));
//    QComboBox *serverCombo = new QComboBox;
//    serverCombo->addItem(tr("Qt (Australia)"));
//    serverCombo->addItem(tr("Qt (Germany)"));
//    serverCombo->addItem(tr("Qt (Norway)"));
//    serverCombo->addItem(tr("Qt (People's Republic of China)"));
//    serverCombo->addItem(tr("Qt (USA)"));

//    QHBoxLayout *serverLayout = new QHBoxLayout;
//    serverLayout->addWidget(serverLabel);
//    serverLayout->addWidget(serverCombo);

//    QVBoxLayout *configLayout = new QVBoxLayout;
//    configLayout->addLayout(serverLayout);
//    configGroup->setLayout(configLayout);

   QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(button_);
//    mainLayout->addStretch(1);
         setLayout(mainLayout);
}

ControlPage::~ControlPage()
{

}

UpdatePage::UpdatePage(QWidget *parent)
    : QFrame(parent)
{
    QGroupBox *updateGroup = new QGroupBox(tr("Package selection"));
    QCheckBox *systemCheckBox = new QCheckBox(tr("Update system"));
    QCheckBox *appsCheckBox = new QCheckBox(tr("Update applications"));
    QCheckBox *docsCheckBox = new QCheckBox(tr("Update documentation"));

    QGroupBox *packageGroup = new QGroupBox(tr("Existing packages"));

    QListWidget *packageList = new QListWidget;
    QListWidgetItem *qtItem = new QListWidgetItem(packageList);
    qtItem->setText(tr("Qt"));
    QListWidgetItem *qsaItem = new QListWidgetItem(packageList);
    qsaItem->setText(tr("QSA"));
    QListWidgetItem *teamBuilderItem = new QListWidgetItem(packageList);
    teamBuilderItem->setText(tr("Teambuilder"));

    QPushButton *startUpdateButton = new QPushButton(tr("Start update"));

    QVBoxLayout *updateLayout = new QVBoxLayout;
    updateLayout->addWidget(systemCheckBox);
    updateLayout->addWidget(appsCheckBox);
    updateLayout->addWidget(docsCheckBox);
    updateGroup->setLayout(updateLayout);

    QVBoxLayout *packageLayout = new QVBoxLayout;
    packageLayout->addWidget(packageList);
    packageGroup->setLayout(packageLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(updateGroup);
    mainLayout->addWidget(packageGroup);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(startUpdateButton);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QueryPage::QueryPage(QWidget *parent)
    : QTabWidget(parent)
{
    queryThread_ = nullptr;
    timerid_ = 0;
    capWidget_ = new QTableView();
    userWidget_ = new QTableView();
    processWidget_ = new QTableView();
    switchWidget_ = new QTableView();
    setTableStyle(capWidget_);
    setTableStyle(userWidget_);
    setTableStyle(processWidget_);
    setTableStyle(switchWidget_);

    QStringList capheaders;
    capheaders << tr("CPU") << tr("Memory") << tr("Disk");
    QStringList capDatas;
    capDatas<<"12#13#1";
    serverCapModel_ = new ServerResourceModel(capheaders, capDatas);
    capWidget_->setModel(serverCapModel_);
    capWidget_->setAlternatingRowColors(true);
    capWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList userheaders;
    userheaders << tr("Name") << tr("Time") ;
    qDebug()<<userheaders;
    QStringList userdatas;
    //userdatas<<"aaa#19:56:34";
    //userdatas<<"aaa#21:16:54";
    serverUserModel_ = new ServerResourceModel(userheaders, userdatas);
    userWidget_->setModel(serverUserModel_);
    userWidget_->setAlternatingRowColors(true);
    userWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    for (int column = 0; column < serverUserModel_->columnCount(); ++column)
//        userWidget->resizeColumnToContents(column);


    QStringList processheaders;
    processheaders << tr("ID") << tr("Name") << tr("Path");
    QStringList processDatas;
    processDatas<<"122#samba#aaaaaa";
    processDatas<<"3455#ssl#bbbbb";
    serverProcessModel_ = new ServerResourceModel(processheaders, processDatas);
    processWidget_->setModel(serverProcessModel_);
    processWidget_->setAlternatingRowColors(true);
    processWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    addTab(userWidget_,  u8"用户");
    addTab(capWidget_, u8"性能");
    addTab(processWidget_, u8"进程");

    QStringList switchHeaders;
    switchHeaders << tr("In") << tr("Out") << tr("Loss")<< tr("Error");
    QStringList switchDatas;
    switchDatas<<"111#222#333#1";
    switchDatas<<"444#555#666#1";
    swicthModel_ = new ServerResourceModel(switchHeaders, switchDatas);
    switchWidget_->setModel(swicthModel_);
    switchWidget_->setAlternatingRowColors(true);
    startQueryThread();
    //timeid_ = startTimer(3000);
    //rmiClient_ = new rmi::ap;
    //rmiClient_->start(serverIp_.toUtf8().constData(), serverPort_);
    //rmiClient_->start("192.168.31.150", 10001);
    qDebug()<<"192.168.31.150";

}

QueryPage::~QueryPage()
{
    if(timerid_ != 0)
        killTimer(timerid_);
    delete queryThread_;
}

rmi::ap *QueryPage::getRmiClient()
{
    return rmiClient_;
}

void QueryPage::setCentralServer(const QString &ip, const QString &port)
{
    serverIp_ = ip;
    serverPort_ = port;
    queryThread_->setServer(ip, port);
}

void QueryPage::queryData(QTreeWidgetItem *item, int column)
{
    (void)column;
#if 0
    if( ( (item->type() == TreeItem::ESERVER) || (item->type() == ESWTICH) )
        && (timerid_ == 0) )
    {
        timerid_ = startTimer(3000);
    }
    if(item->text(0).isEmpty())
    {
        return;
    }
    if(item->type() == ESERVER)
    {
        if(count() == 1)
        {
            clear();
        }
        addTab(userWidget_, u8"用户");
        addTab(capWidget_, u8"能力");
        addTab(processWidget_, u8"进程");
        deviceName_ = item->text(0);

        //std::list<std::string> servers;
        rmi::server_collect_info serverinfo;
        rmiClient_->get(deviceName_.toUtf8().constData(), serverinfo);
        rmi::capability cap = serverinfo.top();
        std::list<rmi::user> users = serverinfo.who();
        std::list<rmi::process> processes = serverinfo.ps();

        QStringList capdata;
        capdata<<QString("%1#%2#%3").arg(cap.cpu() ).arg(cap.memory()).arg(cap.disk());
        serverCapModel_->updateData(capdata);

        QStringList userData;
        for(auto user : users)
        {
            userData<<QString("%1#%2").arg( QString::fromUtf8(user.name().c_str()) ).arg(QString::fromUtf8(user.time().c_str()));
        }
        serverUserModel_->updateData(userData);

        QStringList processData;
        for(auto process:processes)
        {
            processData<<QString("%1#%2#%3").arg(process.id()).arg( QString::fromUtf8(process.name().c_str()) ).arg( QString::fromUtf8(process.path().c_str()) );
        }
        serverProcessModel_->updateData(processData);

    }
    else if(item->type() == ESWTICH)
    {
        if(count() == 3)
        {
            clear();
        }
        addTab(switchWidget_ ,u8"交换机");
        deviceName_ = item->text(0);
//        rmi::switch_collect_info switchInfo;
//        rmiClient_->get(deviceName_.toUtf8().constData(), switchInfo);
//        std::list<rmi::flow> flows = switchInfo.ifconfig();
        QStringList switchData;
//        for(auto flow:flows)
//        {
//            switchData<<QString("%1#%2#%3#%4").arg(flow.in()).arg( flow.out() ).arg( flow.loss() ).arg( flow.error() );
//        }

        switchData<<"111#222#333#1";
        switchData<<"444#555#666#1";

        serverProcessModel_->updateData(switchData);
    }
#endif
}

void QueryPage::queryData(QModelIndex index)
{
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    qDebug()<<"Item type: "<<item->type()<<"-----"<<item->data(0);
    if(item->type() == TreeItem::ESERVER || item->type() == TreeItem::ESWTICH )
        queryThread_->setDeivce(item->data(0).toString(), item->type());


    if(item->type() == TreeItem::ESERVER)
    {
        if(count() == 1)
        {
            clear();
        }
        addTab(userWidget_, u8"用户");
        addTab(capWidget_, u8"性能");
        addTab(processWidget_, u8"进程");

    }
    else if(item->type() == TreeItem::ESWTICH)
    {
        if(count() == 3)
        {
            clear();
        }
        addTab(switchWidget_ ,u8"交换机");
    }
#if 0
    if( ( (item->type() == TreeItem::ESERVER) || (item->type() == TreeItem::ESWTICH) )
        && (timerid_ == 0) )
    {
        timerid_ = startTimer(3000);
    }
//    if(item->text(0).isEmpty())
//    {
//        return;
//    }
    if(item->type() == TreeItem::ESERVER)
    {
        if(count() == 1)
        {
            clear();
        }
        addTab(userWidget_, u8"用户");
        addTab(capWidget_, u8"能力");
        addTab(processWidget_, u8"进程");
        deviceName_ = item->data(0).toString();

        //std::list<std::string> servers;
        rmi::server_collect_info serverinfo;
        rmiClient_->get(deviceName_.toUtf8().constData(), serverinfo);
        rmi::capability cap = serverinfo.top();
        std::list<rmi::user> users = serverinfo.who();
        std::list<rmi::process> processes = serverinfo.ps();

        QStringList capdata;
        capdata<<QString("%1#%2#%3").arg(cap.cpu() ).arg(cap.memory()).arg(cap.disk());
        serverCapModel_->updateData(capdata);

        QStringList userData;
        for(auto user : users)
        {
            userData<<QString("%1#%2").arg( QString::fromUtf8(user.name().c_str()) ).arg(QString::fromUtf8(user.time().c_str()));
        }
        serverUserModel_->updateData(userData);

        QStringList processData;
        for(auto process:processes)
        {
            processData<<QString("%1#%2#%3").arg(process.id()).arg( QString::fromUtf8(process.name().c_str()) ).arg( QString::fromUtf8(process.path().c_str()) );
        }
        serverProcessModel_->updateData(processData);

    }
    else if(item->type() == TreeItem::ESWTICH)
    {
        if(count() == 3)
        {
            clear();
        }
        addTab(switchWidget_ ,u8"交换机");
        deviceName_ = item->data(0).toString();
//        rmi::switch_collect_info switchInfo;
//        rmiClient_->get(deviceName_.toUtf8().constData(), switchInfo);
//        std::list<rmi::flow> flows = switchInfo.ifconfig();
        QStringList switchData;
//        for(auto flow:flows)
//        {
//            switchData<<QString("%1#%2#%3#%4").arg(flow.in()).arg( flow.out() ).arg( flow.loss() ).arg( flow.error() );
//        }

        switchData<<"111#222#333#1";
        switchData<<"444#555#666#1";

        serverProcessModel_->updateData(switchData);
    }
#endif
}

void QueryPage::startQueryThread(/*const QString &ip, const QString &port*/)
{
    if(!queryThread_)
    {
        queryThread_ = new QueryThread;
        connect(queryThread_, SIGNAL(dataReady(const rmi::server_collect_info &)),
                this, SLOT(updaData(const rmi::server_collect_info&)));

        connect(queryThread_, SIGNAL(dataReady(const rmi::switch_collect_info &)),
                this, SLOT(updaData(const rmi::switch_collect_info&)));
    }
}

void QueryPage::timerEvent(QTimerEvent *t)
{
    if(currentDeviceType_ == TreeItem::ESERVER)
    {
        queryServerData();
    }
    else if(currentDeviceType_ == TreeItem::ESWTICH)
    {
        querySwitchData();
    }
//    QStringList datas;
//    datas<<"2222#samba#aaaaaa;";
//     datas<<"3333#event#bbbbb;";
//     qDebug()<<datas;
    //     serverProcessModel_->updateData(datas);
}

void QueryPage::updaData(const rmi::server_collect_info &serverinfo)
{
    rmi::capability cap = serverinfo.top();
    std::list<rmi::user> users = serverinfo.who();
    std::list<rmi::process> processes = serverinfo.ps();

    QStringList capdata;
    capdata<<QString("%1#%2#%3").arg(cap.cpu() ).arg(cap.memory()).arg(cap.disk());
    serverCapModel_->updateData(capdata);

    QStringList userData;
    for(auto user : users)
    {
        userData<<QString("%1#%2").arg( QString::fromUtf8(user.name().c_str()) ).arg(QString::fromUtf8(user.time().c_str()));
    }
    serverUserModel_->updateData(userData);

    QStringList processData;
    for(auto process:processes)
    {
        processData<<QString("%1#%2#%3").arg(process.id()).arg( QString::fromUtf8(process.name().c_str()) ).arg( QString::fromUtf8(process.path().c_str()) );
    }
    serverProcessModel_->updateData(processData);

}

void QueryPage::updaData(const rmi::switch_collect_info &switchInfo)
{
    std::list<rmi::flow> flows = switchInfo.ifconfig();
    QStringList switchData;
    for(auto flow:flows)
    {
        switchData<<QString("%1#%2#%3#%4").arg(flow.in()).arg( flow.out() ).arg( flow.loss() ).arg( flow.error() );
    }

//            switchData<<"111#222#333#1";
//            switchData<<"444#555#666#1";

    serverProcessModel_->updateData(switchData);

}

void QueryPage::queryServerData()
{
    rmi::server_collect_info serverinfo;
    rmiClient_->get(deviceName_.toUtf8().constData(), serverinfo);
    rmi::capability cap = serverinfo.top();
    std::list<rmi::user> users = serverinfo.who();
     std::list<rmi::process> processes = serverinfo.ps();

     QStringList capdata;
     capdata<<QString("%1#%2#%3").arg(cap.cpu() ).arg(cap.memory()).arg(cap.disk());
     serverCapModel_->updateData(capdata);

     QStringList userData;
     for(auto user : users)
     {
         userData<<QString("%1#%2").arg( QString::fromUtf8(user.name().c_str()) ).arg(QString::fromUtf8(user.time().c_str()));
     }
     serverUserModel_->updateData(userData);

     QStringList processData;
     for(auto process:processes)
     {
         processData<<QString("%1#%2#%3").arg(process.id()).arg( QString::fromUtf8(process.name().c_str()) ).arg( QString::fromUtf8(process.path().c_str()) );
     }
     serverProcessModel_->updateData(processData);
}

void QueryPage::querySwitchData()
{
            rmi::switch_collect_info switchInfo;
            rmiClient_->get(deviceName_.toUtf8().constData(), switchInfo);
            std::list<rmi::flow> flows = switchInfo.ifconfig();
            QStringList switchData;
            for(auto flow:flows)
            {
                switchData<<QString("%1#%2#%3#%4").arg(flow.in()).arg( flow.out() ).arg( flow.loss() ).arg( flow.error() );
            }

//            switchData<<"111#222#333#1";
//            switchData<<"444#555#666#1";

            serverProcessModel_->updateData(switchData);
}

void QueryPage::setTableStyle(QTableView* table)
{
    //capWidget_->horizontalHeader()->setClickable(false);    //设置表头不可点击（默认点击后进行排序

    QFont font = table->horizontalHeader()->font();    //设置表头字体加粗
    font.setBold(true);
    table->horizontalHeader()->setFont(font);
    //capWidget_->horizontalHeader()->setStretchLastSection(true);    //设置充满表宽度
    //capWidget_->horizontalHeader()->setMovable(false);              //表头左右互换
    table->setFrameShape(QFrame::NoFrame);      //设置无边框

    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setSelectionMode(QAbstractItemView::SingleSelection);

    table->setSortingEnabled(false);

    table->verticalHeader()->hide();

    table->setWordWrap(false);

    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    table->setShowGrid(false);

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table->horizontalHeader()->setHighlightSections(false);

    //table->setItemDelegate(new NoFocusDelegate());

    //m_viewList->setModel(m_proxyModel);

    //m_viewList->setAlternatingRowColors(true);  // alternative colors

   // m_viewList->setFrameShape(QFrame::NoFrame);
}

//void QueryPage::showResorce(QTreeWidgetItem *item, int column)
//{
//    if(item->type() == ServerResourceModel::ESERVER)
//    {

//    }
//}


