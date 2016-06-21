#include "realdataplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RealDataPlayer w;
	w.show();
	return a.exec();
}
