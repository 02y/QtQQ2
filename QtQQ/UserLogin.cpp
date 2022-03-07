#include "UserLogin.h"
#include <QLabel>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include "CCMainWindow.h"

QString gLoginEmployeeID;

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{
}

void UserLogin::onLoginBtnClicked()
{
	if (!veryfyAccountCode())
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("账号或密码输入有误"));
		return;
	}

	QSqlQuery updateOnline(QString("UPDATE tab_employees SET online = 1 WHERE employeeID = %1").arg(gLoginEmployeeID));
	updateOnline.exec();
	close();

	CCMainWindow* mainWindow = new CCMainWindow();
	mainWindow->show();
}

void UserLogin::initControl()
{
	QLabel* label = new QLabel(this);
	label->setFixedSize(68, 68);

	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	label->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, label->size()));
	label->move(width() / 2 - 34, ui.titleWidget->height() - 34);

	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	if (!connectMySql())
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("连接数据库失败！"));
		close();
	}
}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("qtqq");
	db.setUserName("Gusu");
	db.setPassword("zzh1009..");
	db.setHostName("localhost");
	db.setPort(3306);

	if (db.open())
		return true;
	else
		return false;
}

bool UserLogin::veryfyAccountCode()
{
	QString strAccount = ui.editUserAccount->text();
	QString strCode = ui.editPassword->text();

	QSqlQuery sqlQueryId(QString("SELECT code FROM tab_account WHERE employeeID = %1").arg(strAccount));
	sqlQueryId.exec();
	if (sqlQueryId.next())
	{
		if (strCode == sqlQueryId.value(0).toString())
		{
			gLoginEmployeeID = strAccount;
			return true;
		}
		else return false;
	}
	
	QSqlQuery sqlQueryAcc(QString("SELECT code,employeeID FROM tab_account WHERE account = '%1'").arg(strAccount));
	sqlQueryAcc.exec();
	if (sqlQueryAcc.next())
	{
		if (strCode == sqlQueryAcc.value(0).toString())
		{
			gLoginEmployeeID = sqlQueryAcc.value(1).toString();
			return true;
		}
		else return false;
	}

	return false;
}
