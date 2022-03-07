#include "MsgTextEdit.h"
#include <qmovie.h>

MsgTextEdit::MsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{
}

MsgTextEdit::~MsgTextEdit()
{
	deleteAllEmotionImage();
}

void MsgTextEdit::addEmotionUrl(int emotionNum)
{
	const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);
	insertHtml(QString("<img src='%1' />").arg(imageName));

	if (m_listEmotionUrl.contains(imageName)) return;
	else m_listEmotionUrl.append(imageName);

	
	QMovie* movie = new QMovie(imageName, "apng", this);
	m_emotionMap.insert(movie, imageName);

	connect(movie, SIGNAL(frameChanged(int)), this, SLOT(onEmotionImageFrameChange(int)));
	movie->start();
	updateGeometry();
}

void MsgTextEdit::deleteAllEmotionImage()
{
	for (auto itor = m_emotionMap.constBegin(); itor != m_emotionMap.constEnd(); itor++)
	{
		delete itor.key();
	}
	m_emotionMap.clear();
}

void MsgTextEdit::onEmotionImageFrameChange(int frame)
{
	QMovie* movie = qobject_cast<QMovie*>(sender());
	document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
}
