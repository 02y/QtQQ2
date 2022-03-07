#pragma once

#include <QTextEdit>

class MsgTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	MsgTextEdit(QWidget *parent = nullptr);
	~MsgTextEdit();

private slots:
	void onEmotionImageFrameChange(int frame);  //�����ı�ʱ��Ӧ�Ĳۺ�����֡����

public:
	void addEmotionUrl(int emotionNum);        //��ӱ���
	void deleteAllEmotionImage();

private:
	QList<QString> m_listEmotionUrl;
	QMap<QMovie*, QString> m_emotionMap;
};
