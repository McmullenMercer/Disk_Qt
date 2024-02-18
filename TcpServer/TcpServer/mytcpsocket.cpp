#include "mytcpsocket.h"
#include <QDebug>
#include <stdio.h>
#include "mytcpserver.h"
#include <QMessageBox>

MyTcpSocket::MyTcpSocket()
{
    //关联信号槽
    connect(this,SIGNAL(readyRead())
            , this,SLOT(recvMsg()));   //readyRead()信号与recvMsg()函数关联

    connect(this,SIGNAL(disconnected())
            , this,SLOT(clientOffline()));   //disconnected()信号与clientOffline()函数关联
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::recvMsg() //数据接收槽函数
{
    qDebug() << this->bytesAvailable(); //打印当前获取的数据大小

    uint uiPDULen = 0;
    this->read((char *)&uiPDULen,sizeof(uint)); //先收一个uint长度的数据，实际获得uiPDULen(从通讯协议上看其排在前4个字节)
    uint uiMsgLen = uiPDULen-sizeof(PDU);  //计算实际消息长度，为PDU总长度-sizeof(PDU)
    PDU *pdu = mkPDU((uiMsgLen)); //通过实际消息长度产生一个PDU来接收剩余数据
    this->read((char *)pdu+sizeof(uint),uiPDULen-sizeof(uint)); //偏移一个uint的长度，因为第一个uint已经被读取;
                                                                //长度为总长度-一个uint大小
    switch(pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_REQUEST:  //类型为注册请求
    {
        //新建两个数组存放接收到的用户名和密码
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        //将接收到的用户名和密码copy到caData
        strncpy(caName,pdu->caData,32); //同样前32位放用户名
        strncpy(caPwd,pdu->caData+32,32); //后32位放密码

        bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);//调用数据库类中 处理注册请求函数 将数据存入数据库
        PDU* respdu = mkPDU(0); //回复PDU，用于存放处理注册请求函数的返回值，其中的caData[64]空间足够，故mkPDU形参为0即可
        respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND; //消息类型为注册回复
        if(ret)
        {
            strcpy(respdu->caData,REGI_OK);
        }
        else
        {
            strcpy(respdu->caData,REGI_FAILED);
        }
        //通过socket将respdu发送出去
        write((char *)respdu,respdu->uiPDULen); //发送数据
        free(respdu); //释放respdu
        respdu = NULL;

        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:  //类型为登录请求
    {

       //新建两个数组存放接收到的用户名和密码
       char caName[32] = {'\0'};
       char caPwd[32] = {'\0'};
       //将接收到的用户名和密码copy到caData
       strncpy(caName,pdu->caData,32); //同样前32位放用户名
       strncpy(caPwd,pdu->caData+32,32); //后32位放密码

       bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);
       PDU* respdu = mkPDU(0); //回复PDU，用于存放处理注册请求函数的返回值，其中的caData[64]空间足够，故mkPDU形参为0即可
       respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND; //消息类型为注册回复
       if(ret)
       {
           strcpy(respdu->caData,LOGIN_OK);
           m_strName = caName; //当登录成功时记录当前用户名，用于退出时的清空操作
       }
       else
       {
           strcpy(respdu->caData,LOGIN_FAILED);
       }
       //通过socket将respdu发送出去
       write((char *)respdu,respdu->uiPDULen); //发送数据
       free(respdu); //释放respdu
       respdu = NULL;



        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST://类型为查看所有在线用户请求
    {
        QStringList ret = OpeDB::getInstance().handleAllOnline(); //创建ret接收handleAllOnline()函数返回值

        //新建PDU
        uint uiMsgLen = ret.size()*32;  //ret存放返回的用户名，每个用户名占32个字节，构成 消息长度(Msglen)
        PDU * respdu = mkPDU(uiMsgLen); //产生pdu
        respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        //用for循环拷贝用户名到respdu中
        for(int i =0;i<ret.size();i++)
        {
            memcpy((char *)(respdu->caMsg)+i*32, //新数据地址
                   ret.at(i).toStdString().c_str(), //原数据的首地址
                   ret.at(i).size()); //原数据的大小
        }
        //通过socket将respdu发送出去
        write((char *)respdu,respdu->uiPDULen);
        //释放PDU
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST: //类型为搜索用户请求
    {
        int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
        PDU * respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(ret == -1)
        {
            strcpy(respdu->caData,SEARCH_USR_NO);
        }
        else if(ret == 1)
        {
            strcpy(respdu->caData,SEARCH_USR_ONLINE);
        }
        else if(ret == 0)
        {
            strcpy(respdu->caData,SEARCH_USR_OFFLINE);
        }
        //通过socket将respdu发送出去
        write((char *)respdu,respdu->uiPDULen);
        //释放PDU
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: //类型为添加好友请求
    {
        //新建两个数组存放接收到的好友名和自己名
        char caperName[32] = {'\0'};
        char caName[32] = {'\0'};
        //将接收到的好友名和自己名copy到caData
        strncpy(caperName,pdu->caData,32); //前32位放好友名
        strncpy(caName,pdu->caData+32,32); //后32位放自己名

        int ret = OpeDB::getInstance().handleAddfriend(caperName,caName);
        PDU * respdu = NULL;

        if(ret == -1)
        {
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, UNKOW_ERROR);
            //通过socket将respdu发送出去
            write((char *)respdu,respdu->uiPDULen);
            //释放PDU++++
            free(respdu);
            respdu = NULL;
        }
        else if(ret == 0)  //双方已经是好友
        {
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, EXISTED_FRIEND);
            //通过socket将respdu发送出去
            write((char *)respdu,respdu->uiPDULen);
            //释放PDU
            free(respdu);
            respdu = NULL;
        }
        else if(ret == 1)  //双方不是好友且对方在线
        {
            MyTcpServer::getInstance().resend(caperName,pdu);//没有新建立pdu，转发的pdu类型仍为REQUEST
        }
        else if(ret == 2) //双方不是好友且对方不在线
        {
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
            //通过socket将respdu发送出去
            write((char *)respdu,respdu->uiPDULen);
            //释放PDU
            free(respdu);
            respdu = NULL;
        }
        else if(3 == ret) //双方不是好友且对方不存在
        {
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_NO_EXIST);
            //通过socket将respdu发送出去
            write((char *)respdu,respdu->uiPDULen);
            //释放PDU
            free(respdu);
            respdu = NULL;
        }

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName, pdu->caData, 32);
        strncpy(caName, pdu->caData+32, 32);
        OpeDB::getInstance().handleAddFriendAgree(caPerName, caName);
        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData+32, 32);
        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    default:
        break;
    }
    free(pdu); //释放pdu
    pdu = NULL;


    //qDebug() << pdu->uiMsgType<<caName<<caPwd; //消息类型用的枚举方式

    //qDebug() << pdu->uiMsgType<<(char *)(pdu->caMsg); //打印数据类型 和 实际数据
}

void MyTcpSocket::clientOffline()  //处理客户端下线槽函数
{
    //将online设置为0
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str()); //调用处理客户端下线函数
    //将offline()信号发给socket，让其从列表中删除建立的客户端MyTcpSocket
    emit offline(this);//将信号发出去。this保存的是对象的地址
}



