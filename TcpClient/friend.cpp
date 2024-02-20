#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include "privatechat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;          //消息显示框
    m_pFriendListWidget = new QListWidget; //好友列表
    m_pInputMsgLE = new QLineEdit;         //消息输入框

    m_pDelFriendPB= new QPushButton("删除好友");        //删除好友按钮
    m_pFlushFriendPB= new QPushButton("刷新好友");      //刷新好友按钮
    m_pShowOnlineUsrPB= new QPushButton("显示在线用户"); //显示在线用户按钮
    m_pSearchUsrPB= new QPushButton("查找用户");         //查找用户按钮
    m_pMsgSendPB= new QPushButton("信息发送");;     //信息发送按钮
    m_pPrivateChatPB= new QPushButton("私聊");;     //私聊按钮

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;  //设置垂直布局
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;  //设置水平布局
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(searchUsr()));
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool))
            ,this, SLOT(flushFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked(bool))
            ,this, SLOT(delFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool))
            ,this, SLOT(privateChat()));
    connect(m_pMsgSendPB,SIGNAL(clicked(bool))
            ,this, SLOT(groupChat()));

}

void Friend::showAllonlinUsr(PDU *pdu)
{
    if(pdu == NULL) return;
    m_pOnline->showUsr(pdu);

}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu == NULL) return;

    m_pFriendListWidget->clear();
    //循环提取pdu中名字(Msg)
    uint uiSize = pdu->uiMsgLen/32; //总人数
    char caName[32] = {'\0'};

    for(uint i = 0;i<uiSize;i++)
    {
        memcpy(caName,(char *)(pdu->caMsg) + i*32,32);
        m_pFriendListWidget->addItem(caName);
    }

}

void Friend::updateGroupMsg(PDU *pdu)
{

    QString strMsg = QString("%1 says : %2").arg(pdu->caData).arg((char*)(pdu->caMsg));
    m_pShowMsgTE->append(strMsg);
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();

        //产生PDU
        PDU * pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST; //消息类型
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen); //信息发送，write()为内置函数
        //释放PDU
        free(pdu);
        pdu = NULL;

    }
    else
    {
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    m_strSearchName = QInputDialog::getText(this,"搜索","用户名：");
    if(m_strSearchName != NULL)
    {
        //建立PDU
        qDebug() << m_strSearchName;
        PDU * pdu = mkPDU(0);
        //将name拷贝进PDU
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen); //信息发送，write()为内置函数
        //释放PDU
        free(pdu);
        pdu = NULL;
    }

}

void Friend::flushFriend()
{
    QString  strName = TcpClient::getInstance().loginName();//获取登录用户名
    //产生PDU
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());//将strName拷贝到pdu的caData里

    //发送
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

    //释放PDU
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()
{
    if( m_pFriendListWidget->currentItem() != NULL)
    {
        QString strFriendName = m_pFriendListWidget->currentItem()->text();
        PDU * pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString strName = TcpClient::getInstance().loginName(); //从TcpClient中获取用户名
        //将用户名和好友名拷进pdu中
        memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());

        //发送pdu
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

        //释放pdu
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"删除好友","请选择要删除的好友！");
    }

}

void Friend::privateChat()
{
    if( m_pFriendListWidget->currentItem() != NULL)
    {

        QString strFriendName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strFriendName);
        if (PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }
    else
    {
        QMessageBox::warning(this,"私聊","请选择要聊天的好友！");
    }

}

void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLE->text();
    m_pInputMsgLE->clear();
    QString strMeg = QString("%1 says: %2").arg("you").arg(strMsg);//聊天内容在caMsg中
    m_pShowMsgTE->append(strMsg);
    if(strMsg.isEmpty())
    {
        QMessageBox::warning(this,"群聊","发送信息不能为空");
    }
    else
    {
        PDU *pdu = mkPDU(strMsg.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        QString strSendName = TcpClient::getInstance().loginName();
        strncpy(pdu->caData,strSendName.toStdString().c_str(),strSendName.size()); //将用户名拷贝进pdu的caData中
        strncpy((char*)(pdu->caMsg),strMsg.toStdString().c_str(),strMsg.size());//将消息内容拷贝进pdu的caMsg中

        TcpClient::getInstance().getTcpSocket().write((char *) pdu,pdu->uiPDULen);

        free(pdu);
        pdu = NULL;
    }
}
