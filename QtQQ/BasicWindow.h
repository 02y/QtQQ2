#pragma once

#include <QDialog>
#include "TitleBar.h"


class BasicWindow : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent = nullptr);
	virtual ~BasicWindow();

public:
	void loadStyleSheet(const QString& sheetName);          //������ʽ��

	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize = QSize(0, 0));    //��ȡԭͷ��

protected:
	void paintEvent(QPaintEvent* event);             //�����¼�
	void mousePressEvent(QMouseEvent* event);        //����¼�
	void mouseMoveEvent(QMouseEvent* event);         //����ƶ��¼�
	void mouseReleaseEvent(QMoveEvent* event);       //����ͷ��¼�

protected:
	void initTitleBar(ButtonType buttontype = MIN_BUTTON);                    //��ʼ��������
	void setTitleBarTitle(const QString& title, const QString& icon = "");    //���ñ�����

public slots:
	void onShowClose(bool);
	void onShowMin(bool);
	void onShowHide(bool);
	void onShowNormal(bool);
	void onShowQuit(bool);
	void onSignalSkinChanged(const QColor& color);

	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

protected:
	QPoint m_mousePoint;         //���λ��
	bool m_mousePressed;         //����Ƿ���
	QColor m_colorBackGround;    //����ɫ
	QString m_styleName;         //��ʽ�ļ�����
	TitleBar* m_titleBar;         //������
};
