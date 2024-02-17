#include "tcpclient.h"

#include <QApplication>

#include <QMessageBox>
#include <QDebug>
//#include "opewidget.h"
//#include "online.h"
//#include "friend.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    TcpClient w;  //未将TcpClient写成单例前
//    w.show();

    TcpClient::getInstance().show();//将TcpClient写成单例后

//    OpeWidget w;
//    w.show();

//    Online w;
//    w.show();

//    Friend w;
//    w.show();

    return a.exec();
}
