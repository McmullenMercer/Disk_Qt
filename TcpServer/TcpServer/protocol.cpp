#include "protocol.h"

//动态空间申请
PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen = sizeof (PDU) + uiMsgLen;
    PDU *pdu = (PDU *)malloc(uiPDULen);//malloc: C语言申请空间函数
    if(NULL == pdu)
    {
        exit(EXIT_FAILURE); //错误退出程序
    }
    memset(pdu,0,uiPDULen);   //数据初始化为0
    pdu->uiPDULen = uiPDULen;  //数据参数初始化
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
