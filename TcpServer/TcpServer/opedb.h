//该类用于操作数据库
#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject //1.
{
    Q_OBJECT //2. 该类支持信号槽
public:
    explicit OpeDB(QObject *parent = nullptr);

    //定义单例
    static OpeDB& getInstance();//先定义一个静态成员函数
    void init();

    //析构函数
    ~OpeDB();

    //处理注册请求函数
    bool handleRegist(const char * name,const char * pwd);

    //处理注册请求函数
    bool handleLogin(const char * name,const char * pwd);

    //处理客户端下线函数
    void handleOffline(const char * name);

    //处理所有在线用户函数
    QStringList handleAllOnline();

    //搜索用户函数
    int handleSearchUsr(const char* name);

    //处理添加好友函数
    int handleAddfriend(const char * pername,const char * name);

    //处理同意好友函数
    void handleAddFriendAgree(const char *addedName, const char *sourceName);

    //处理刷新好友列表函数
    QStringList handleFlushFriend(const char *name);

    //处理删除好友函数
    bool handleDelFriend(const char * name,const char * friendname);


signals:
public slots:
private:
    QSqlDatabase m_db; //用于连接数据库


};

#endif // OPEDB_H
