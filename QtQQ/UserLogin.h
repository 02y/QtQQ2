#pragma once

#include "BasicWindow.h"
#include "ui_UserLogin.h"

class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = Q_NULLPTR);
	~UserLogin();

private slots:
	void onLoginBtnClicked();

private:
	void initControl();
	bool connectMySql();        //连接数据库
	bool veryfyAccountCode();   //验证

private:
	Ui::UserLogin ui;
};
