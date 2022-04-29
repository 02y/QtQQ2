#ifndef SQLTHREAD_H
#define SQLTHREAD_H

#include <QThread>
#include <QSqlDatabase>

class SqlThread : public QThread
{
    Q_OBJECT
public:
    SqlThread(QObject* parent = Q_NULLPTR);
    void run() override;

signals:
    void openFileErr(QString fileName);
    void updateProgress(int value);
    void openMysqlErr(const QString& str);
    void openLogErr();

private:
    bool connectMysql();
    void MysqlError(QSqlDatabase& db);   //获取mysql出错信息并保存至 MysqlError.txt
    void insertDepPicture();
    void insertEmpPicture();
    QSqlDatabase db;
};

#endif // SQLTHREAD_H
