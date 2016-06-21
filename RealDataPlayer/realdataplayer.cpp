//! My header files
#include "realdataplayer.h"
#include "database.h"
#include "myThread.h"
#include "loopManager.h"

//! Qt Library header files
#include <qfiledialog.h>
#include <qgridlayout.h>

RealDataPlayer::RealDataPlayer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_pDatabase = CDatabase::GetInstance();
	m_pThread = new CThread();
	m_pLoopManager = new CLoopManager();

	m_pLoopManager->moveToThread(m_pThread);
	connect(ui.btnStart, SIGNAL(clicked()), m_pLoopManager, SLOT(DoWork()));

	m_pThread->start();

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(m_pLoopManager->pWindow, 0, 0);
	ui.widget->setLayout(layout);
}

RealDataPlayer::~RealDataPlayer()
{

}
/*********************************************************************/
/* Private slot functions */
void RealDataPlayer::LoadFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("mazda data file"), "C:/2016project/dataset/mazda/Mazda_data_20160128", tr("Data file (*.csv);;All Files (*)"));

	//! 計測データを読み込む
	m_pDatabase->SetOrigin();
	m_pDatabase->LoadMeasurementData(fileName);
	

	ui.btnStart->setEnabled(true);
	ui.btnReset->setEnabled(true);

	ui.lblFileName->setText(fileName);
}

void RealDataPlayer::StartButtonEntered()
{
	bool bFlag = m_pLoopManager->GetLoopFlag();
	m_pLoopManager->SetLoopFlag(!bFlag);

	if (bFlag)
		ui.btnStart->setText("Start");
	else
		ui.btnStart->setText("Pause");
}

void RealDataPlayer::ResetButtonEntered()
{
	m_pLoopManager->ResetTime();

	m_pLoopManager->SetLoopFlag(false);
	ui.btnStart->setText("Start");
}