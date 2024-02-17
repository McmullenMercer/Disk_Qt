#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    //从配置文件中加载数据
    loadConfig();

    //监听
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);


}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll(); // 读取全部内容
        QString strData = baData.toStdString().c_str(); //转化为字符串

        file.close();

        strData.replace("\r\n"," "); //将其中的换行符(\r\n)替换为空格

        QStringList strList = strData.split(" ");//将字符串按空格切分,返回的是字符串列表(QStringList)

        m_strIP = strList.at(0);  //从config文件中去除IP和port
        m_usPort = strList.at(1).toUShort();
        qDebug() <<"IP："<< m_strIP;
        qDebug() <<"Port:"<< m_usPort;

    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

