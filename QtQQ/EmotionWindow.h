#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget *parent = Q_NULLPTR);
	~EmotionWindow();

private:
	void initControl();

private slots:
	void addEmotion(int emotionNum);

signals:
	void signalEmotionWindowHide();                   //���鴰������
	void signalEmotionItemClicked(int emotionNum);    //���鱻���

private:
	void paintEvent(QPaintEvent* event) override;        //��д��ͼ�¼�

private:
	Ui::EmotionWindow ui;
};
