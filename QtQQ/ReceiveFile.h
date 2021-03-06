#pragma once

#include <QWidget>
#include "ui_ReceiveFile.h"
#include "BasicWindow.h"

class ReceiveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceiveFile(QWidget *parent = Q_NULLPTR);
	~ReceiveFile();
	void setMsg(QString& msgLabel);

signals:
	void refuseFile();

private slots:
	void on_cancelBtn_clicked();
	void on_okBtn_clicked();

private:
	Ui::ReceiveFile ui;
};