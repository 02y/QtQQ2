#include "TalkWindowShell.h"
#include "EmotionWindow.h"
#include "CommonUtils.h"
#include <qsqlquery.h>
#include <qsqlquerymodel.h>
#include <qfile.h>
#include <qmessagebox.h>
#include "TalkWindowItem.h"
#include "TalkWindow.h"
#include "ReceiveFile.h"
#include "WindowManager.h"

const int gtcpPort = 8888;
const int gudpPort = 6666;

extern QString gLoginEmployeeID;
QString gfileName;    //文件名称
QString gfileData;    //文件内容

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();
	initUdpSocket();

	QStringList employeeList;
	getEmployeesIDList(employeeList);
	if (!createJsFile(employeeList))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("更新js文件数据失败"));
	}
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = Q_NULLPTR;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("聊天窗口1"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();

	QList<int> splitterSizes;
	splitterSizes << 154 << width() - 154;
	ui.splitter->setSizes(splitterSizes);
	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(m_emotionWindow, &EmotionWindow::signalEmotionItemClicked, this, &TalkWindowShell::onEmotionItemClicked);
	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceiver = new QUdpSocket(this);

	for (qint16 port = gudpPort; port < gudpPort + 200; ++port)
	{
		if (m_udpReceiver->bind(port, QUdpSocket::ShareAddress)) break;
	}
	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void TalkWindowShell::getEmployeesIDList(QStringList & employeesIDList)
{
	QSqlQueryModel queryEmpId;
	queryEmpId.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1"));
	
	int rows = queryEmpId.rowCount();
	QModelIndex empIdIndex;

	for(int i = 0; i < rows; i++)
	{
		empIdIndex = queryEmpId.index(i, 0);
		employeesIDList << queryEmpId.data(empIdIndex).toString();
	}
}

bool TalkWindowShell::createJsFile(QStringList & employeesList)
{
	QFile temFile(":/Resources/MainWindow/MsgHtml/msgtmpl.txt");
	QString strTemFile;
	if (temFile.open(QIODevice::ReadOnly))
	{
		strTemFile = QLatin1String(temFile.readAll());
		temFile.close();
	}
	else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取 msgtmpl.txt 失败！"));
		return false;
	}

	QFile endFile("./Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (endFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QString strVarExternalNull = "var external = null;";
		QString strExternalObj = "external = channel.objects.external;";
		QString strRecvHtml;

		QFile recvFile(":/Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (recvFile.open(QIODevice::ReadOnly))
		{
			strRecvHtml = QLatin1String(recvFile.readAll());
			recvFile.close();
		}
		else
		{
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取 recvHtml.txt 失败！"));
			return false;
		}

		QString strReplaceVar;
		QString strReplaceObj;
		QString strReplaceRecv;

		for (int i = 0; i < employeesList.count(); i++)
		{
			QString strInitVar = strVarExternalNull;
			strInitVar.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceVar += strInitVar;
			strReplaceVar += "\n";

			QString strInitObj = strExternalObj;
			strInitObj.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceObj += strInitObj;
			strReplaceObj += "\n";

			QString strInitRecv = strRecvHtml;
			strInitRecv.replace("recvHtml", QString("recvHtml_%1").arg(employeesList.at(i)));
			strInitRecv.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceRecv += strInitRecv;
			strReplaceRecv += "\n";
		}
		strTemFile.replace(strVarExternalNull, strReplaceVar);
		strTemFile.replace(strExternalObj, strReplaceObj);
		strTemFile.replace(strRecvHtml, strReplaceRecv);

		/*QTextStream stream(&endFile);
		stream << strTemFile;*/

		endFile.write(strTemFile.toUtf8());
		endFile.close();

		return true;
	}
	else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("写 msgtmp1.js 失败！"));
		return false;
	}
}

void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	MsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);

	if (1 == msgType)
	{
		msgTextEdit.document()->toHtml();
	}
	else if (0 == msgType)
	{
		const int emotionWidth = 3;
		int emotionNum = strMsg.length() / 3;
		for (int i = 0; i < emotionNum; ++i)
		{
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}

	QString&& html = msgTextEdit.document()->toHtml();

	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = QLatin1String(file.readAll());
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else
		{
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件 msgFont.txt 不存在！"));
			return;
		}

		if (!html.contains(fontHtml)) html.replace(strMsg, fontHtml);
	}

	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(senderEmployeeID));
}

void TalkWindowShell::addTalkWindow(TalkWindow * talkWindow, TalkWindowItem * talkWindowItem, const QString uid)
{
	ui.rightStackedWidget->addWidget(talkWindow);

	QListWidgetItem* item = new QListWidgetItem(ui.listWidget);
	item->setSelected(true);
	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, talkWindowItem);
	m_talkwindowItemMap.insert(item, talkWindow);
	setCurrentWidget(talkWindow);

	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [this, talkWindow, talkWindowItem, item]()
		{
			m_talkwindowItemMap.remove(item);
			ui.listWidget->takeItem(ui.listWidget->row(item));
			ui.rightStackedWidget->removeWidget(talkWindow);
			delete talkWindowItem;
			delete item;
			talkWindow->close();
			if (ui.rightStackedWidget->count() < 1)
			{
				//this->setVisible(false);
				this->close();
			}
		});
}

void TalkWindowShell::setCurrentWidget(QWidget * widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkwindowItemMap;
}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	m_emotionWindow->setVisible(true);

	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));
	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);
}

