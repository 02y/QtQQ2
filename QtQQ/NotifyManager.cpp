#include "NotifyManager.h"
#include "CommonUtils.h"

Q_GLOBAL_STATIC(NotifyManager, theInstance);

NotifyManager::NotifyManager()
	:QObject(Q_NULLPTR)
{
}

NotifyManager::~NotifyManager()
{
}

NotifyManager * NotifyManager::getInstance()
{
	return theInstance();
}

void NotifyManager::notifyOtherWindowChangeSkin(const QColor & color)
{
	emit signalSkinChanged(color);

	CommonUtils::setDefaultSkinColor(color);
}
