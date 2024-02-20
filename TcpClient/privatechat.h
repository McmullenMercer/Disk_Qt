#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"


namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = nullptr);
    ~PrivateChat();
    void setChatName(QString strName);  //获取用户姓名聊天对象姓名函数
    static PrivateChat &getInstance();  //设置单例
    void updateMsg(const PDU *pdu); //接收并更新聊天窗口信息

private slots:
    void on_sendMsg_pb_clicked();

private:
    Ui::PrivateChat *ui;
    QString m_strChatName;  //聊天对象名字
    QString m_strLoginName; //用户(自己)名字
};

#endif // PRIVATECHAT_H
