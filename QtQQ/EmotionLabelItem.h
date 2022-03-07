#pragma once

#include "ClickLabel.h"
#include <QMovie>

class EmotionLabelItem : public ClickLabel
{
	Q_OBJECT

public:
	EmotionLabelItem(QWidget *parent);
	~EmotionLabelItem();
	void setEmotionName(int emotion);

private:
	void initControl();

signals:
	void emotionClicked(int emotionNum);

private:
	int m_emotionName;
	QMovie* m_apngMovie;
};
