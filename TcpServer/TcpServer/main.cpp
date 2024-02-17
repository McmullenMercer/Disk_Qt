#include "tcpserver.h"
#include <QApplication>

#include "opedb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    OpeDB::getInstance().init(); //数据库初始化，用来连接数据库

    TcpServer w;
    w.show();
    return a.exec();
}
