//弹性结构体测试

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef  struct PDU
{
    int a;
    int b;
    int c;
    int d[];
}PDU;

int main()
{
    printf("%ld\n",sizeof(PDU));//只会输出确定类型的长度（3个int，是12）

    PDU *pdu = (PDU*)malloc(sizeof(PDU)+100*sizeof(int));

    pdu->a = 90;
    pdu->b = 89;
    pdu->c = 88;
    memcpy(pdu->d,"you jump i jump",16);


    printf("a=%d,b=%d,c=%d,%s\n",pdu->a,pdu->b,pdu->c,(char*)(pdu->d));
    free(pdu);
    pdu = NULL;
    return 0;
}
