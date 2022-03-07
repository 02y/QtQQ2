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
	void onFileOpenBtnClicked(bool);   //文件打开被点击了 
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item, int colunm);

private:
	void initControl();
	void initGroupTalkStatus();   //是否为群聊
	void initTalkWindow();        //初始化群聊
	void initPtoPTalk();          //初始化单聊
	int getCompDepID();          //获取公司群ID号

	void addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID);

private:
	Ui::TalkWindow ui;
	QString m_talkId;
	bool m_isGroupTalk;    //是否为群聊
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;    //所有分组联系人姓名

	friend class TalkWindowShell;
};
