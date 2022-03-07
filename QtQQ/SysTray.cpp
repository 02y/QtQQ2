#include "SysTray.h"
#include "CustomMenu.h"

SysTray::SysTray(QWidget *parent)
	: QSystemTrayIcon(parent), m_parent(parent)
{
	initSystemTray();
	show();
}

SysTray::~SysTray()
{
}

void SysTray::initSystemTray()
{
	setToolTip(QStringLiteral("Qt-QQ"));
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));
	connect(this, &QSystemTrayIcon::activated, this, &SysTray::onIconActivated);
}

void SysTray::addSystrayMenu()
{
	CustomMenu* menu = new CustomMenu(m_parent);
	menu->addCustomMenu("onShow", ":/Resources/MainWindow/app/logo.ico", QStringLiteral("ÏÔÊ¾"));
	menu->addCustomMenu("onQuit", ":/Resources/MainWindow/app/page_close_btn_hover.png", QStringLiteral("ÍË³ö"));
	
	connect(menu->getAction("onShow"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowNormal(bool)));
	connect(menu->getAction("onQuit"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowQuit(bool)));
	menu->exec(QCursor::pos());
	menu->deleteLater();
}

void SysTray::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
	{
		m_parent->show();
		m_parent->activateWindow();
	}
	else if (reason == QSystemTrayIcon::Context)
	{
		addSystrayMenu();
	}
}
