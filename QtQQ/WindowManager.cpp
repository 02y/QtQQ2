#include "WindowManager.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <qsqlquerymodel.h>

Q_GLOBAL_STATIC(WindowManager, theInstance);

WindowManager::WindowManager()
{
}

WindowManager::~WindowManager()
{
}

QWidget * WindowManager::findWindowName(const QString & qsWindowName)
{
	if (m_windowMap.contains(qsWindowName))
		return m_windowMap.value(qsWindowName);
	
	return Q_NULLPTR;
}

void WindowManager::deleteWindowName(const QString & qsWindowName)
{
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString & qsWindowName, QWidget * qWidget)
{
	if (!m_windowMap.contains(qsWindowName)) m_windowMap.insert(qsWindowName, qWidget);
}

WindowManager * WindowManager::getInstance()
{
	return theInstance();
}

TalkWindowShell * WindowManager::getTalkWindowShell()
{
	return m_talkwindowshell;
}

void WindowManager::addNewTalkWindow(const QString & uid)
{
	if (m_talkwindowshell == Q_NULLPTR)
	{
		m_talkwindowshell = new TalkWindowShell;
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* obj)
			{
				m_talkwindowshell = Q_NULLPTR;
			});
	}

	QWidget* widget = findWindowName(uid);
	if (!widget)
	{
		m_strCreatingTalkID = uid;
		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid);
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);
		m_strCreatingTalkID = "";

		QSqlQueryModel queryModel;
		queryModel.setQuery(QString("SELECT department_name,sign,picture FROM tab_department WHERE departmentID = %1").arg(uid));
		if (0 == queryModel.rowCount())
		{
			queryModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = %1").arg(uid));
		}

		QModelIndex nameIndex, signIndex, picIndex;
		nameIndex = queryModel.index(0, 0);
		signIndex = queryModel.index(0, 1);
		picIndex = queryModel.index(0, 2);

		QString strName, strSign, strPic;
		strName = queryModel.data(nameIndex).toString();
		strSign = queryModel.data(signIndex).toString();
		strPic = queryModel.data(picIndex).toString();
		talkwindow->setWindowName(strSign);
		talkwindowItem->setMsgLabelContent(strName);
		talkwindowItem->setHeadPixmap(QPixmap(strPic));

		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, uid);
	}
	else
	{
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();
}

QString WindowManager::getCreatingTalkID()
{
	return m_strCreatingTalkID;
}
