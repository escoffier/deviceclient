#ifndef RESOUREPAGE_H
#define RESOUREPAGE_H
#include <QTableWidget>
#include <QWidget>
#include <QDialog>

#endif // RESOUREPAGE_H
class ResourcePage : public QDialog
{
public:
    enum RESTYPE
    {
        ESERVERROOT = 1,
        ESWTICHROOT = 2,
        ESERVER = 3,
        ESWTICH = 4,
    };

    ResourcePage() {}
    ResourcePage(QWidget *parent, int type, int ColumnCount);
};

//class UserPage : public QTabWidget
//{
//public:

//}


