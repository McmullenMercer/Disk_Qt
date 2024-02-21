#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket> //用于连接服务器和收发服务器数据
#include "protocol.h"
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();

    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket(); //该函数用于返回m_tcpSocket的引用

    QString loginName();

    QString curPath();


public slots:  //槽函数，是信号的处理函数，与普通函数区分
    void showConnect();  //指示服务器连接槽函数
    void  recvMsg();  //数据接收函槽数


private slots:
//    void on_pushButton_clicked();

    void on_login_pb_clicked();  //登录槽

    void on_regist_pb_clicked(); //注册槽

    void on_cancel_pb_clicked(); //注销槽

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    //创建一个Socket对象，用于连接服务器并于服务器交互
    QTcpSocket m_tcpSocket;

    QString m_strLoginName;

    QString m_strCurPath;
};
#endif // TCPCLIENT_H
