#include "pathplanner.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PathPlanner w;
	w.show();
	return a.exec();
}
