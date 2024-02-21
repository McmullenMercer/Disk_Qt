#include "mytcpsocket.h"
#include <QDebug>
#include <stdio.h>
#include "mytcpserver.h"
#include <QMessageBox>
#include <QDir>
#include <QFileInfoList>

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
            QDir dir;
            qDebug()<<dir.mkdir(QString("./%1").arg(caName)); //创建一个以用户名命名的文件夹
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
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:  //类型为同意好友申请
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName, pdu->caData, 32);
        strncpy(caName, pdu->caData+32, 32);
        OpeDB::getInstance().handleAddFriendAgree(caPerName, caName);
        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:  //类型为拒绝好友申请
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData+32, 32);
        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST://类型为刷新好友请求
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);  //获取在线好友的名字
        //将获取到的名字打包并发送出去
        //新建PDU
        uint uiMsgLen = ret.size()*32;  //一个名字占32位
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        //循环将ret里的名字拷贝到新建立的pdu中
        for (int i = 0;i<ret.size();i++)
        {
            memcpy((char *)(respdu->caMsg)+i*32        //目标respdu中名字的起始位置，临时转化为char *型每+1位置往后偏1
                   ,ret.at(i).toStdString().c_str()    //原ret中名字的起始位置
                   ,ret.at(i).size());                 //大小
        }
        //发送pdu
        write((char *) respdu,respdu->uiPDULen);
        //释放pdu
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST: //类型为删除好友请求
    {

        //新建两个数组存放接收到的用户名和好友名
        char caName[32] = {'\0'};
        char cafriendName[32] = {'\0'};
        //将接收到的用户名和密码copy到caData
        strncpy(caName,pdu->caData,32); //同样前32位放用户名
        strncpy(cafriendName,pdu->caData+32,32); //后32位放好友名

        OpeDB::getInstance().handleDelFriend(caName,cafriendName);

        //新建回复pdu
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        strcpy(respdu->caData,DEL_FRIEND_OK); //将回复信息放入respdu的caData
        //发送pdu
        write((char *) respdu,respdu->uiPDULen);
        //释放pdu
        free(respdu);
        respdu = NULL;


        MyTcpServer::getInstance().resend(cafriendName,pdu);



        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST: //类型为私聊请求
    {
        //新建两个数组存放接收到的用户名和好友名
        char caName[32] = {'\0'};
        char cafriendName[32] = {'\0'};
        //将接收到的用户名和密码copy到caData
        strncpy(caName,pdu->caData,32); //同样前32位放用户名
        strncpy(cafriendName,pdu->caData+32,32); //后32位放好友名
        qDebug()<<cafriendName;
        MyTcpServer::getInstance().resend(cafriendName,pdu); //将私聊请求转发给聊天对象客户端
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST: //类型为群聊请求
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName);  //获取在线好友的名字
        QString tmp;
        for(int i = 0;i<onlineFriend.size();i++)
        {
            tmp = onlineFriend.at(i);
            MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu); //循环转发群发消息
        }
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST: //类型为新建文件夹请求
    {
        QDir dir;
        QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
        bool ret = dir.exists(strCurPath);

        PDU *respdu = NULL;
        if(ret)  //当前目录存在
        {
            char caNewDir[32] = {'\0'};
            memcpy(caNewDir,pdu->caData+32,32);
            QString strNewPath = strCurPath + "/" + caNewDir;
            qDebug()<<strNewPath;
            ret = dir.exists(strNewPath);
            qDebug()<<ret;
            if(ret)  //创建的文件已存在
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,FILE_NAME_EXIST);
            }
            else   //创建的文件不存在
            {
                dir.mkdir(strNewPath);
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,CREAT_DIR_OK);
            }
        }
        else    //当前目录不存在
        {
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            strcpy(respdu->caData,DIR_NOT_EXIST);
        }

        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST: //类型为刷新文件请求
    {
        char *pCurPath = new char[pdu->uiMsgLen];  //根据pdu中的uiMsgLen确定new出的空间大小
        memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen); //将pdu中的caMsg，即当前目录拷贝出来
        QDir dir(pCurPath);
        QFileInfoList fileInfoList = dir.entryInfoList();

        //新建pdu
        int iFileCount = fileInfoList.size();
        PDU *respdu = mkPDU(sizeof(FileInfo)*iFileCount);
        respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

        FileInfo* pFileInfo = NULL;
        QString strFileName;
        for(int i =0;i<fileInfoList.size();i++)
        {
            pFileInfo = (FileInfo*)(respdu->caMsg)+i; //将respdu临时传化为FileInfo类型，便于存储偏移（结构体数组）
            strFileName = fileInfoList[i].fileName();

            memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size()); //拷贝文件名
            if(fileInfoList.at(i).isDir())
            {
                pFileInfo->iFileType = 0; //0表示类型为文件夹
            }else if(fileInfoList.at(i).isFile())
            {
                pFileInfo->iFileType = 1; //1表示类型为常规文件
            }

