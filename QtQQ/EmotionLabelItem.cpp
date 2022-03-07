#include "EmotionLabelItem.h"

EmotionLabelItem::EmotionLabelItem(QWidget *parent)
	: ClickLabel(parent)
{
	initControl();
	connect(this, &ClickLabel::clicked, [this]()
		{
			emit emotionClicked(m_emotionName);
		});
}

EmotionLabelItem::~EmotionLabelItem()
{
}

void EmotionLabelItem::setEmotionName(int emotion)
{
	m_emotionName = emotion;
	QString&& imagePath = QString(":/Resources/MainWindow/emotion/%1.png").arg(emotion);

	m_apngMovie = new QMovie(imagePath, "apng", this);
	setMovie(m_apngMovie);
	m_apngMovie->start();
	m_apngMovie->stop();
}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);
	setObjectName("emotionLabelItem");
	setFixedSize(32, 32);
}
