#include "SqlThread.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QSqlError>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QSqlQueryModel>

SqlThread::SqlThread(QObject* parent)
    :QThread(parent)
{

}

void SqlThread::run()
{
    if(!connectMysql()) return;

    const QString& path = QApplication::applicationDirPath() + "/mysql/" + QString("insertMysql.txt");
    QFile insertFile(path);
    QString str;
    if(insertFile.open(QIODevice::ReadOnly))
    {
        str = QLatin1String(insertFile.readAll());
    }
    else
    {
        insertFile.close();
        emit openFileErr("insertMysql.txt");
        return;
    }
    insertFile.close();

    QStringList strList;
    strList = str.split(";");
    //qDebug() << strList.count();   //23

    QSqlQuery insertQuery = QSqlQuery(db);
    for(int i = 1; i <= strList.count(); i++)
    {
        insertQuery.exec(strList.at(i - 1).toLatin1());
        //qDebug() << i;
        emit updateProgress(40 + i * 2);
    }
    insertDepPicture();
    insertEmpPicture();

    db.close();
}

bool SqlThread::connectMysql()
{
    const QString& path = QApplication::applicationDirPath() + "/mysql/" + QString("MysqlInFo.ini");
    QSettings settings(path, QSettings::IniFormat);
    QStringList list;

    list << settings.value("MysqlInfo/username").toString();
    list << settings.value("MysqlInfo/password").toString();
    list << settings.value("MysqlInfo/port").toString();
    list << settings.value("MysqlInfo/hostname").toString();

    db = QSqlDatabase::addDatabase("QMYSQL", "thread");
    db.setUserName(list[0]);
    db.setPassword(list[1]);
    db.setPort(list[2].toInt());
    db.setHostName(list[3]);
    db.setDatabaseName("qtqq1_Enterprise_Edition");
    if(!db.open())
    {
        MysqlError(db);
        return false;
    }
    return true;
}

void SqlThread::MysqlError(QSqlDatabase& db)
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
            const QString& str = current_data + " <thread>-> " + strErr + "\n";
            errFile.write(str.toUtf8());
        }
        else emit openLogErr();
    }
    emit openMysqlErr(strErr);
}

void SqlThread::insertDepPicture()
{
    const QString& path = QApplication::applicationDirPath() + "/images/";

    QStringList depName;
    depName << QString::fromLocal8Bit("公司群") << QString::fromLocal8Bit("人事部")
            << QString::fromLocal8Bit("研发部") << QString::fromLocal8Bit("市场部");
    QStringList depPicName;
    depPicName << QString::fromLocal8Bit("gsq.png") << QString::fromLocal8Bit("rsb.png")
               << QString::fromLocal8Bit("yfb.png") << QString::fromLocal8Bit("scb.png");

    QSqlQuery query = QSqlQuery(db);
    for(int i = 0; i < 4; i++)
    {
        QString picPath;
        picPath = path + depPicName[i];
        picPath.replace("/", "\\\\");
        picPath = picPath.toLatin1();

        query.exec(QString("UPDATE tab_department SET picture = '%1' WHERE department_name = '%2'")
                   .arg(picPath).arg(depName[i]));
        emit updateProgress(86 + i + 1);
    }

}

void SqlThread::insertEmpPicture()
{
    const QString& path = QApplication::applicationDirPath() + "/images/";

    QVector<int> empId;
    for(int i = 0; i < 10; i++)
    {
        empId.push_back(10000 + i + 1);
        //qDebug() << empId[i];
    }

    QList<QString> empPicName;
    empPicName << "r1.png" << "r2.png" << "r3.png" << "y1.png" << "y2.png" << "y3.png" << "s1.png"
               << "s2.png" << "s3.png" << "r4.png";

    QList<QString> empPicPath;
    for(int i = 0; i < 10; i++)
    {
        QString strPicPath;
        strPicPath = path + empPicName[i];
        strPicPath.replace("/", "\\\\");
        empPicPath << strPicPath.toLatin1();
    }

    QSqlQuery query = QSqlQuery(db);
    for(int i = 0; i < 10; i++)
    {
        query.exec(QString("UPDATE tab_employees SET picture = '%1' WHERE employeeID = %2")
                   .arg(empPicPath[i]).arg(empId[i]));
        emit updateProgress(90 + i + 1);
    }
}





