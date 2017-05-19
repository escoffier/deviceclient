#ifndef DIALOG_H
#define DIALOG_H
#include <QFrame>
#include <QDialog>
#include<QLabel>
#include<QLineEdit>
class QTableView;
class ServerResourceModel;

class ServerDialog : public QDialog
{
public:
    ServerDialog(QWidget *parent = 0);

    QLabel *ipLabel_;
    QLineEdit *ipEdit_;

    QLabel *portLabel_;
    QLineEdit *portEdit_;

    QLabel *userLabel_;
    QLineEdit *userEdit_;

    QLabel *pwdLabel_;
    QLineEdit *pwdEdit_;
};

class AlarmDialog : public QDialog
 {
 public:

     AlarmDialog(QWidget *parent = 0);
     ~AlarmDialog();

 private:
     QTableView * alarmTable_;
     ServerResourceModel *alarmModel_;
 };
#endif // DIALOG_H
