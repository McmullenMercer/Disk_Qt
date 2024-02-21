#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"
#include <QDir>

class MyTcpSocket : public QTcpSocket //1.继承QObject
{
    Q_OBJECT //2.有该宏   故MyTcpSocket支持信号槽
public:
    MyTcpSocket();
    QString getName();


signals:
    void offline(MyTcpSocket * mysocket);//定义一个下线信号,形参与处理该信号的槽函数相同

public slots:  //槽函数
    void recvMsg();  //接收数据槽函数
    void clientOffline(); //处理客户端下线槽函数

private:
    QString m_strName;
};

#endif // MYTCPSOCKET_H
