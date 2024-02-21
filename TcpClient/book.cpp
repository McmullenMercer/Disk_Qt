#include "book.h"
#include <QMessageBox>
#include  "tcpclient.h"
#include <QInputDialog>

Book::Book(QWidget *parent) : QWidget(parent)  //构造函数
{
    m_pBookListW = new QListWidget;  //列表
    m_pReturnPB = new QPushButton("返回");    //返回按钮
    m_pCreateDirPB = new QPushButton("创建文件夹");   //创建文件夹按钮
    m_pDelDirPB = new QPushButton("删除文件夹");   //删除文件夹按钮
    m_pRenamePB = new QPushButton("重命名文件夹");  //重命名文件夹按钮
    m_pFlushPB = new QPushButton("刷新文件");   //刷新文件夹按钮

    QVBoxLayout *pDirVBL = new QVBoxLayout; //垂直布局
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushPB);

    m_pUpLoadPB = new QPushButton("上传文件");   //上传文件按钮
    m_pDownLoadPB = new QPushButton("删除文件");   //删除文件按钮
    m_pDelFilePB = new QPushButton("下载文件");   //下载文件按钮
    m_pshareFilePB = new QPushButton("分享文件");   //分享文件按钮

    QVBoxLayout *pFileVBL = new QVBoxLayout; //垂直布局
    pFileVBL->addWidget(m_pUpLoadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pshareFilePB);


    QHBoxLayout *pMain = new QHBoxLayout; //水平布局
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool))
            , this,SLOT(createDir()));
    connect(m_pFlushPB,SIGNAL(clicked(bool))
            , this,SLOT(flushFile()));
    connect(m_pDelDirPB,SIGNAL(clicked(bool))
            , this,SLOT(delDir()));
    connect(m_pRenamePB,SIGNAL(clicked(bool))
            , this,SLOT(renameFile()));

}

void Book::updateFileList(PDU *pdu)
{
    if(pdu ==NULL) return;

    //清除原列表中的内容
    m_pBookListW->clear();


    FileInfo *pFileInfo = NULL;

    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    for(int i =0;i<iCount;i++)
    {
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;
        qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(pFileInfo->iFileType == 0) //文件夹
        {
            pItem->setIcon(QIcon(QPixmap("./map/dir.png")));
        }
        else if(pFileInfo->iFileType ==1) //常规文件
        {
            pItem->setIcon(QIcon(QPixmap("./map/reg.png")));
        }

        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);

    }
}


void Book::createDir()
{

    QString NewDir =  QInputDialog::getText(this,"新建文件夹","新文件夹名字"); //新文件夹名字

    if(NewDir.isEmpty())
    {
        QMessageBox::warning(this,"新建文件夹","文件夹名字为空!");
    }
    else
    {
        if(NewDir.size()>32)
        {
            QMessageBox::warning(this,"新建文件夹","文件夹名字过长!");
        }
        else
        {
            QString strName = TcpClient::getInstance().loginName();  //获取用户名

            QString strCurPath = TcpClient::getInstance().curPath(); //获取当前路径

            PDU * pdu =mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());// caData的前32位放名字
            strncpy(pdu->caData+32,NewDir.toStdString().c_str(),NewDir.size());// caData的后32位放新文件夹名字
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());//caMsg里放根路径

            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

            free(pdu);
            pdu = NULL;
        }
    }
}

void Book::flushFile()
{
    QString  strCurPath = TcpClient::getInstance().curPath(); //获取当前路径

    PDU *pdu = mkPDU(strCurPath.size()+1);  //将路径放在pdu的caMsg中
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*) pdu,pdu->uiPDULen);

    free(pdu);
    pdu = NULL;
}

void Book::delDir()
{
    QString  strCurPath = TcpClient::getInstance().curPath(); //获取当前路径
    //从窗口获取选中的文件名
    QListWidgetItem *item = m_pBookListW->currentItem();

    if( item == NULL)
    {
        QMessageBox::warning(this,"删除文件夹","未选中文件夹");
    }
    else
    {
        QString strDelName = item->text(); //获取要删除的文件夹名字

        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;

        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());  //要删除的文件夹名字放在caData中
        strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size()); //要删除的文件夹路径放在caMsg中

        TcpClient::getInstance().getTcpSocket().write((char*) pdu,pdu->uiPDULen);

        free(pdu);
        pdu = NULL;

    }
}

void Book::renameFile()
{
    QString  strCurPath = TcpClient::getInstance().curPath(); //获取当前路径

    QListWidgetItem *item = m_pBookListW->currentItem();

    if( item == NULL)
    {
        QMessageBox::warning(this,"重命名文件夹","未选中文件夹");
    }
    else
    {
        QString strOldName = item->text(); //获取要删除的文件夹名字
        QString strNewName = QInputDialog::getText(this,"重命名文件夹","请输入新文件夹名");
        if(!strNewName.isEmpty())
        {
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;

            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());  //旧名字放在caData的前32位
            strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size());  //新名字放在caData的后32位
            strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size()); //要删除的文件夹路径放在caMsg中

            TcpClient::getInstance().getTcpSocket().write((char*) pdu,pdu->uiPDULen);

            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::warning(this,"重命名文件夹","文件夹名不能为空");
        }
    }
}
