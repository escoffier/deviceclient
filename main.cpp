#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTranslator translator;
//    //bool b = true;
//    if(!translator.load(":/cn.qm"))
//    {
//        qDebug()<<"load qm failed";
//    }


//    if(!a.installTranslator(&translator))
//    {
//        qDebug()<<"installTranslator failed";
//    }
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }

    MainWindow w;
    w.setWindowFlags(w.windowFlags()& ~Qt::WindowMaximizeButtonHint);
    w.show();

    return a.exec();
}
