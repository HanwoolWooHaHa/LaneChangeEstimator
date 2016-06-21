#include "lanechangeestimator.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LaneChangeEstimator w;
	w.show();
	return a.exec();
}
