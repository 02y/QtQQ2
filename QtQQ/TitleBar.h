#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>


enum ButtonType
{
	MIN_BUTTON = 0,     //��С�����رհ�ť
	MIN_MAX_BUTTON,     //��С������󻯼��رհ�ť
	ONLY_CLOSE_BUTTON   //ֻ�йرհ�ť
};

class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget *parent = nullptr);
	~TitleBar();

	void setTitleIcon(const QString& filePath);    //���ñ�����ͼ��
	void setTitleContent(const QString& titleContent);     //���ñ���������
	void setTitleWidth(int width);                   //���ñ���������
	void setButtonType(ButtonType buttonType);       //���ñ�������ť����

	void saveRestoreInfo(const QPoint& point, const QSize& size);       //���洰�����ǰ���ڵ�λ�ü���С
	void getRestoreInfo(QPoint& point, QSize& size);                    //��ȡ�������ǰ���ڵ�λ�ü���С

private:
	//��д�¼�
	void paintEvent(QPaintEvent* event);                  //��ͼ�¼�
	void mouseDoubleClickEvent(QMouseEvent* event);       //���˫���¼�
	void mousePressEvent(QMouseEvent* event);             //��갴���¼�
	void mouseMoveEvent(QMouseEvent* event);              //����ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event);           //����ͷ��¼�

	void initControl();          //��ʼ���ؼ�
	void initConnections();      //��ʼ���ź���۵�����
	void loadStyleSheet(const QString& sheetName);        //������ʽ��

signals:
	//��ť������ź�
	void signalButtonMinClicked();          //��С����ť
	void signalButtonRestoreClicked();      //��󻯻�ԭ��ť
	void signalButtonMaxClicked();          //��󻯰�ť
	void signalButtonCloseClicked();        //�رհ�ť

private slots:
	//��ť��Ӧ�Ĳ�
	void onButtonMinClicked();          //��С����ť��Ӧ�Ĳ�
	void onButtonRestoreClicked();      //��󻯻�ԭ��ť��Ӧ�Ĳ�
	void onButtonMaxClicked();          //��󻻰�ť��Ӧ�Ĳ�
	void onButtonCloseClicked();        //�رհ�ť��Ӧ�Ĳ�

private:
	QLabel* m_pIcon;                    //������ͼ��
	QLabel* m_pTitleContent;            //չʾ����������
	QString m_titleContent;             //����������
	ButtonType m_buttonType;            //���������Ͻǰ�ť����

	QPushButton* m_pButtonMin;          //��С����ť
	QPushButton* m_pButtonRestore;      //��󻯻�ԭ��ť
	QPushButton* m_pButtonMax;          //��󻯰�ť
	QPushButton* m_pButtonClose;        //�رհ�ť

	//��󻯻�ԭ��ť���������ڱ��洰��λ�ü���С��
	QPoint m_restorePos;            //����λ��
	QSize m_restoreSize;            //�����С

	//�ƶ����ڱ���
	bool m_isPressed;               //�Ƿ񱻵��
	QPoint m_startMovePos;           //��ʼ�ƶ��ĵ�
};
