#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QSettings>
#include <QApplication>

CommonUtils::CommonUtils()
{
}

QPixmap CommonUtils::getRoundImage(const QPixmap & src, QPixmap & mask, QSize masksize)
{
	if (QSize(0, 0) == masksize)
		masksize = mask.size();
	else
		mask = mask.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	QImage img(masksize, QImage::Format_ARGB32_Premultiplied);
	QPainter p(&img);

	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.fillRect(img.rect(), Qt::transparent);
	p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	p.drawPixmap(0, 0, mask);
	p.setCompositionMode(QPainter::CompositionMode_SourceIn);
	p.drawPixmap(0, 0, src.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	p.end();

	return QPixmap::fromImage(img);
}

void CommonUtils::loadStyleSheet(QWidget * widget, const QString & sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	if (file.open(QIODevice::ReadOnly))
	{
		widget->setStyleSheet("");
		QString styleSheet = QLatin1String(file.readAll());
		widget->setStyleSheet(styleSheet);
	}
	file.close();
}

void CommonUtils::setDefaultSkinColor(const QColor & color)
{
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	QSettings settings(path, QSettings::IniFormat);
	settings.setValue("DefaultSkin/red", color.red());
	settings.setValue("DefaultSkin/green", color.green());
	settings.setValue("DefaultSkin/blue", color.blue());
}

QColor CommonUtils::getDefaultSkinColor()
{
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	if (!QFile::exists(path))
	{
		setDefaultSkinColor(QColor(22, 154, 218));
	}

	QSettings settings(path, QSettings::IniFormat);
	QColor color;
	color.setRed(settings.value("DefaultSkin/red").toInt());
	color.setGreen(settings.value("DefaultSkin/green").toInt());
	color.setBlue(settings.value("DefaultSkin/blue").toInt());

	return color;
}