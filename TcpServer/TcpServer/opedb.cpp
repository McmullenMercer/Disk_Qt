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

int OpeDB::handleSearchUsr(const char *name)
{
    if(name == NULL) return -1;
    //数据库操作：将处于在线状态的用户查询出来
    QString data = QString("select online from usrInfo where name=\'%1\'").arg(name);//name替换%1
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        int ret = query.value(0).toInt();
        if(ret ==1 )  //在线
        {
            return 1;
        }
        else if(ret ==0) //不在线
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int OpeDB::handleAddfriend(const char *pername, const char *name)
{
    if(pername == NULL || name == NULL) return -1;

    QString data = QString("select * from friend where (id = (select id from usrInfo where name = \'%1\') and friendId = (select id from usrInfo where name = \'%2\'))"
                           "or(id = (select id from usrInfo where name = \'%3\') and friendId = (select id from usrInfo where name = \'%4\'))")
            .arg(pername).arg(name).arg(name).arg(pername);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);

    if(query.next())
    {
        return 0; //双方已是好友返回0
    }
    else
    {
        data = QString("select online from usrInfo where name=\'%1\'").arg(pername);//pername为好友名字
        QSqlQuery query;
        query.exec(data);
        if(query.next())
        {
            int ret = query.value(0).toInt();
            if(ret ==1 )  //不是好友且在线返回1
            {
                return 1;
            }
            else if(ret ==0) //不是好友且不在线返回2
            {
                return 2;
            }
        }
        else  //不是好友且不存在不存在返回3
        {
            return 3;
        }

    }
}

// 在同意添加好友之后向数据库中写入相关数据
void OpeDB::handleAddFriendAgree(const char *pername, const char *name)
{
    if (NULL == pername || NULL == name)
    {
        return;
    }
    QString data = QString("insert into friend(id, friendId) values((select id from usrInfo where name=\'%1\'), (select id from usrInfo where name=\'%2\'))").arg(pername).arg(name);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);

}



