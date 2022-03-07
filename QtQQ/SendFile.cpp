#include "SendFile.h"
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <qmessagebox.h>
#include <qfiledialog.h>

SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("SendFile");
	move(100, 400);

	TalkWindowShell* talkwindowshell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &SendFile::sendFileClicked, talkwindowshell, &TalkWindowShell::updateSendTcpMsg);
}

SendFile::~SendFile()
{
}

void SendFile::on_openBtn_clicked()
{
	m_filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"), ".",
		QString::fromLocal8Bit("发送的文件(*.txt *.doc);;所有文件(*.*)"));
	ui.lineEdit->setText(m_filePath);
}

void SendFile::on_sendBtn_clicked()
{
	if (!m_filePath.isEmpty())
	{
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly))
		{
			int msgType = 2;
			QString str = QLatin1String(file.readAll());
			QFileInfo info(file);
			QString fileName = info.fileName();

			emit sendFileClicked(str, msgType, fileName);
			file.close();
		}
		else
		{
			QMessageBox::information(this, QStringLiteral("提示"), QString::fromLocal8Bit("发送文件：%1 失败").arg(m_filePath));
			ui.lineEdit->clear();
			this->close();
			return;
		}

		m_filePath = "";
		this->close();
	}
}
