#include "TitleBar.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>

#define BUTTON_HEIGHT 27     //按钮高度
#define BUTTON_WIDTH 27      //按钮宽度
#define TITLE_HEIGHT 27      //标题栏高度


TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent), m_isPressed(false), m_buttonType(MIN_MAX_BUTTON)
{
	initControl();
	initConnections();
	loadStyleSheet("Title");
}

TitleBar::~TitleBar()
{
}

void TitleBar::setTitleIcon(const QString & filePath)
{
	QPixmap pix(filePath);
	m_pIcon->setFixedSize(pix.size());
	m_pIcon->setPixmap(pix);
}

void TitleBar::setTitleContent(const QString & titleContent)
{
	m_pTitleContent->setText(titleContent);
	m_titleContent = titleContent;
}

void TitleBar::setTitleWidth(int width)
{
	setFixedWidth(width);
}

void TitleBar::setButtonType(ButtonType buttonType)
{
	m_buttonType = buttonType;

	switch (buttonType)
	{
	case MIN_BUTTON:
	{
		m_pButtonMax->setVisible(false);
		m_pButtonRestore->setVisible(false);
		break;
	}
	
	case MIN_MAX_BUTTON:
	{
		m_pButtonRestore->setVisible(false);
		break;
	}

	case ONLY_CLOSE_BUTTON:
	{
		m_pButtonMax->setVisible(false);
		m_pButtonMin->setVisible(false);
		m_pButtonRestore->setVisible(false);
		break;
	}

	default:
		break;
	}
}

void TitleBar::saveRestoreInfo(const QPoint & point, const QSize & size)
{
	m_restorePos = point;
	m_restoreSize = size;
}

void TitleBar::getRestoreInfo(QPoint & point, QSize & size)
{
	point = m_restorePos;
	size = m_restoreSize;
}

void TitleBar::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.drawPath(path);

	if (width() != parentWidget()->width())
		setTitleWidth(parentWidget()->width());

	QWidget::paintEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (MIN_MAX_BUTTON == m_buttonType)
	{
		if (m_pButtonMax->isVisible())
			onButtonMaxClicked();
		else
			onButtonRestoreClicked();
	}

	return QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::mousePressEvent(QMouseEvent * event)
{
	if (MIN_MAX_BUTTON == m_buttonType)
	{
		if (m_pButtonMax->isVisible())
		{
			m_isPressed = true;
			m_startMovePos = event->globalPos();
		}
	}
	else
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent * event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = parentWidget()->pos();

		parentWidget()->move(widgetPos + movePoint);
	}

	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent * event)
{
	m_isPressed = false;

	return QWidget::mouseReleaseEvent(event);
}

void TitleBar::initControl()
{
	m_pIcon = new QLabel(this);
	m_pTitleContent = new QLabel(this);
	m_pButtonMin = new QPushButton(this);
	m_pButtonRestore = new QPushButton(this);
	m_pButtonMax = new QPushButton(this);
	m_pButtonClose = new QPushButton(this);

	// 对按钮,设置固定大小
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));				// 最小化
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));			// 最大化还原按钮
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));				// 最大化
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));			// 关闭按钮

	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	QHBoxLayout* mylayout = new QHBoxLayout(this);
	mylayout->addWidget(m_pIcon);										// 添加各种按钮
	mylayout->addWidget(m_pTitleContent);
	mylayout->addWidget(m_pButtonMin);
	mylayout->addWidget(m_pButtonRestore);
	mylayout->addWidget(m_pButtonMax);
	mylayout->addWidget(m_pButtonClose);

	mylayout->setContentsMargins(5, 0, 0, 0);						// 设置布局, 间隙
	mylayout->setSpacing(0);									    // 设置布局里部件之间, 间隙

	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(TITLE_HEIGHT);
	setWindowFlags(Qt::FramelessWindowHint);
}

void TitleBar::initConnections()
{
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

void TitleBar::loadStyleSheet(const QString & sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	if (file.open(QIODevice::ReadOnly))
	{
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		setStyleSheet(styleSheet);
	}
	file.close();
}

void TitleBar::onButtonMinClicked()
{
	emit signalButtonMinClicked();
}

void TitleBar::onButtonRestoreClicked()
{
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

void TitleBar::onButtonMaxClicked()
{
	m_pButtonMax->setVisible(false);
	m_pButtonRestore->setVisible(true);
	emit signalButtonMaxClicked();
}

void TitleBar::onButtonCloseClicked()
{
	emit signalButtonCloseClicked();
}
