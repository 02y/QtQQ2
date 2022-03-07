#include "RootContactItem.h"
#include <QPainter>

RootContactItem::RootContactItem(bool hasArrow, QWidget *parent)
	: QLabel(parent), m_hasArrow(hasArrow), m_rotation(0)
{
	setFixedHeight(32);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	m_animation = new QPropertyAnimation(this, "rotation");
	m_animation->setDuration(30);
	m_animation->setEasingCurve(QEasingCurve::InQuad);
}

RootContactItem::~RootContactItem()
{
}

void RootContactItem::setText(const QString & title)
{
	m_titleText = title;
	update();
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand)	
		m_animation->setEndValue(90);
	else 
		m_animation->setEndValue(0);
	
	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();
}

void RootContactItem::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::TextAntialiasing, true);

	QFont font;
	font.setPointSize(10);
	painter.setFont(font);
	painter.drawText(24, 0, width() - 21, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.save();

	if (m_hasArrow)
	{
		QPixmap arrow(":/Resources/MainWindow/arrow.png");
		QPixmap tmp(arrow.size());
		tmp.fill(Qt::transparent);

		QPainter p(&tmp);
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);
		p.translate(arrow.width() / 2, arrow.height() / 2);
		p.rotate(m_rotation);
		p.drawPixmap(0 - arrow.width() / 2, 0 - arrow.height() / 2, arrow);
		p.end();

		painter.drawPixmap(6, (height() - arrow.height()) / 2, tmp);
	}
	painter.restore();

	return QLabel::paintEvent(event);
}
