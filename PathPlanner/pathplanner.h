#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include <QtWidgets/QMainWindow>
#include "ui_pathplanner.h"

class CThread;
class CLoopManager;

class PathPlanner : public QMainWindow
{
	Q_OBJECT

public:
	PathPlanner(QWidget *parent = 0);
	~PathPlanner();

private:
	Ui::PathPlannerClass ui;

	CThread* m_pThread;
	CLoopManager* m_pLoopManager;

private slots:
	void StartButtonEntered();
	void Generate();
};

#endif // PATHPLANNER_H
