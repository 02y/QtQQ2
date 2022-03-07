#include "ContactItem.h"
#include "CommonUtils.h"

ContactItem::ContactItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ContactItem::~ContactItem()
{
}

void ContactItem::setUserName(const QString & userName)
{
	ui.userName->setText(userName);
}

void ContactItem::setSignName(const QString & signName)
{
	ui.signName->setText(signName);
}

void ContactItem::setHeadPixmap(const QPixmap & src)
{
	QPixmap head_mask(":/Resources/MainWindow/head_mask.png");
	ui.label->setPixmap(CommonUtils::getRoundImage(src, head_mask, ui.label->size()));
}

QString ContactItem::getUserName() const
{
	return ui.userName->text();
}

QSize ContactItem::getHeadLabelSize() const
{
	return ui.label->size();
}
