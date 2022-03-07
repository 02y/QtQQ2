#pragma once

#include <QWidget>
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"
#include <QTreeWidgetItem>

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent, const QString& uid);
	~TalkWindow();

public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
	QString getTalkId();

private slots:
	void onFileOpenBtnClicked(bool);   //�ļ��򿪱������ 
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item, int colunm);

private:
	void initControl();
	void initGroupTalkStatus();   //�Ƿ�ΪȺ��
	void initTalkWindow();        //��ʼ��Ⱥ��
	void initPtoPTalk();          //��ʼ������
	int getCompDepID();          //��ȡ��˾ȺID��

	void addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID);

private:
	Ui::TalkWindow ui;
	QString m_talkId;
	bool m_isGroupTalk;    //�Ƿ�ΪȺ��
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;    //���з�����ϵ������

	friend class TalkWindowShell;
};
