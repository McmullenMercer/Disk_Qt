#include "online.h"
#include "ui_online.h"
#include <QDebug>
#include <QMessageBox>
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if(pdu == NULL) return;
    uint uiSize = pdu->uiMsgLen/32; //在线用户个数
    char caTmp[32];
    ui->online_lw->clear();
    for(uint i =0;i<uiSize;i++)
    {
        memcpy(caTmp,
               (char*)(pdu->caMsg)+i*32,
               32);

        ui->online_lw->addItem(caTmp);
    }
}

// 加好友功能实现
//void Online::on_addFriend_pb_clicked()
//{
//    QListWidgetItem *pItem = ui->online_lw->currentItem();
//    QString strPerUsrName = pItem->text();
//    QString strLoginName = TcpClient::getInstance().loginName();
//    PDU *pdu = mkPDU(0);
//    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
//    memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());
//    memcpy(pdu->caData + 32,strLoginName.toStdString().c_str(),strLoginName.size());
//    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
//    free(pdu);
//    pdu = NULL;
//}

void Online::on_addFriend_pb_clicked()
{
    QListWidgetItem *pItem =  ui->online_lw->currentItem();
    if(pItem != NULL)
    {
        QString strPerUsrName =  pItem->text();  //好友名字
        QString strLoginName = TcpClient::getInstance().loginName(); //自己名字

        if(strPerUsrName == strLoginName)
        {
            QMessageBox::warning(this,"加好友","你不能添加自己为好友");
        }else
        {
            //新建
            PDU* pdu = mkPDU(0);
            pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());//拷贝好友名字进pdu
            memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());//拷贝自己名字进pdu
            //发送
            TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
            //清空
            free(pdu);
            pdu = NULL;
        }


    }else
    {
        QMessageBox::warning(this,"加好友","未选中用户");
    }

}
