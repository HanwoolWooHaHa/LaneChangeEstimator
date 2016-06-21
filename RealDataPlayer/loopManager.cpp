#include "loopManager.h"
#include "closeView.h"
#include "birdView.h"
#include "database.h"
#include "estimator.h"
#include "extractor.h"

#include <QThread>
#include <QLabel>
#include <QGridLayout>
/*********************************************************************/
CLoopManager::CLoopManager( QObject *parent ) : DELTA_T(80)
{
    pWindow = new CWindow;
	m_pEstimator = CEstimator::GetInstance(CEstimator::SVM);
	m_pExtractor = CExtractor::GetInstance();
	
	pWindow->Initialize();
	m_pEstimator->Initialize();
	m_pExtractor->Initialize();

	ResetTime();
}

CLoopManager::~CLoopManager()
{
	if (pWindow != NULL)
		delete pWindow;

	if (m_pEstimator != NULL)
		delete m_pEstimator;

	if (m_pExtractor != NULL)
		delete m_pExtractor;
}

void CLoopManager::ShowWindow( void )
{
	pWindow->show();
}
/*********************************************************************/
/* Public slot functions */
void CLoopManager::DoWork()
{
	int nDataLength = CDatabase::GetInstance()->GetDataLength();

    while( 1 )
    {
		if( !m_bLoopFlag )
			continue;

		/* Extract the features using measurements */
		m_pExtractor->Extract(m_nTick);

		/* Estimate the lane changing using the proposed method */
		m_pEstimator->Estimate( m_nTick );

		//! Viewer‚ÉŒ»Ý‚Ìó‹µ‚ð•`‚­
		pWindow->Update( m_nTick );
		m_nTick++;

		if( m_nTick > nDataLength )
		{
			ResetTime();
			CDatabase::GetInstance()->SaveRecordFile();
			CDatabase::GetInstance()->SaveData(CDatabase::FEATURE);
			CDatabase::GetInstance()->SaveData(CDatabase::ESTIMATION_RESULT);
		}

        QThread::msleep( DELTA_T );
    }
}

void CLoopManager::TimeoutHandler()
{}
/*********************************************************************/
/* Class CWindow */
/*********************************************************************/
CWindow::CWindow()
{
    setWindowTitle(tr("Simulation viewer"));

    m_pCloseView = new CCloseView();
	m_pBirdView = new CBirdView();

	QLabel *closeViewLabel = new QLabel(tr("DRIVER VIEW"));
    closeViewLabel->setAlignment(Qt::AlignHCenter);
    QLabel *birdViewLabel = new QLabel(tr("TOP VIEW"));
    birdViewLabel->setAlignment(Qt::AlignHCenter);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_pCloseView,0,0);
    layout->addWidget(closeViewLabel,1,0);
	
	layout->addWidget(m_pBirdView,0,1);
    layout->addWidget(birdViewLabel,1,1);

    setLayout(layout);
}

void CWindow::Initialize( void )
{
	m_pCloseView->Initialize();
	m_pBirdView->Initialize();
}

void CWindow::Update( int tick )
{
    m_pBirdView->Update( tick );
	m_pCloseView->Update( tick );
}