//文本数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）+ 信息类型 + 数据长度 + 数据（hello）
//表情数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）+ 信息类型 + 表情个数 + images + 数据
//msgType 0-表情信息 1-文本信息 2-文件信息
void TalkWindowShell::updateSendTcpMsg(QString & strData, int & msgType, QString fileName)
{
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkID = talkWindow->getTalkId();
	QString strdataLength;
	QString strGroupFlag;
	QString strSend;
	
	if (4 == talkID.length()) strGroupFlag = "1";
	else strGroupFlag = "0";

	if (1 == msgType)
	{
		int nstrDataLength = strData.length();
		int dataLength = QString::number(nstrDataLength).length();

		if (1 == dataLength) strdataLength = "0000" + QString::number(nstrDataLength);
		else if (2 == dataLength)
			strdataLength = "000" + QString::number(nstrDataLength);
		else if (3 == dataLength)
			strdataLength = "00" + QString::number(nstrDataLength);
		else if (4 == dataLength)
			strdataLength = "0" + QString::number(nstrDataLength);
		else if (5 == dataLength)
			strdataLength = QString::number(nstrDataLength);
		else
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不合理的数据长度！"));

		strSend = strGroupFlag + gLoginEmployeeID + talkID + "1" + strdataLength + strData;
	}
	else if (0 == msgType)
	{
		strSend = strGroupFlag + gLoginEmployeeID + talkID + "0" + strData;
	}
	else if (2 == msgType)
	{
		QString strLength = QString::number(strData.length());
		strSend = strGroupFlag + gLoginEmployeeID + talkID + "2" + strLength + "bytes" + fileName + "data_begin" + strData;
	}

	QByteArray bt;
	bt.resize(strSend.length());
	bt = strSend.toUtf8();
	m_tcpClientSocket->write(bt);
}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem * item)
{
	QWidget* widget = getTalkWindowItemMap().value(item);
	setCurrentWidget(widget);
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if(talkWindow) talkWindow->addEmotionImage(emotionNum);
}

/*                   *******规则太浪费字节数，需优化    解析规则需要优化（不能信息+表情一起发）*******
	数据包的格式
	文本数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）+ 信息类型(1) + 数据长度 + 数据（hello）
	表情数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）+ 信息类型(0) + 表情个数 + images + 表情名称
	文件数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）+ 信息类型(2) + 文件长度 + "bytes" + 文件名称 + \
				 "data_begin" + 文件内容

	群聊标志占1位，0表示单聊，1表示群聊
	信息类型占1位，0表示表情信息，1表示文本信息，2表示文件信息

	QQ号占5位，群QQ号占4位，数据长度占5位（5000个字节长度是4位），表情名称占3位
	   （注意：当群聊标志为1时，则数据包没有收信息员工QQ号，而是收信息群QQ号
	   　　　　当群聊标志为０时，则数据包没有收信息群QQ号，而是收信息员工QQ号）

	群聊文本信息如：1100012001100005hello   表示QQ-10001向群-2001发送文本信息，数据长度为00005，内容为hello
	单聊图片信息如：0100091000201images060
	群聊文件信息如：1100052000210bytestest.textdata_beginhelloworld

	群聊文件信息解析：1 10001 2001 1 00005 hello
	单聊图片信息解析：0 10009 10002 0 1 images 60
*/
void TalkWindowShell::processPendingData()
{
	while (m_udpReceiver->hasPendingDatagrams())
	{
		const static int groupFlagWidth = 1;     //群聊标志占位
		const static int groupWidth = 4;         //群QQ号宽度
		const static int employeeWidth = 5;      //员工QQ号宽度
		const static int msgTypeWidth = 1;       //信息类型宽度
		const static int msgLengthWidth = 5;     //文本信息长度的宽度
		const static int pictureWidth = 3;       //表情图片的宽度

		QByteArray btData;
		btData.resize(m_udpReceiver->pendingDatagramSize());
		m_udpReceiver->readDatagram(btData.data(), btData.size());

		QString strData(btData);
		QString strWindowID;
		QString strSendEmployeeID, strReceiveEmployeeID;
		QString strMsg;
		int msgLen;
		int msgType;

		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);
		if (strSendEmployeeID == gLoginEmployeeID) return;

		if (btData[0] == '1')
		{
			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];

			if (cMsgType == '1')
			{
				msgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.right(msgLen);
			}
			else if(cMsgType == '0')
			{
				msgType = 0;
				int posImages = strData.indexOf("images");
				strMsg = strData.right(strData.length() - posImages - QString("images").length());
			}
			else if (cMsgType == '2')
			{
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - bytesWidth - posBytes);
				gfileName = fileName;

				strMsg = strData.mid(posData_begin + data_beginWidth);
				gfileData = strMsg;

				QString Sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
				querySenderName.exec();
				if (querySenderName.next()) Sender = querySenderName.value(0).toString();

				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() { return; });
				recvFile->setMsg(Sender);
				recvFile->show();
			}
		}
		else
		{
			strReceiveEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);
			strWindowID = strSendEmployeeID;
			if (strReceiveEmployeeID != gLoginEmployeeID) return;

			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if (cMsgType == '1')
			{
				msgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.right(msgLen);
			}
			else if (cMsgType == '0')
			{
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages + imagesWidth);
			}
			else if (cMsgType == '2')
			{
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - bytesWidth - posBytes);
				gfileName = fileName;

				strMsg = strData.mid(posData_begin + data_beginWidth);
				gfileData = strMsg;

				QString Sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySendName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
				querySendName.exec();
				if (querySendName.next()) Sender = querySendName.value(0).toString();

				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() { return; });
				recvFile->setMsg(Sender);
				recvFile->show();
			}
		}

		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget)
		{
			this->setCurrentWidget(widget);
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);
			item->setSelected(true);
		}
		else return;

		if (msgType != 2) handleReceivedMsg(strSendEmployeeID.toInt(), msgType, strMsg);
	}
}