#include "TalkWindowItem.h"
#include "CommonUtils.h"

TalkWindowItem::TalkWindowItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initControl();
}

TalkWindowItem::~TalkWindowItem()
{
}

void TalkWindowItem::setHeadPixmap(const QPixmap & pixmap)
{
	QPixmap head_mask(":/Resources/MainWindow/head_mask.png");
	const QPixmap& headPixmap = CommonUtils::getRoundImage(pixmap, head_mask, ui.headlabel->size());
	ui.headlabel->setPixmap(headPixmap);
}

void TalkWindowItem::setMsgLabelContent(const QString & msg)
{
	ui.msgLabel->setText(msg);
}

QString TalkWindowItem::getMsgLabelText()
{
	return ui.msgLabel->text();
}

void TalkWindowItem::initControl()
{
	ui.tclosebtn->setVisible(false);
	connect(ui.tclosebtn, &QPushButton::clicked, this, &TalkWindowItem::signalCloseClicked);
}

void TalkWindowItem::enterEvent(QEvent * event)
{
	ui.tclosebtn->setVisible(true);
	__super::enterEvent(event);
}

void TalkWindowItem::leaveEvent(QEvent * event)
{
	ui.tclosebtn->setVisible(false);
	__super::leaveEvent(event);
}

void TalkWindowItem::resizeEvent(QResizeEvent * event)
{
	__super::resizeEvent(event);
}
