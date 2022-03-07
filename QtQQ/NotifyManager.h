#pragma once

#include <QObject>

class NotifyManager : public QObject
{
	Q_OBJECT

public:
	NotifyManager();
	~NotifyManager();

signals:
	void signalSkinChanged(const QColor& color);    //�ı���ɫ�ź�

public:
	static NotifyManager* getInstance();          //��ȡʵ��   ����

	void notifyOtherWindowChangeSkin(const QColor& color);       //֪ͨ�������ڽ���Ƥ����ɫ�ĸı�
};