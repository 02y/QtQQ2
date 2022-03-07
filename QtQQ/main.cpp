#include "UserLogin.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
	a.setQuitOnLastWindowClosed(false);

	UserLogin* ul = new UserLogin;
	ul->show();

    return a.exec();
}
