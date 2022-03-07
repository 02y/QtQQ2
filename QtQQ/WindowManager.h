#pragma once

#include <QObject>
#include "TalkWindowShell.h"


class WindowManager : public QObject
{
	Q_OBJECT

public:
	WindowManager();
	~WindowManager();

public:
	QWidget* findWindowName(const QString& qsWindowName);         //���Ҵ�����
	void deleteWindowName(const QString& qsWindowName);           //ɾ��������
	void addWindowName(const QString& qsWindowName, QWidget* qWidget);          //��Ӵ�����

	static WindowManager* getInstance();          //����

	TalkWindowShell* getTalkWindowShell();

	void addNewTalkWindow(const QString& uid);    //����´���
	QString getCreatingTalkID();

private:
	TalkWindowShell* m_talkwindowshell;
	QMap<QString, QWidget*> m_windowMap;
	QString m_strCreatingTalkID = "";     //���ڴ��������촰�ڣ�QQ�ţ�
};
