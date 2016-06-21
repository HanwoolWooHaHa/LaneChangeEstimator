#include "pathplanner.h"
#include "../Library/Thread/myThread.h"
#include "loopManager.h"
#include "database.h"

#include <QGridLayout>

PathPlanner::PathPlanner(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_pThread = new CThread();
	m_pLoopManager = new CLoopManager( ui.widget->width(), ui.widget->height() );

	m_pLoopManager->moveToThread(m_pThread);
	connect(ui.btnStart, SIGNAL(clicked()), m_pLoopManager, SLOT(DoWork()));

	m_pThread->start();

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(m_pLoopManager->pWindow, 0, 0);
	ui.widget->setLayout(layout);
}

PathPlanner::~PathPlanner()
{

}
/*********************************************************************/
/* Private slot functions */
void PathPlanner::StartButtonEntered()
{
	bool bFlag = m_pLoopManager->GetLoopFlag();
	m_pLoopManager->SetLoopFlag(!bFlag);

	if (bFlag)
		ui.btnStart->setText("Start");
	else
		ui.btnStart->setText("Pause");
}

void PathPlanner::Generate()
{
	CDatabase::GetInstance()->GenerateData();
}