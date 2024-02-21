#ifndef BOOK_H
#define BOOK_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(PDU* pdu);
    void clearEnterDir();
    QString EnterDir();

signals:

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void renameFile();
    void enterDir(const QModelIndex &index);
    void returnpre();

private:
    QListWidget *m_pBookListW;  //列表
    QPushButton *m_pReturnPB;    //返回按钮
    QPushButton *m_pCreateDirPB;   //创建文件夹按钮
    QPushButton *m_pDelDirPB;   //删除文件夹按钮
    QPushButton *m_pRenamePB;  //重命名文件夹按钮
    QPushButton *m_pFlushPB;   //刷新文件夹按钮

    QPushButton *m_pUpLoadPB;   //上传文件按钮
    QPushButton *m_pDownLoadPB;   //删除文件按钮
    QPushButton *m_pDelFilePB;   //下载文件按钮
    QPushButton *m_pshareFilePB;   //分享文件按钮

    QString m_strEnterDir;
};

#endif // BOOK_H
