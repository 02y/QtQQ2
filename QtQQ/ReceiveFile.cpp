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
	const QString& label = QString::fromLocal8Bit("�յ�����") + msgLabel + QString::fromLocal8Bit("�������ļ�, �Ƿ����");
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

	QString fileDirPath = QFileDialog::getExistingDirectory(nullptr, QString("�ļ�����·��"), "/");
	QString filePath = fileDirPath + "/" + gfileName;
	QFile file(filePath);

	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(nullptr, QStringLiteral("��ʾ"), QString::fromLocal8Bit("�ļ�����ʧ�ܣ�"));
	}
	else
	{
		file.write(gfileData.toUtf8());
		file.close();
		QMessageBox::information(nullptr, QStringLiteral("��ʾ"), QString::fromLocal8Bit("�ļ����ճɹ���"));
	}
}
