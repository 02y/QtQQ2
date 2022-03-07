#include "ReceiveFile.h"
#include <qfiledialog.h>
#include <qmessagebox.h>

extern QString gfileName;
extern QString gfileData;

ReceiveFile::ReceiveFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	move(100, 400);
	connect(m_titleBar, &TitleBar::signalButtonCloseClicked, this, &ReceiveFile::refuseFile);
}

ReceiveFile::~ReceiveFile()
{
}

void ReceiveFile::setMsg(QString & msgLabel)
{
	const QString& label = QString::fromLocal8Bit("收到来自") + msgLabel + QString::fromLocal8Bit("发来的文件, 是否接收");
	ui.label->setText(label);
}

void ReceiveFile::on_cancelBtn_clicked()
{
	emit refuseFile();
	this->close();
}

void ReceiveFile::on_okBtn_clicked()
{
	this->close();

	QString fileDirPath = QFileDialog::getExistingDirectory(nullptr, QString("文件保存路径"), "/");
	QString filePath = fileDirPath + "/" + gfileName;
	QFile file(filePath);

	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(nullptr, QStringLiteral("提示"), QString::fromLocal8Bit("文件接收失败！"));
	}
	else
	{
		file.write(gfileData.toUtf8());
		file.close();
		QMessageBox::information(nullptr, QStringLiteral("提示"), QString::fromLocal8Bit("文件接收成功！"));
	}
}
