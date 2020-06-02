/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QGroupBox *groupBox_Host;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_HostIp;
    QLabel *label_HostIp;
    QLineEdit *lineEdit_IpAddr;
    QHBoxLayout *horizontalLayout_HostPort;
    QLabel *label_HostPort;
    QLineEdit *lineEdit_Rxport;
    QPushButton *pushButton_Accept;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_HostIp_2;
    QLineEdit *lineEdit_peer_IpAddr;
    QHBoxLayout *horizontalLayout_HostPort_2;
    QLabel *label_HostPort_2;
    QLineEdit *lineEdit_Txport;
    QPushButton *pushButton_clear;
    QPushButton *pushButton_close;

    void setupUi(QWidget *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QStringLiteral("Settings"));
        Settings->resize(627, 301);
        groupBox_Host = new QGroupBox(Settings);
        groupBox_Host->setObjectName(QStringLiteral("groupBox_Host"));
        groupBox_Host->setGeometry(QRect(10, 10, 401, 131));
        gridLayout = new QGridLayout(groupBox_Host);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_HostIp = new QHBoxLayout();
        horizontalLayout_HostIp->setObjectName(QStringLiteral("horizontalLayout_HostIp"));
        label_HostIp = new QLabel(groupBox_Host);
        label_HostIp->setObjectName(QStringLiteral("label_HostIp"));
        label_HostIp->setMaximumSize(QSize(16777215, 30));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_HostIp->setFont(font);

        horizontalLayout_HostIp->addWidget(label_HostIp);

        lineEdit_IpAddr = new QLineEdit(groupBox_Host);
        lineEdit_IpAddr->setObjectName(QStringLiteral("lineEdit_IpAddr"));
        lineEdit_IpAddr->setMinimumSize(QSize(30, 30));
        lineEdit_IpAddr->setMaximumSize(QSize(270, 16777215));

        horizontalLayout_HostIp->addWidget(lineEdit_IpAddr);


        gridLayout->addLayout(horizontalLayout_HostIp, 0, 0, 1, 1);

        horizontalLayout_HostPort = new QHBoxLayout();
        horizontalLayout_HostPort->setObjectName(QStringLiteral("horizontalLayout_HostPort"));
        label_HostPort = new QLabel(groupBox_Host);
        label_HostPort->setObjectName(QStringLiteral("label_HostPort"));
        label_HostPort->setMaximumSize(QSize(350, 30));
        label_HostPort->setFont(font);

        horizontalLayout_HostPort->addWidget(label_HostPort);

        lineEdit_Rxport = new QLineEdit(groupBox_Host);
        lineEdit_Rxport->setObjectName(QStringLiteral("lineEdit_Rxport"));
        lineEdit_Rxport->setMinimumSize(QSize(30, 30));
        lineEdit_Rxport->setMaximumSize(QSize(270, 16777215));

        horizontalLayout_HostPort->addWidget(lineEdit_Rxport);


        gridLayout->addLayout(horizontalLayout_HostPort, 1, 0, 1, 1);

        pushButton_Accept = new QPushButton(Settings);
        pushButton_Accept->setObjectName(QStringLiteral("pushButton_Accept"));
        pushButton_Accept->setGeometry(QRect(430, 40, 151, 61));
        pushButton_Accept->setFont(font);
        groupBox = new QGroupBox(Settings);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 157, 411, 121));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_HostIp_2 = new QLabel(groupBox);
        label_HostIp_2->setObjectName(QStringLiteral("label_HostIp_2"));
        label_HostIp_2->setMaximumSize(QSize(16777215, 30));
        label_HostIp_2->setFont(font);

        horizontalLayout->addWidget(label_HostIp_2);

        lineEdit_peer_IpAddr = new QLineEdit(groupBox);
        lineEdit_peer_IpAddr->setObjectName(QStringLiteral("lineEdit_peer_IpAddr"));
        lineEdit_peer_IpAddr->setMinimumSize(QSize(30, 30));
        lineEdit_peer_IpAddr->setMaximumSize(QSize(270, 16777215));

        horizontalLayout->addWidget(lineEdit_peer_IpAddr);


        gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_HostPort_2 = new QHBoxLayout();
        horizontalLayout_HostPort_2->setObjectName(QStringLiteral("horizontalLayout_HostPort_2"));
        label_HostPort_2 = new QLabel(groupBox);
        label_HostPort_2->setObjectName(QStringLiteral("label_HostPort_2"));
        label_HostPort_2->setMaximumSize(QSize(350, 30));
        label_HostPort_2->setFont(font);

        horizontalLayout_HostPort_2->addWidget(label_HostPort_2);

        lineEdit_Txport = new QLineEdit(groupBox);
        lineEdit_Txport->setObjectName(QStringLiteral("lineEdit_Txport"));
        lineEdit_Txport->setMinimumSize(QSize(30, 30));
        lineEdit_Txport->setMaximumSize(QSize(270, 16777215));

        horizontalLayout_HostPort_2->addWidget(lineEdit_Txport);


        gridLayout_2->addLayout(horizontalLayout_HostPort_2, 1, 0, 1, 1);

        pushButton_clear = new QPushButton(Settings);
        pushButton_clear->setObjectName(QStringLiteral("pushButton_clear"));
        pushButton_clear->setGeometry(QRect(430, 110, 151, 61));
        pushButton_clear->setFont(font);
        pushButton_close = new QPushButton(Settings);
        pushButton_close->setObjectName(QStringLiteral("pushButton_close"));
        pushButton_close->setGeometry(QRect(430, 190, 151, 61));
        pushButton_close->setFont(font);

        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QWidget *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "Form", Q_NULLPTR));
        groupBox_Host->setTitle(QApplication::translate("Settings", "HOST", Q_NULLPTR));
        label_HostIp->setText(QApplication::translate("Settings", "IP ADDRESS:             ", Q_NULLPTR));
        label_HostPort->setText(QApplication::translate("Settings", "PORT NUMBER:          ", Q_NULLPTR));
        pushButton_Accept->setText(QApplication::translate("Settings", "ACCEPT", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("Settings", "PEER", Q_NULLPTR));
        label_HostIp_2->setText(QApplication::translate("Settings", "IP ADDRESS:             ", Q_NULLPTR));
        label_HostPort_2->setText(QApplication::translate("Settings", "PORT NUMBER:         ", Q_NULLPTR));
        pushButton_clear->setText(QApplication::translate("Settings", "CLEAR", Q_NULLPTR));
        pushButton_close->setText(QApplication::translate("Settings", "CLOSE", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
