#include "mainwindow.h"
#include <QApplication>

QString osName =
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
	"unix"
#elif defined(Q_OS_WIN32)
	"win"
#else
	#error "Os not supported."
#endif
;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
