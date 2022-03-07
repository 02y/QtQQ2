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
	void setUserName(const QString& username);          //�û���
	void setLevelPixmap(int level);                     //�ȼ�
	void setHeadPixmap(const QString& headPath);        //ͷ��
	void setStatusMenuIcon(const QString& statusPath);  //����״̬
	QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);   //���Ӧ�ò���(appͼƬ·����app����������)
	void initContactTree();        //��ʼ����ϵ����

private:
	void initTimer();                //��ʼ���ȼ���ʱ��
	void initControl();              //��ʼ������
	QString getHeadPicturePath();    //��ȡͷ��·��
	void updateSeachStyle();         //����������ʽ
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);     //��ӹ�˾����

private:
	void resizeEvent(QResizeEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* event) override;    //�¼�����
	void mousePressEvent(QMouseEvent* event);

private slots:
	void onItemClicked(QTreeWidgetItem* item, int column);          //�����
	void onItemExpanded(QTreeWidgetItem* item);                     //��չ��
	void onItemCollapsed(QTreeWidgetItem* item);                    //������
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);    //��˫��
	void onAppIconClicked();

private:
	Ui::CCMainWindowClass ui;
};
