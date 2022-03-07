#pragma once

#include <QtWidgets/QWidget>
#include "ui_CCMainWindow.h"
#include "BasicWindow.h"
#include <QTreeWidget>

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	CCMainWindow(QWidget *parent = Q_NULLPTR);

public:
	void setUserName(const QString& username);          //用户名
	void setLevelPixmap(int level);                     //等级
	void setHeadPixmap(const QString& headPath);        //头像
	void setStatusMenuIcon(const QString& statusPath);  //在线状态
	QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);   //添加应用部件(app图片路径，app部件对象名)
	void initContactTree();        //初始化联系人树

private:
	void initTimer();                //初始化等级计时器
	void initControl();              //初始化部件
	QString getHeadPicturePath();    //获取头像路径
	void updateSeachStyle();         //更新搜索样式
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);     //添加公司部门

private:
	void resizeEvent(QResizeEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* event) override;    //事件过滤
	void mousePressEvent(QMouseEvent* event);

private slots:
	void onItemClicked(QTreeWidgetItem* item, int column);          //被点击
	void onItemExpanded(QTreeWidgetItem* item);                     //被展开
	void onItemCollapsed(QTreeWidgetItem* item);                    //被收缩
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);    //被双击
	void onAppIconClicked();

private:
	Ui::CCMainWindowClass ui;
};
