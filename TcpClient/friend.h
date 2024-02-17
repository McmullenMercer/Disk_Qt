#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"


class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllonlinUsr(PDU *pdu);

    QString m_strSearchName;// 改变量临时存储所搜索的名字


signals:

public slots:
    void showOnline();
    void searchUsr();

private:
    QTextEdit *m_pShowMsgTE;
    QListWidget *m_pFriendListWidget;
    QLineEdit *m_pInputMsgLE;

    QPushButton *m_pDelFriendPB;     //删除好友按钮
    QPushButton *m_pFlushFriendPB;   //刷新好友按钮
    QPushButton *m_pShowOnlineUsrPB; //显示在线用户按钮
    QPushButton *m_pSearchUsrPB;     //搜索用户按钮
    QPushButton *m_pMsgSendPB;     //发送信息按钮
    QPushButton *m_pPrivateChatPB;     //私聊按钮

    Online *m_pOnline;



};

#endif // FRIEND_H
