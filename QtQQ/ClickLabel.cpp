#include "ClickLabel.h"
#include <QMouseEvent>

ClickLabel::ClickLabel(QWidget *parent)
	: QLabel(parent)
{
}

ClickLabel::~ClickLabel()
{
}

void ClickLabel::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit clicked();
	}

	return QLabel::mousePressEvent(event);
}
