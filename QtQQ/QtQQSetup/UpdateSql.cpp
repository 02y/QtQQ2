#include "UpdateSql.h"
#include "ui_UpdateSql.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDateTime>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include "UserLogin.h"


UpdateSql::UpdateSql(QWidget *parent) :
    BasicWindow(parent),
    ui(new Ui::UpdateSql)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    ui->QiNiu->setPixmap(QString(":/Resources/MainWindow/qn.png"));
    initTitleBar();
    loadStyleSheet("UpdateSql");
    mThreadOpenSqlErr = false;

    mThread = new SqlThread(this);
    connect(mThread, &SqlThread::openFileErr, this, &UpdateSql::threadOpenFileErr);
    connect(mThread, &SqlThread::updateProgress, this, &UpdateSql::threadUpdateProgress);
    connect(mThread, &SqlThread::openMysqlErr, this, &UpdateSql::threadOpenMysqlErr);
    connect(mThread, &SqlThread::openLogErr, this, &UpdateSql::threadOpenLogErr);
}

UpdateSql::~UpdateSql()
{
    delete ui;
}

bool UpdateSql::connectMySql(QStringList& list)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "main");
    db.setUserName(list[0]);
    db.setPassword(list[1]);
    db.setPort(list[2].toInt());
    db.setHostName(list[3]);
    if(!db.open())
    {
        MysqlError(db);
        return false;
    }
    db.exec("CREATE database if not exists qtqq1_Enterprise_Edition");    //不存在则创建database
    db.setDatabaseName("qtqq1_Enterprise_Edition");

    if(!db.open())     //需要再次打开数据库
    {
        MysqlError(db);
        return false;
    }
    else
    {
        const QString& path = QApplication::applicationDirPath() + "/mysql/" + QString("MysqlInFo.ini");
        QSettings settings(path, QSettings::IniFormat);
        settings.setValue("MysqlInfo/username", list[0]);
        settings.setValue("MysqlInfo/password", list[1]);
        settings.setValue("MysqlInfo/port", list[2].toInt());
        settings.setValue("MysqlInfo/hostname", list[3]);
        this->db = db;

        return true;
    }
}

void UpdateSql::MysqlError(QSqlDatabase& db)
{

    QSqlError err = db.lastError();    //返回数据库出错信息
    const QString& strErr = err.text();
    const QString& path = QApplication::applicationDirPath() + "/mysql/" + QString("MysqlError.txt");
    QFile errFile(path);
    if(strErr.length())
    {
        if(errFile.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            QDateTime currentDataTime = QDateTime::currentDateTime();
            const QString& current_data = currentDataTime.toString("yyyy.MM.dd hh:mm:ss");
            const QString& str = current_data + " -> " + strErr + "\n";
            errFile.write(str.toUtf8());
        }
        else QMessageBox::information(Q_NULLPTR, "INFO", QString::fromLocal8Bit("MysqlError.txt 打开失败!"));
    }
    QMessageBox::warning(this, "Warning", QString::fromLocal8Bit(strErr.toUtf8()));
}

void UpdateSql::on_pushBtn_clicked()
{
    QStringList list;
    QString strAcc = ui->accLineEdit->text();    //账号
    QString strPwd = ui->pwdLineEdit->text();    //密码
    if(!strAcc.length() || !strPwd.length())
    {
        QMessageBox::information(Q_NULLPTR, "InFo", QString::fromLocal8Bit("请输入数据库账号或密码"));
        return;
    }
    list << strAcc << strPwd;

    QString strPort = ui->portLineEdit->text();       //端口号
    QString strHostName = ui->addrLineEdit->text();   //主机名
    if(!strPort.length() || !strHostName.length())
    {
        QMessageBox::information(Q_NULLPTR, "InFo", QString::fromLocal8Bit("请检查端口号或主机名"));
        return;
    }
    list << strPort << strHostName;


    if(!connectMySql(list))     //打开数据库，打开失败
    {
        this->setProgressBarValue(1);
        return;
    }
    else
    {
        this->setProgressBarValue(15);

        QMessageBox::StandardButton btn = QMessageBox::warning(this, "Warning",
                             QString::fromLocal8Bit("请注意：\n"
                                                    "接下来将对您的MySQL数据库进行创建库、创建表、插入数据操作\n"
                                                    "请确定您是否希望完成该操作"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(btn == QMessageBox::Yes)
        {
            if(createTableValue())
            {
                insertTableValue();
                if(mThread->wait())     //线程必须从run返回
                {
                    if(!mThreadOpenSqlErr)
                    {
                        db.close();
                        this->close();
                        UserLogin* userLogin = new UserLogin;     //进入登陆界面
                        userLogin->show();
                    }
                }
                else removeMysqlIni();
            }
        }
        else removeMysqlIni();
        return;
    }
}

void UpdateSql::threadOpenFileErr(QString fileName)
{
    QString str = fileName + QString::fromLocal8Bit("打开失败！");
    QMessageBox::information(Q_NULLPTR, "INFO", str.toUtf8());
}

void UpdateSql::threadUpdateProgress(int value)
{
    setProgressBarValue(value);
}

void UpdateSql::threadOpenMysqlErr(const QString& str)
{
    mThreadOpenSqlErr = true;
    QMessageBox::warning(this, "Thread Warning", QString::fromLocal8Bit(str.toUtf8()));
}

void UpdateSql::threadOpenLogErr()
{
    QMessageBox::information(Q_NULLPTR, "Thread INFO", QString::fromLocal8Bit("MysqlError.txt 打开失败!"));
}

void UpdateSql::removeMysqlIni()
{
    const QString& path = QApplication::applicationDirPath() + "/mysql/" + QString("MysqlInFo.ini");
    QDir dir;
    bool ok = dir.remove(path);
    if(ok) QMessageBox::information(Q_NULLPTR, "INFO", QString::fromLocal8Bit("MysqlInFo.ini已成功移除！"));
}

bool UpdateSql::createTableValue()
{
    QStringList strList;
    strList << "tab_account" << "tab_department" << "tab_employees";

    QSqlQuery query = QSqlQuery(db);
    for(int i = 0; i < 3; i++)       //创建表之前先查询表是否存在，如果存在则删除表
    {
        query.exec(QString("DROP table IF EXISTS %1").arg(strList.at(i)));
    }

    query.exec(QString("CREATE table IF NOT EXISTS %1(employeeID int NOT NULL, account char(20) PRIMARY KEY NOT NULL, "
                       "code char(20) NOT NULL)")
               .arg(strList.at(0)));
    query.exec(QString("CREATE table IF NOT EXISTS %1(departmentID int PRIMARY KEY AUTO_INCREMENT NOT NULL, "
                       "department_name char(20) NOT NULL, picture char(100) DEFAULT NULL,"
                       "sign char(50) NOT NULL)")
               .arg(strList.at(1)));
    query.exec(QString("CREATE table IF NOT EXISTS %1(departmentID int NOT NULL, "
                       "employeeID int PRIMARY KEY AUTO_INCREMENT NOT NULL,"
                       "employee_name char(20) NOT NULL, employee_sign char(200) DEFAULT NULL,"
                       "status tinyint DEFAULT 1 NOT NULL, picture char(50) DEFAULT NULL,"
                       "online tinyint DEFAULT 1 NOT NULL)")
               .arg(strList.at(2)));

    setProgressBarValue(40);
    return true;
}

bool UpdateSql::insertTableValue()
{
    mThread->start();

    return true;
}

void UpdateSql::setProgressBarValue(int progress)
{
    ui->progressBar->setValue(progress);
}





