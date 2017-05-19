#include "dialog.h"
#include <QGroupBox>
#include <QLabel>
#include <QtWidgets>
#include <QMainWindow>
#include "resourcemodel.h"

ServerDialog::ServerDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr(u8"登录服务器"));
    resize(QSize(300,200));
    QGroupBox *packagesGroup = new QGroupBox(tr(u8"登录信息"));

     ipLabel_ = new QLabel(tr("IP"));
     ipEdit_ = new QLineEdit;

     portLabel_ = new QLabel(tr("Port:"));
     portEdit_ = new  QLineEdit;

     userLabel_ = new QLabel(tr(u8"用户"));
     userEdit_ = new QLineEdit;

     pwdLabel_ = new QLabel(tr(u8"密码:"));
     pwdEdit_ = new  QLineEdit;
//    QCheckBox *releasesCheckBox = new QCheckBox(tr("Releases"));
//    QCheckBox *upgradesCheckBox = new QCheckBox(tr("Upgrades"));

//    QSpinBox *hitsSpinBox = new QSpinBox;
//    hitsSpinBox->setPrefix(tr("Return up to "));
//    hitsSpinBox->setSuffix(tr(" results"));
//    hitsSpinBox->setSpecialValueText(tr("Return only the first result"));
//    hitsSpinBox->setMinimum(1);
//    hitsSpinBox->setMaximum(100);
//    hitsSpinBox->setSingleStep(10);

    //QPushButton *startQueryButton = new QPushButton(tr(u8"登录"));
    //QPushButton *cancelButton = new QPushButton(tr(u8"取消"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    //buttonBox->button((QDialogButtonBox::Ok))->setVisible(true);
    //buttonBox->button((QDialogButtonBox::Abort))->setVisible(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    //connect(startQueryButton, QPushButton::clicked, this , );
   // QHBoxLayout * btnLayout = new QHBoxLayout;
    //btnLayout->insertWidget(0, buttonBox);
    //btnLayout->insertWidget(0, startQueryButton);
    //btnLayout->insertWidget(1, cancelButton);

    QGridLayout *packagesLayout = new QGridLayout;
    packagesLayout->addWidget(ipLabel_, 0, 0);
    packagesLayout->addWidget(ipEdit_, 0, 1);
    packagesLayout->addWidget(portLabel_, 0, 2);
    packagesLayout->addWidget(portEdit_, 0, 3);
    packagesLayout->addWidget(userLabel_, 1, 0);
    packagesLayout->addWidget(userEdit_, 1, 1);
    packagesLayout->addWidget(pwdLabel_, 1, 2);
    packagesLayout->addWidget(pwdEdit_, 1, 3);
    packagesGroup->setLayout(packagesLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(packagesGroup);
    mainLayout->addWidget(buttonBox);
    //mainLayout->addWidget(packagesGroup);
    //mainLayout->addSpacing(3);
    //mainLayout->addWidget(startQueryButton);
    //mainLayout->addLayout(btnLayout);
    //mainLayout->addStretch(1);
    setLayout(mainLayout);

}

AlarmDialog::AlarmDialog(QWidget *parent):
    QDialog(parent)
{
    setWindowTitle(tr(u8"告警信息"));
    resize(QSize(300,200));
    alarmTable_ = new QTableView;
    QStringList headers;
    headers<<tr("name")<<tr("type");
    alarmModel_ = new ServerResourceModel(headers, QStringList());
    alarmTable_->setModel(alarmModel_);
    alarmTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(alarmTable_);
    setLayout(mainLayout);
}

AlarmDialog::~AlarmDialog()
{

}
