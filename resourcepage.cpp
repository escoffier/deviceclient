#include "resourepage.h"
#include <QLayout>
#include <QTabWidget>

//resourcePage::resourcePage():
//    QDialog(0)
//{
//    tableWidget = new QTableWidget(this);
//    //tableWidget->setRowCount(10);
//    tableWidget->setColumnCount(1);
//}

ResourcePage::ResourcePage(QWidget *parent, int type,int columnCount):
    QDialog(parent)
{

    resize(QSize(300,200));

    QTableWidget *tableWidget = new QTableWidget(this);
    if(type == ESERVERROOT) //server
    {
       setWindowTitle(u8"服务器资源");
//       QStringList strLabels;
//       strLabels.append(u8"CPU");
//       strLabels.append(u8"内存");
//       strLabels.append(u8"硬盘");
      //tableWidget->setHorizontalHeaderLabels(strLabels);
    }
    else if(type == ESWTICHROOT)
    {
         setWindowTitle(u8"交换机资源");
//        QStringList strLabels;
//        strLabels.append(u8"IN");
//        strLabels.append(u8"OUT");
//        strLabels.append(u8"硬盘");
//       tableWidget->setHorizontalHeaderLabels(strLabels);
    }
    //tableWidget = new QTableWidget(this);
    //tableWidget->setRowCount(10);
    tableWidget->setColumnCount(columnCount);

     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addWidget(tableWidget, 0);
     setLayout(mainLayout);
}
