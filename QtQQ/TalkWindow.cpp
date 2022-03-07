#include "TalkWindow.h"
#include "WindowManager.h"
#include "SendFile.h"
#include "qsqlquery.h"
#include <qtooltip.h>
#include <qfile.h>
#include <qmessagebox.h>
#include "RootContactItem.h"
#include <qsqlquerymodel.h>
#include "ContactItem.h"

extern QString gLoginEmployeeID;

TalkWindow::TalkWindow(QWidget *parent, const QString& uid)
	: QWidget(parent), m_talkId(uid)
{
	ui.setupUi(this);
	
	WindowManager::getInstance()->addWindowName(m_talkId, this);
	setAttribute(Qt::WA_DeleteOnClose);

	initGroupTalkStatus();
	initControl();
}

TalkWindow::~TalkWindow()
{
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	ui.textEdit->setFocus();
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::setWindowName(const QString & name)
{
	ui.nameLabel->setText(name);
}

QString TalkWindow::getTalkId()
{
	return m_talkId;
}

void TalkWindow::onSendBtnClicked(bool)
{
	if (ui.textEdit->toPlainText().isEmpty())
	{
		QToolTip::showText(this->mapToGlobal(QPoint(630, 660)), QString::fromLocal8Bit("发送的信息不能为空！"), this,
			QRect(0, 0, 120, 100), 2000);
		return;
	}

	QString&& html = ui.textEdit->document()->toHtml();

	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString text = ui.textEdit->toPlainText();
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = QLatin1String(file.readAll());
			fontHtml.replace("%1", text);
			file.close();
		}
		else
		{
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件 msgFont.txt 不存在！"));
			return;
		}

		if (!html.contains(fontHtml)) html.replace(text, fontHtml);
	}

	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();
	ui.msgWidget->appendMsg(html);
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem * item, int colunm)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild)
	{
		QString talkId = item->data(0, Qt::UserRole + 1).toString();

		if (talkId == gLoginEmployeeID) return;
		WindowManager::getInstance()->addNewTalkWindow(talkId);
	}
}

void TalkWindow::initControl()
{
	QList<int> bodySplitterSizes;
	bodySplitterSizes << 600 << 128;
	ui.bodySplitter->setSizes(bodySplitterSizes);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();

	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));

	if (m_isGroupTalk)
		initTalkWindow();
	else
		initPtoPTalk();
}

void TalkWindow::initGroupTalkStatus()
{
	QSqlQuery query(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(m_talkId));
	query.exec();

	if (query.next()) m_isGroupTalk = true;
	else m_isGroupTalk = false;
}

void TalkWindow::initTalkWindow()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setData(0, Qt::UserRole, 0);

	RootContactItem* pRoot = new RootContactItem(false, ui.treeWidget);
	ui.treeWidget->setFixedHeight(646);

	QString strDepGroupName;
	QSqlQuery queryDepName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(m_talkId));
	queryDepName.exec();
	if (queryDepName.next()) strDepGroupName = queryDepName.value(0).toString();

	QSqlQueryModel queryEmployeeNum;
	if (getCompDepID() == m_talkId.toInt())
	{
		queryEmployeeNum.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1"));
	}
	else
	{
		queryEmployeeNum.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(m_talkId));
	}

	int employeeNum = queryEmployeeNum.rowCount();
	QString pRootText = QString::fromLocal8Bit("%1 %2/%3").arg(strDepGroupName).arg(0).arg(employeeNum);

	pRoot->setText(pRootText);
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pRoot);
	pRootItem->setExpanded(true);

	for (int i = 0; i < employeeNum; i++)
	{
		QModelIndex indexEmployeeId;
		indexEmployeeId = queryEmployeeNum.index(i, 0);
		int employeeID = queryEmployeeNum.data(indexEmployeeId).toInt();

		addPeopInfo(pRootItem, employeeID);
	}
}

void TalkWindow::initPtoPTalk()
{
	QPixmap skin(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(skin.size());
	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(skin);
	skinLabel->setFixedSize(ui.widget->size());
}

int TalkWindow::getCompDepID()
{
	QSqlQuery queryCompId(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("公司群")));
	queryCompId.exec();
	queryCompId.next();

	return queryCompId.value(0).toInt();
}

void TalkWindow::addPeopInfo(QTreeWidgetItem * pRootGroupItem, int employeeID)
{
	QTreeWidgetItem* pChildItem = new QTreeWidgetItem;
	pChildItem->setData(0, Qt::UserRole, 1);
	pChildItem->setData(0, Qt::UserRole + 1, employeeID);

	ContactItem* pChild = new ContactItem(ui.treeWidget);
	
	QSqlQueryModel queryEmployeeInfo;
	queryEmployeeInfo.setQuery(QString("SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = %1").arg(employeeID));

	QModelIndex nameIndex, signIndex, picIndex;
	nameIndex = queryEmployeeInfo.index(0, 0);
	signIndex = queryEmployeeInfo.index(0, 1);
	picIndex = queryEmployeeInfo.index(0, 2);

	QString name, sign, pic;
	name = queryEmployeeInfo.data(nameIndex).toString();
	sign = queryEmployeeInfo.data(signIndex).toString();
	pic = queryEmployeeInfo.data(picIndex).toString();

	pChild->setUserName(name);
	pChild->setSignName(sign);
	pChild->setHeadPixmap(QPixmap(pic));

	pRootGroupItem->addChild(pChildItem);
	ui.treeWidget->setItemWidget(pChildItem, 0, pChild);
	m_groupPeopleMap.insert(pChildItem, name);
}

void TalkWindow::onFileOpenBtnClicked(bool)
{
	SendFile* sendfile = new SendFile(this);
	sendfile->show();
}
