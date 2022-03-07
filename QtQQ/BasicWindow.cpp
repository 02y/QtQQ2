#include "BasicWindow.h"
#include <QFile>
#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QSqlQuery>
#include "CommonUtils.h"
#include "NotifyManager.h"

extern QString gLoginEmployeeID;

BasicWindow::BasicWindow(QWidget *parent)
	: QDialog(parent), m_mousePressed(false)
{
	m_colorBackGround = CommonUtils::getDefaultSkinColor();

	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);

	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, this, &BasicWindow::onSignalSkinChanged);
}

BasicWindow::~BasicWindow()
{
}

void BasicWindow::loadStyleSheet(const QString & sheetName)
{
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet("");
		QString styleSheet = file.readAll();

		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		styleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												border-top-left-radius:4px;}\
												QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												background-color:rgba(%1,%2,%3,50);\
												border-bottom-left-radius:4px;\
												border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);
		setStyleSheet(styleSheet);
	}
	file.close();
}

QPixmap BasicWindow::getRoundImage(const QPixmap & src, QPixmap & mask, QSize masksize)
{
	if (QSize(0, 0) == masksize) 
		masksize = mask.size();
	else 
		mask = mask.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	QImage img(masksize, QImage::Format_ARGB32_Premultiplied);
	QPainter p(&img);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.fillRect(img.rect(), Qt::transparent);
	p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	p.drawPixmap(0, 0, mask);
	p.setCompositionMode(QPainter::CompositionMode_SourceIn);
	p.drawPixmap(0, 0, src.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	p.end();

	return QPixmap::fromImage(img);
}

void BasicWindow::paintEvent(QPaintEvent * event)
{
	QStyleOption opt;
	opt.init(this);

	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

	QDialog::paintEvent(event);
}

void BasicWindow::mousePressEvent(QMouseEvent * event)
{
	if (Qt::LeftButton == event->button())
	{
		m_mousePressed = true;
		m_mousePoint = event->globalPos() - pos();
		event->accept();
	}
}

void BasicWindow::mouseMoveEvent(QMouseEvent * event)
{
	if (m_mousePressed && (event->buttons() & Qt::LeftButton))
	{
		move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

void BasicWindow::mouseReleaseEvent(QMoveEvent * event)
{
	m_mousePressed = false;
}

void BasicWindow::initTitleBar(ButtonType buttontype)
{
	m_titleBar = new TitleBar(this);
	m_titleBar->setButtonType(buttontype);
	m_titleBar->move(0, 0);

	connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));
}

void BasicWindow::setTitleBarTitle(const QString & title, const QString & icon)
{
	m_titleBar->setTitleIcon(icon);
	m_titleBar->setTitleContent(title);
}

void BasicWindow::onShowClose(bool)
{
	close();
}

void BasicWindow::onShowMin(bool)
{
	showMinimized();
}

void BasicWindow::onShowHide(bool)
{
	hide();
}

void BasicWindow::onShowNormal(bool)
{
	show();
	activateWindow();
}

void BasicWindow::onShowQuit(bool)
{
	QSqlQuery sqlQuery(QString("UPDATE tab_employees SET online = 2 WHERE employeeID = %1").arg(gLoginEmployeeID));
	sqlQuery.exec();

	QApplication::quit();
}

void BasicWindow::onSignalSkinChanged(const QColor & color)
{
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);
}

void BasicWindow::onButtonMinClicked()
{
	if (Qt::Tool == (windowFlags() & Qt::Tool)) 
		hide();
	else 
		showMinimized();
}

void BasicWindow::onButtonRestoreClicked()
{
	QPoint point;
	QSize windowSize;

	m_titleBar->getRestoreInfo(point, windowSize);
	setGeometry(QRect(point, windowSize));
}

void BasicWindow::onButtonMaxClicked()
{
	m_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));

	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QRect factRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
	setGeometry(factRect);
}

void BasicWindow::onButtonCloseClicked()
{
	close();
}