//            qDebug()<<fileInfoList.at(i).fileName()
//                    <<fileInfoList.at(i).size()
//                    <<"文件夹"<<fileInfoList.at(i).isDir()
//                   <<"常规文件"<<fileInfoList.at(i).isFile();
        }
        write((char*)respdu,respdu->uiPDULen);

        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST: //类型为删除文件夹请求
    {
        char caName[32];  //文件夹名字
        strcpy(caName,pdu->caData);

        char *pPath = new char[pdu->uiMsgLen]; //路径
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);

        //拼接得到完整路径
        QString strPath = QString("%1/%2").arg(pPath).arg(caName);
        qDebug()<<strPath;

        QFileInfo fileInfo(strPath);
        bool ret = false;
        if(fileInfo.isDir())
        {
            QDir dir;
            dir.setPath(strPath);
            ret = dir.removeRecursively(); //删除文件夹包含的所有文件
        }
        else if(fileInfo.isFile()) ret = false;

        PDU * respdu = NULL;
        if(ret)
        {
            respdu =  mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_DIR_OK,strlen(DEL_DIR_OK));
        }
        else
        {
            respdu =  mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_DIR_FAILED,strlen(DEL_DIR_FAILED));
        }
        write((char*)respdu,respdu->uiPDULen);

        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST: //类型为重命名文件夹请求
    {
        char caOldName[32];  //旧名字
        char caNewName[32];  //新名字
        strncpy(caOldName,pdu->caData,32);
        strncpy(caNewName,pdu->caData+32,32);

        char *pPath = new char[pdu->uiMsgLen]; //路径
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);

        //拼接得到路径
        QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldName);
        QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewName);

        QDir dir;
        bool ret = dir.rename(strOldPath,strNewPath);
        PDU * respdu = mkPDU(0);
        if(ret)
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            memcpy(respdu->caData,RENAME_DIR_OK,strlen(RENAME_DIR_OK));

        }else
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            memcpy(respdu->caData,RENAME_DIR_FAILED,strlen(RENAME_DIR_FAILED));
        }

        write((char*)respdu,respdu->uiPDULen);

        free(respdu);
        respdu = NULL;
        break;

        break;
    }
    case ENUM_MSG_TYPE_ENTER_DIR_REQUEST: //类型为查看文件夹请求
    {
        char caDirName[32];  //文件夹名字
        strncpy(caDirName,pdu->caData,32);

        char *pPath = new char[pdu->uiMsgLen]; //路径
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);

        //拼接完整路径
        QString strPath = QString("%1/%2").arg(pPath).arg(caDirName);
        qDebug()<<strPath;

        QFileInfo fileInfo(strPath);  //用来判断路径对应文件类型
        PDU *respdu = NULL;
        if(fileInfo.isDir())
        {
            QDir dir(strPath);
            QFileInfoList fileInfoList = dir.entryInfoList();

            //新建pdu
            int iFileCount = fileInfoList.size();
            respdu = mkPDU(sizeof(FileInfo)*iFileCount);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

            FileInfo* pFileInfo = NULL;
            QString strFileName;
            for(int i =0;i<fileInfoList.size();i++)
            {
                pFileInfo = (FileInfo*)(respdu->caMsg)+i; //将respdu临时传化为FileInfo类型，便于存储偏移（结构体数组）
                strFileName = fileInfoList[i].fileName();

                memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size()); //拷贝文件名
                if(fileInfoList.at(i).isDir())
                {
                    pFileInfo->iFileType = 0; //0表示类型为文件夹
                }else if(fileInfoList.at(i).isFile())
                {
                    pFileInfo->iFileType = 1; //1表示类型为常规文件
                }

            }
            write((char*)respdu,respdu->uiPDULen);

            free(respdu);
            respdu = NULL;

        }
        else if(fileInfo.isFile())
        {
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
            strcpy(respdu->caData,ENTER_DIR_FAILED);

            write((char*)respdu,respdu->uiPDULen);

            free(respdu);
            respdu = NULL;
        }

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



