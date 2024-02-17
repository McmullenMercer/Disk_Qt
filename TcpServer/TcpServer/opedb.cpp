//该类用于操作数据库
#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance() //单例
{
    static OpeDB instance;//再在静态成员函数中定义一个静态对象
    return instance; //返回静态对象的引用
}

void OpeDB::init() //初始化函数，用于和数据库的连接
{
    m_db.setHostName("localhost");//连接本地数据库
    m_db.setDatabaseName("D:\\Software\\Qt\\Qt_project\\project1\\TcpServer\\TcpServer\\cloud.db"); //要操作的数据库的名字
    if(m_db.open())
    {
        QSqlQuery query;
        query .exec("select * from usrInfo");
        while(query.next())
        {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

OpeDB::~OpeDB() //析构函数
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd) //处理注册请求函数
{
    //将数据插入进数据库中，先将用户名和密码拼接
    if(name == NULL || pwd == NULL)
    {
        return false;
        qDebug()<< "name | pwd is NULL";
    }
    QString data = QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);//name替换%1,pwd替换%2
    qDebug()<<data;
    QSqlQuery query;
    return query .exec(data); //注册成功返回1 失败返回0
}

bool OpeDB::handleLogin(const char *name, const char *pwd) //处理登录请求函数
{
    //将数据插入进数据库中，先将用户名和密码拼接
    if(name == NULL || pwd == NULL)
    {
        return false;
        qDebug()<< "name | pwd is NULL";
    }
    //数据库操作：在usrInfo表中查看是否有对应用户名和密码，以及登录状态是否为0
    QString data = QString("select * from usrInfo where name = \'%1\' and pwd = \'%2\' and online = 0").arg(name).arg(pwd);//name替换%1,pwd替换%2
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);

    if(query.next()) //如果可以登录，将online状态置1
    {
        QString data = QString("update usrInfo set online=1 where name = \'%1\' and pwd = \'%2\'").arg(name).arg(pwd);//name替换%1,pwd替换%2
        qDebug()<<data;
        QSqlQuery query;
        query.exec(data);
        return true;
    }
    else
    {
        return false;
    }
    // return query.next();//用next()成员函数判断是否匹配成功,成功返回1否则返回0
}

void OpeDB::handleOffline(const char *name)
{
    if(name == NULL) //判断形参有效性
    {
        qDebug()<< "name  is NULL";
        return;
    }

    //数据库操作：在usrInfo表匹配name，将其登录状态置为0
    QString data = QString("update usrInfo set online=0 where name = \'%1\' ").arg(name);//name替换%1
    qDebug()<<data;
    QSqlQuery query;
    query.exec(data);

}

QStringList OpeDB::handleAllOnline()
{
    //数据库操作：将处于在线状态的用户查询出来
    QString data = QString("select name from usrInfo where online=1 ");//name替换%1
    QSqlQuery query;
    query.exec(data);
    QStringList result;

    result.clear();

    while(query.next())
    {
        result.append(query.value(0).toString());
    }

    return result;
}
