#include "lanechangeestimator.h"
#include "defines.h"
#include "database.h"
#include "estimator.h"

LaneChangeEstimator::LaneChangeEstimator(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.menuBar->setEnabled(false);
	this->resize(500, 100);

	m_pEstimator = NULL;

	m_nMode = DEFAULT;
	m_nType = CHANGING;
	m_nMethod = HMM;
}

LaneChangeEstimator::~LaneChangeEstimator()
{
	if (m_pEstimator != NULL)
		delete m_pEstimator;
}
///////////////////////////////////////////////////////////////////////////
/* Private slot functions */
///////////////////////////////////////////////////////////////////////////
void LaneChangeEstimator::handleModeChanged(int index)
{
	m_nMode = index;
}

void LaneChangeEstimator::handleModeSelected(void)
{
	int nReturn = 0;

	switch (m_nMode)
	{
	case DEFAULT:
		ui.message->setText("Please select the simulator mode");
		return;

	case LINE_EXTRACTOR:
		ui.message->setText("Line extractor mode was selected");
		ui.menuBar->setEnabled(true);
		break;

	case TRAINER:
		nReturn = CDatabase::GetInstance()->LoadData(APPROXIMATE_CURVES);
		if (nReturn == FAIL)
		{
			ui.message->setText("Approx. Curve data not loaded...");
			return;
		}
		ui.message->setText("Trainer mode was selected");
		this->resize(500, 300);
		break;

	case TESTER:
		nReturn = CDatabase::GetInstance()->LoadData(APPROXIMATE_CURVES);
		if (nReturn == FAIL)
		{
			ui.message->setText("Approx. Curve data not loaded...");
			return;
		}

		ui.message->setText("Tester mode was selected");
		this->resize(500, 300);
		break;
	}

	ui.comboBox->setEnabled(false);
	ui.pushButton->setEnabled(false);
}

void LaneChangeEstimator::handleTypeChanged(int index)
{
	switch (index)
	{
	case 0: m_nType = CHANGING; break;
	case 1: m_nType = KEEPING; break;
	}
}

void LaneChangeEstimator::handleTypeSelected(void)
{
	int nReturn = CDatabase::GetInstance()->LoadData(m_nMode, m_nType);
	if (nReturn == FAIL)
	{
		ui.message->setText("Data not loaded...");
		return;
	}

	ui.comboBox2->setEnabled(false);
	ui.comboBox3->setEnabled(false);
	ui.pushButton2->setEnabled(false);

	ui.message->setText("Traffic data successfully loaded...");

	m_pEstimator = new CEstimator(m_nMethod);

	switch (m_nMode)
	{
	case TRAINER:
		m_pEstimator->Train(m_nType);
		break;

	case TESTER:
		m_pEstimator->Test();
		break;
	}
}

void LaneChangeEstimator::handleMethodChanged(int index)
{
	m_nMethod = index;
}

void LaneChangeEstimator::openMenuClicked(void)
{
	int nReturn = 0;

	switch (m_nMode)
	{
	case DEFAULT:
		ui.message->setText("Open menu function has some problems");
		return;

	case LINE_EXTRACTOR:
		nReturn = CDatabase::GetInstance()->LoadData(m_nMode);
		if (nReturn == FAIL)
		{
			ui.message->setText("Data not loaded...");
			return;
		}
		break;

	case TRAINER:
		break;
	}
}