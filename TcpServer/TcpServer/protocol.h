#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string>

typedef  unsigned int uint;

//枚举方法定义消息类型
enum ENUM_MSF_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,  //最小

    ENUM_MSG_TYPE_REGIST_REQUEST,  //注册请求 1
    ENUM_MSG_TYPE_REGIST_RESPOND,  //注册回复 2
    ENUM_MSG_TYPE_LOGIN_REQUEST,   //登录请求 3
    ENUM_MSG_TYPE_LOGIN_RESPOND,   //登录回复 4
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,  //查看在线用户请求5
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,  //查看在线用户回复6
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,  //搜索用户请求7
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,  //搜索用户回复8
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,  //添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,  //添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,  //同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,  //拒绝添加好友
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,  //刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,  //刷新好友回复
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,  //删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,  //删除好友回复
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,  //私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,  //私聊回复
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,  //群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,  //群聊回复

    ENUM_MSG_TYPE_MAX = 0x00ffffff, //最大
};

#define REGI_OK "resigt ok"
#define REGI_FAILED "resigt failed:name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed:name error or pwd error or relogin "

#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"

#define ADD_FRIEND_OK "add friend ok"
#define UNKOW_ERROR "unkonw error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NO_EXIST "usr not exist"

#define DEL_FRIEND_OK "delete friend OK"

// 设计通讯协议
struct PDU // 弹性结构体
{
    uint uiPDULen;   //总协议数据单元(PDU)大小
    uint uiMsgType;  //消息类型
    char caData[64]; //文件其他信息（文件名等）
    uint uiMsgLen;   //实际消息大小
    //上面大小不变
    //下面大小变化
    int caMsg[];     //实际消息
};

PDU *mkPDU(uint uiMsgLen); //动态空间申请函数

#endif // PROTOCOL_H
