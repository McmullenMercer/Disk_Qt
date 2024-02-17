#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"


class MyTcpServer : public QTcpServer //1继承QObject
{
    Q_OBJECT  //2.有了该宏.满足2个条件才能检测信号槽
public:
    MyTcpServer();

    static MyTcpServer &getInstance();

    void incomingConnection(qintptr handle); //每个客户端与服务器连接都会建立一个socket

    void resend(const char * pername,PDU* pdu); //转发函数

public slots:
    void deleteSocket(MyTcpSocket * mysocket);//删除socket槽函数，客户端下线时用于删除建立的MyTcpSocket

private:
    QList<MyTcpSocket *> m_tcpSocketList;  //将每一个MyTcpSocket保存在一个链表中
};

#endif // MYTCPSERVER_H
