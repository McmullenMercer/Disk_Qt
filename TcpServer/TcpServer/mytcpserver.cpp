#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{

    qDebug()<<"New Client Connected";

    MyTcpSocket * pTcpSocket = new MyTcpSocket; //每次有新客户端连接时会建立一个MyTcpSocket
    pTcpSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pTcpSocket); //将Socket放进列表中

    //当客户端下线时需要将建立的Socket删除
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket *)),
          this, SLOT(deleteSocket(MyTcpSocket *)));//将下线信号与处理下线槽函数（删除MyTcpSocket函数）关联


}

void MyTcpServer::resend(const char *pername, PDU *pdu) //转发函数
{
    if(pername == NULL || pdu == NULL) return;

    QString strName = pername;
    for (int i = 0;i<m_tcpSocketList.size();i++)
    {
        if(strName == m_tcpSocketList.at(i)->getName())  //若匹配上了
        {
            m_tcpSocketList.at(i)->write((char *)pdu,pdu->uiMsgLen);  //则发送出去
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    //在建立的m_tcpSocketList中匹配*mysocket
    QList<MyTcpSocket*>::iterator iter =m_tcpSocketList.begin();  //迭代器
    for(;iter != m_tcpSocketList.end();iter++)  //遍历
    {
        if(mysocket == *iter)
        {
            delete * iter; //删除new出来的对象
            * iter = NULL; //指针指向NULL
            m_tcpSocketList.erase(iter); //删除iter
            break;
        }
    }
    for (int i = 0;i<m_tcpSocketList.size();i++)
    {
        qDebug()<< m_tcpSocketList.at(i)->getName();
    }
}
