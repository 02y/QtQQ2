#pragma once

#include <QWidget>
#include "ui_ContactItem.h"

class ContactItem : public QWidget
{
	Q_OBJECT

public:
	ContactItem(QWidget *parent = Q_NULLPTR);
	~ContactItem();

	void setUserName(const QString& userName);         //�����û���
	void setSignName(const QString& signName);         //����ǩ��
	void setHeadPixmap(const QPixmap& src);       //����ͷ��
	QString getUserName() const;                       //��ȡ�û���
	QSize getHeadLabelSize() const;                    //��ȡͷ��ߴ�

private:
	Ui::ContactItem ui;
};
