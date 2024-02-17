#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QLineEdit>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    resize(500,256);


    //该函数从配置文件中读取IP和port
    loadConfig();

    //将信号与信号处理函数关联
    connect(&m_tcpSocket,SIGNAL(connected()) //将connected()函数(SIGNAL内置)与showConnect()函数关联
            ,this,SLOT(showConnect()));

    connect(&m_tcpSocket,SIGNAL(readyRead())//将readyRead()函数(SIGNAL内置)与recvMsg()函数
            ,this,SLOT(recvMsg()));

    //在构造函数中连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort); //连接服务器

}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()  //配置文件加载函数：从配置文件config中读取IP和port
{
    QFile file(":/clinet.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll(); // 读取全部内容
        QString strData = baData.toStdString().c_str(); //转化为字符串
       // qDebug() << strData; //打印输出
        file.close();

        strData.replace("\r\n"," "); //将其中的换行符(\r\n)替换为空格
        //qDebug() << strData;
        QStringList strList = strData.split(" ");//将字符串按空格切分,返回的是字符串列表(QStringList)
//       for (int i = 0;i<strList.size();i++)
//        {
//            qDebug() <<"-----------"<< strList[i];

//        }
        m_strIP = strList.at(0);  //从config文件中去除IP和port
        m_usPort = strList.at(1).toUShort();
//        qDebug() <<"IP："<< m_strIP;
//        qDebug() <<"Port:"<< m_usPort;
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }


}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

void TcpClient::showConnect() //指示服务器连接槽函数
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::recvMsg() //数据接收槽函数
{
    qDebug() << m_tcpSocket.bytesAvailable(); //打印当前获取的数据大小

    uint uiPDULen = 0;
    m_tcpSocket.read((char *)&uiPDULen,sizeof(uint)); //先收一个uint长度的数据，实际获得uiPDULen(从通讯协议上看其排在前4个字节)
    uint uiMsgLen = uiPDULen-sizeof(PDU);  //计算实际消息长度，为PDU总长度-sizeof(PDU)
    PDU *pdu = mkPDU((uiMsgLen)); //通过实际消息长度产生一个PDU来接收剩余数据
    m_tcpSocket.read((char *)pdu+sizeof(uint),uiPDULen-sizeof(uint)); //偏移一个uint的长度，因为第一个uint已经被读取;
                                                                //长度为总长度-一个uint大小
    switch(pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND:  //类型为注册回复
    {
        if(strcmp(pdu->caData,REGI_OK) == 0)
        {
            QMessageBox::information(this,"注册",REGI_OK);
        }
        else if(strcmp(pdu->caData,REGI_FAILED) == 0)
        {
            QMessageBox::warning(this,"注册",REGI_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:  //类型为登录回复
    {
        if(strcmp(pdu->caData,LOGIN_OK) == 0)
        {
            QMessageBox::information(this,"登录",LOGIN_OK);
            OpeWidget::getInstance().show();// 调用单例
            this->hide();
        }
        else if(strcmp(pdu->caData,LOGIN_FAILED) == 0)
        {
            QMessageBox::warning(this,"登录",LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND: //类型为查看所有在线用户回复
    {
        OpeWidget::getInstance().getFriend()->showAllonlinUsr(pdu);

        break;
    }
    default:
        break;
    }
    free(pdu); //释放pdu
    pdu = NULL;
}


//void TcpClient::on_pushButton_clicked()
//{
//    QString strMsg = ui->lineEdit->text();
//    if(!strMsg.isEmpty())
//    {
//        PDU* pdu = mkPDU(strMsg.size()+1); //申请数据空间
//        pdu->uiMsgType = 8888; //统一定义了数据类型为8888
//        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());//将要发送的数据拷贝到申请的数据空间中

//        m_tcpSocket.write((char *)pdu,pdu->uiPDULen); //发送数据
//        free(pdu);
//        pdu = NULL;
//    }
//    else
//    {
//        QMessageBox::warning(this,"信息发送","发送的信息不能为空");
//    }
//}

void TcpClient::on_login_pb_clicked() //登录槽函数
{
    //从文本框获取输入的用户名和密码
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU* pdu = mkPDU(0); //申请新空间
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST; //确定消息类型为登录请求
        strncpy(pdu->caData,strName.toStdString().c_str(),32);//拷贝用户名进caData[64]
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);//拷贝密码进caData[64]
        //通过socket将数据发送出去
        m_tcpSocket.write((char *)pdu,pdu->uiPDULen); //发送数据
        free(pdu); //释放
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败：用户名或密码不正确");
    }
}

void TcpClient::on_regist_pb_clicked() //注册槽函数
{
    //从文本框获取输入的用户名和密码
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU* pdu = mkPDU(0); //申请新空间
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST; //确定消息类型为注册请求
        strncpy(pdu->caData,strName.toStdString().c_str(),32);//拷贝用户名进caData[64]
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);//拷贝密码进caData[64]
        //通过socket将数据发送出去
        m_tcpSocket.write((char *)pdu,pdu->uiPDULen); //发送数据
        free(pdu); //释放
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败：用户名或密码为空");
    }
}


void TcpClient::on_cancel_pb_clicked() //注销槽函数
{

}
