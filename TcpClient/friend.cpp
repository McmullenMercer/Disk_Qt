#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>

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

}

void Friend::showAllonlinUsr(PDU *pdu)
{
    if(pdu == NULL) return;
    m_pOnline->showUsr(pdu);

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
