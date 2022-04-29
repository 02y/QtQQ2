#ifndef UPDATESQL_H
#define UPDATESQL_H

#include "BasicWindow.h"
#include <QSqlDatabase>
#include "SqlThread.h"

namespace Ui {
class UpdateSql;
}

class UpdateSql : public BasicWindow
{
    Q_OBJECT

public:
    explicit UpdateSql(QWidget *parent = nullptr);
    ~UpdateSql();
    void MysqlError(QSqlDatabase& db);   //获取mysql出错信息并保存至 MysqlError.txt

private slots:
    void on_pushBtn_clicked();
    void threadOpenFileErr(QString fileName);
    void threadUpdateProgress(int value);
    void threadOpenMysqlErr(const QString& str);
    void threadOpenLogErr();

private:
    bool createTableValue();       //创建表
    bool insertTableValue();       //插入表数据
    void setProgressBarValue(int progress);      //设置进度条
    bool connectMySql(QStringList& list);        //连接数据库
    void removeMysqlIni();        //移除文件 MysqlInFo.ini

    Ui::UpdateSql *ui;
    SqlThread* mThread;
    bool mThreadOpenSqlErr;
    QSqlDatabase db;
};

#endif // UPDATESQL_H
