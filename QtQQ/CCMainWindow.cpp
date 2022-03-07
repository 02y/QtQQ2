#include "CCMainWindow.h"
#include <QTimer>
#include <QSqlQuery>
#include <QProxyStyle>
#include <QPainter>
#include <QMouseEvent>
#include "NotifyManager.h"
#include "SysTray.h"
#include "SkinWindow.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "WindowManager.h"


QString gstrLoginHeadPath;
extern QString gLoginEmployeeID;

class CustomProxyStyle : public QProxyStyle
{
public:
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* opt, QPainter* p, const QWidget* widget = Q_NULLPTR)
	{
		if (PE_FrameFocusRect == element) return;
		else QProxyStyle::drawPrimitive(element, opt, p, widget);
	}
};

CCMainWindow::CCMainWindow(QWidget *parent)
    : BasicWindow(parent)
{
    ui.setupUi(this);
	loadStyleSheet("CCMainWindow");

	initControl();
	initTimer();
}

void CCMainWindow::setUserName(const QString & username)
{
	ui.nameLabel->adjustSize();
	QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());
	ui.nameLabel->setText(name);
}

void CCMainWindow::setLevelPixmap(int level)
{
	QPixmap levelIcon(ui.levelBtn->size());
	levelIcon.fill(Qt::transparent);

	QPainter p(&levelIcon);
	p.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

	int tenNum = level / 10;
	int unitNum = level % 10;

	p.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), tenNum * 6, 0, 6, 7);
	p.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);
	p.end();

	ui.levelBtn->setIconSize(ui.levelBtn->size());
	ui.levelBtn->setIcon(levelIcon);
}

void CCMainWindow::setHeadPixmap(const QString & headPath)
{
	QPixmap head_mask(":/Resources/MainWindow/head_mask.png");
	ui.headLbael->setPixmap(getRoundImage(QPixmap(headPath), head_mask, ui.headLbael->size()));
}

void CCMainWindow::setStatusMenuIcon(const QString & statusPath)
{
	QPixmap pixmap(ui.statusBtn->size());
	pixmap.fill(Qt::transparent);
	
	QPainter p(&pixmap);
	p.drawPixmap(4, 4, QPixmap(statusPath));
	p.end();

	ui.statusBtn->setIconSize(ui.statusBtn->size());
	ui.statusBtn->setIcon(pixmap);
}

QWidget * CCMainWindow::addOtherAppExtension(const QString & appPath, const QString & appName)
{
	QPushButton* btn = new QPushButton(this);
	btn->setFixedSize(20, 20);
	
	QPixmap icon(btn->size());
	icon.fill(Qt::transparent);
	QPainter painter(&icon);
	QPixmap appPixmap(appPath);
	painter.drawPixmap((btn->width() - appPixmap.width()) / 2, (btn->height() - appPixmap.height()) / 2, appPixmap);
	painter.end();

	btn->setIconSize(btn->size());
	btn->setIcon(icon);
	btn->setObjectName(appName);
	btn->setProperty("hasborder", true);

	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);
	return btn;
}

void CCMainWindow::initContactTree()
{
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	pRootItem->setData(0, Qt::UserRole, 0);

	RootContactItem* pRoot = new RootContactItem(true, ui.treeWidget);
	pRoot->setText(QString::fromLocal8Bit("奇牛科技"));

	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("公司群")));
	queryCompDepID.exec();
	queryCompDepID.next();
	int CompDepID = queryCompDepID.value(0).toInt();

	QSqlQuery queryOtherDepId(QString("SELECT departmentID FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
	queryOtherDepId.exec();
	queryOtherDepId.next();
	int OtherDepID = queryOtherDepId.value(0).toInt();

	addCompanyDeps(pRootItem, CompDepID);
	addCompanyDeps(pRootItem, OtherDepID);

	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pRoot);
}

void CCMainWindow::initTimer()
{
	QTimer* timer = new QTimer(this);
	timer->setInterval(2000);	
	connect(timer, &QTimer::timeout, [this]()
		{
			static int level = 0;
			if (99 == level) level = 0;
			level++;
			setLevelPixmap(level);
		});

	timer->start();
}

void CCMainWindow::initControl()
{
	ui.treeWidget->setStyle(new CustomProxyStyle);
	setHeadPixmap(getHeadPicturePath());
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded");
	setLevelPixmap(0);

	QHBoxLayout* appupLayout = new QHBoxLayout(ui.appWidget);
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
	appupLayout->setContentsMargins(0, 0, 0, 0);
	appupLayout->setSpacing(2);
	appupLayout->addStretch();
	
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
	ui.bottomLayout_up->addStretch();

	initContactTree();
	ui.lineEdit->installEventFilter(this);
	ui.searchLineEdit->installEventFilter(this);

	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));
	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
		{
			updateSeachStyle();
		});
	
	SysTray* systray = new SysTray(this);
}

QString CCMainWindow::getHeadPicturePath()
{
	QSqlQuery sqlQueryPic(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
	sqlQueryPic.exec();
	sqlQueryPic.next();
	QString headPath = sqlQueryPic.value(0).toString();
	gstrLoginHeadPath = headPath;

	return headPath;
}

void CCMainWindow::updateSeachStyle()
{
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)} \
                                            QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
		.arg(m_colorBackGround.red()).arg(m_colorBackGround.green()).arg(m_colorBackGround.blue()));
}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem * pRootGroupItem, int DepID)
{
	QTreeWidgetItem* pChildItem = new QTreeWidgetItem;
	pChildItem->setData(0, Qt::UserRole, 1);
	pChildItem->setData(0, Qt::UserRole + 1, DepID);

	QSqlQuery queryDepInfo(QString("SELECT picture, department_name FROM tab_department WHERE departmentID = %1").arg(DepID));
	queryDepInfo.exec();
	queryDepInfo.next();
	QString DepName = queryDepInfo.value(1).toString();
	QString DepPic = queryDepInfo.value(0).toString();

	ContactItem* pChild = new ContactItem(ui.treeWidget);
	pChild->setUserName(DepName);
	pChild->setHeadPixmap(DepPic);

	pRootGroupItem->addChild(pChildItem);
	ui.treeWidget->setItemWidget(pChildItem, 0, pChild);
}

void CCMainWindow::resizeEvent(QResizeEvent * event)
{
	setUserName(QString::fromLocal8Bit("星星亦无痕"));
	return BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject * obj, QEvent * event)
{
	if (obj == ui.searchLineEdit)
	{
		if (event->type() == QEvent::FocusIn)
		{
			ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255); \
													border-bottom:1px solid rgba(%1,%2,%3,100)} \
                                                    QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)} \
													QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)} \
													QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")
				.arg(m_colorBackGround.red()).arg(m_colorBackGround.green()).arg(m_colorBackGround.blue()));
		}
		else if (event->type() == QEvent::FocusOut)
		{
			updateSeachStyle();
		}
	}

	return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent * event)
{
	if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus())
		ui.searchLineEdit->clearFocus();
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus())
		ui.lineEdit->clearFocus();
		
	return BasicWindow::mousePressEvent(event);
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		item->setExpanded(!item->isExpanded());
	}
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem * item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		RootContactItem* pRootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (pRootItem) pRootItem->setExpanded(true);
	}
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem * item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		RootContactItem* pRootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (pRootItem) pRootItem->setExpanded(false);
	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem * item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild)
	{
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
	}
}

void CCMainWindow::onAppIconClicked()
{
	if (sender()->objectName() == "app_skin")
	{
		SkinWindow* skin = new SkinWindow;
		skin->show();
	}
}
