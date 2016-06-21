#ifndef LANECHANGEESTIMATOR_H
#define LANECHANGEESTIMATOR_H

#include <QtWidgets/QMainWindow>
#include "ui_lanechangeestimator.h"

class CEstimator;

class LaneChangeEstimator : public QMainWindow
{
	Q_OBJECT

public:
	LaneChangeEstimator(QWidget *parent = 0);
	~LaneChangeEstimator();

private:
	Ui::LaneChangeEstimatorClass ui;
	CEstimator* m_pEstimator;

	int m_nMode;
	int m_nType;
	int m_nMethod;

private slots:
	void handleModeChanged(int index);
	void handleModeSelected(void);

	void handleTypeChanged(int index);
	void handleTypeSelected(void);

	void handleMethodChanged(int index);

	void openMenuClicked(void);
};

#endif // LANECHANGEESTIMATOR_H
