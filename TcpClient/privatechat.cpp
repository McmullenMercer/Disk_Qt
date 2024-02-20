#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::setChatName(QString strName) //获取用户名及聊天对象名
{
    m_strChatName = strName;
    m_strLoginName = TcpClient::getInstance().loginName();
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return  instance;
}

void PrivateChat::updateMsg(const PDU *pdu)
{
    if(pdu == NULL) return;
    char caSendName[32];
    memcpy(caSendName,pdu->caData,32);

    //信息拼接
    QString strMeg = QString("%1 says: %2").arg(caSendName).arg((char*)(pdu->caMsg));//聊天内容在caMsg中
    ui->showMsg_te->append(strMeg);
}

void PrivateChat::on_sendMsg_pb_clicked()
{
    QString strMsg = ui->inputMsg_le->text(); //从ui窗口获取聊天信息
    ui->inputMsg_le->clear();
    QString strMeg = QString("%1 says: %2").arg("you").arg(strMsg);//聊天内容在caMsg中
    ui->showMsg_te->append(strMeg);
    if(!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.size()+1);//根据聊天信息确定PDU的Msglen
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;

        memcpy(pdu->caData,m_strLoginName.toStdString().c_str(),m_strLoginName.size()); //将用户名拷贝到新建立的PDU的caMsg
        memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),m_strChatName.size()); //将聊天对象拷贝到新建立的PDU的caMsg

        strcpy((char*)pdu->caMsg,strMsg.toStdString().c_str());//将消息内容拷贝进PDU的caMsg
        //发送pdu
        TcpClient::getInstance().getTcpSocket().write((char *) pdu,pdu->uiPDULen);

        //释放pdu
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"私聊","发送的信息不得为空！");
    }
}
