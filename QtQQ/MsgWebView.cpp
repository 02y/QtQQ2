#include "MsgWebView.h"
#include <qfile.h>
#include <qmessagebox.h>
#include <qwebchannel.h>
#include <qsqlquerymodel.h>
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <qsqlquery.h>
#include <qjsonobject.h>
#include <qjsondocument.h>

extern QString gstrLoginHeadPath;

/*   MsgHtmlObj   */

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString msgLPicPath)
	:QObject(parent), m_msgLPicPath(msgLPicPath)
{
	initHtmlTmpl();
}

void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);

	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
}

QString MsgHtmlObj::getMsgTmplHtml(const QString & code)
{
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	QString strContent;
	if (file.open(QIODevice::ReadOnly))
	{
		strContent = QLatin1String(file.readAll());
		file.close();
	}
	else
	{
		QMessageBox::information(nullptr, "Tips", "Faild to init html!");
		return QString("");
	}

	return strContent;
}



/*   MsgWebPage   */

bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	if (url.scheme() == "qrc")
	{
		return true;
	}
	return false;
}



/*   MsgWebView   */

MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent), m_channel(new QWebChannel(this))
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	m_msgHtmlObj = new MsgHtmlObj(this);
	m_channel->registerObject("external0", m_msgHtmlObj);

	TalkWindowShell* talkwindowshell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkwindowshell, &TalkWindowShell::updateSendTcpMsg);

	QString strTalkId = WindowManager::getInstance()->getCreatingTalkID();     //正在创建的TalkID

	QSqlQuery queryComDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QStringLiteral("公司群")));
	queryComDepID.exec();
	queryComDepID.next();
	QString comDepID = queryComDepID.value(0).toString();

	QSqlQueryModel queryInfoModel;
	bool isGroup = false;
	if (comDepID == strTalkId)
	{
		isGroup = true;
		queryInfoModel.setQuery("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
	}
	else
	{
		if (4 == strTalkId.length())
		{
			isGroup = true;
			queryInfoModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(strTalkId));
		}
		else
		{
			queryInfoModel.setQuery(QString("SELECT picture FROM tab_employees WHERE status = 1 AND employeeID = %1").arg(strTalkId));
		}
	}

	QString strEmployeeId, strPic, strExternal;
	if (isGroup)
	{
		QModelIndex employeeIdIndex, picIndex;
		int rows = queryInfoModel.rowCount();

		for (int i = 0; i < rows; i++)
		{
			employeeIdIndex = queryInfoModel.index(i, 0);
			picIndex = queryInfoModel.index(i, 1);

			strEmployeeId = queryInfoModel.data(employeeIdIndex).toString();
			strPic = queryInfoModel.data(picIndex).toString();

			strExternal = "external_" + strEmployeeId;
			MsgHtmlObj* obj = new MsgHtmlObj(this, strPic);
			m_channel->registerObject(strExternal, obj);
		}
	}
	else
	{
		QModelIndex picIndex;
		picIndex = queryInfoModel.index(0, 0);
		strPic = queryInfoModel.data(picIndex).toString();

		strExternal = "external_" + strTalkId;
		MsgHtmlObj* obj = new MsgHtmlObj(this, strPic);
		m_channel->registerObject(strExternal, obj);
	}

	this->page()->setWebChannel(m_channel);
	this->page()->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{
}

void MsgWebView::appendMsg(const QString & html, QString strObj)
{
	const QList<QStringList> msgList = parseHtml(html);
	int msgType = 1;
	int imageNum = 0;
	bool isImageMsg = false;
	QString strData;
	QString strMsg;

	for (int i = 0; i < msgList.count(); ++i)
	{
		if (msgList.at(i).at(0) == "img")
		{
			QString strImagePath = msgList.at(i).at(1);
			QString strPrefix = "qrc:/Resources/MainWindow/emotion/";
			int midPos = strPrefix.length();
			QString strEmotionNum = strImagePath.mid(midPos);
			strEmotionNum.replace(".png", "");

			msgType = 0;
			imageNum++;
			isImageMsg = true;

			int emotionLenth = strEmotionNum.length();
			if (1 == emotionLenth) strData = strData + "00" + strEmotionNum;
			else if (2 == emotionLenth) strData = strData + "0" + strEmotionNum;
			else strData = strData + strEmotionNum;

			QPixmap emotionPix;
			if (strImagePath.left(3) == "qrc") emotionPix.load(strImagePath.mid(3));
			else emotionPix.load(strImagePath);

			QString imgHtml = QString("<img src=\"%1\" width=\"%2\" heigth=\"%3\"/>")
				.arg(strImagePath).arg(emotionPix.width()).arg(emotionPix.height());
			strMsg += imgHtml;
		}
		else if (msgList.at(i).at(0) == "text")
		{
			strData += msgList.at(i).at(1);
			strMsg += strData;
		}
	}

	QJsonObject obj;
	obj.insert("MSG", strMsg);
	const QString& Msg = QJsonDocument(obj).toJson(QJsonDocument::Compact);

	if (strObj == "0")
	{
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));
		
		if (isImageMsg)
		{
			strData = QString::number(imageNum) + "images" + strData;
		}
		emit signalSendMsg(strData, msgType);
	}
	else this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
}

QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
	QDomDocument doc;
	doc.setContent(html);
	const QDomNode& root = doc.documentElement();
	const QDomElement& element = root.firstChildElement("body");

	return parseDocNode(element);
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode & node)
{
	const QDomNodeList& list = node.childNodes();
	QList<QStringList> attributeList;

	for (int i = 0; i < list.count(); i++)
	{
		const QDomNode& node = list.at(i);
		if (node.isElement())
		{
			const QDomElement& element = node.toElement();
			if (element.tagName() == "img")
			{
				QStringList attribute;
				attribute << "img" << element.attribute("src");
				attributeList << attribute;
			}

			if (element.tagName() == "span")
			{
				QStringList attribute;
				attribute << "text" << element.text();
				attributeList << attribute;
			}

			if (node.hasChildNodes())
			{
				attributeList << parseDocNode(node);
			}
		}
	}

	return attributeList;
}
