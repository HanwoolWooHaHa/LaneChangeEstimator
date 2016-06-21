#include "loopManager.h"
#include "viewer.h"
#include "navigator.h"

#include <QThread>
#include <QLabel>
#include <QGridLayout>
/*********************************************************************/
CLoopManager::CLoopManager(int nWindowWidth, int nWindowHeight, QObject *parent) : DELTA_T(100) // time step 100ms
{
	pWindow = new CWindow( nWindowWidth, nWindowHeight );
	pWindow->Initialize();

	m_pNavigator = new CNavigator((double)DELTA_T/1000);
	m_pNavigator->Initialize();

	ResetTime();
}

CLoopManager::~CLoopManager()
{
	if (pWindow != NULL)
		delete pWindow;

	if (m_pNavigator != NULL)
		delete m_pNavigator;
}

void CLoopManager::ShowWindow( void )
{
	pWindow->show();
}
/*********************************************************************/
/* Public slot functions */
void CLoopManager::DoWork()
{
	while( 1 )
    {
		if( !m_bLoopFlag )
			continue;

		if ( m_nTick != 0 )
			m_pNavigator->Update(m_nTick);

		pWindow->Update(m_nTick);
		m_nTick++;
		

        QThread::msleep( DELTA_T );
    }
}

void CLoopManager::TimeoutHandler()
{}
/*********************************************************************/
/* Class CWindow */
/*********************************************************************/
CWindow::CWindow( int nWindowWidth, int nWindowHeight )
{
	setWindowTitle(tr("Simulation viewer"));

	m_pViewer = new CViewer(nWindowWidth,nWindowHeight);

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(m_pViewer, 0, 0);

	setLayout(layout);
}

void CWindow::Initialize(void)
{
	m_pViewer->Initialize();
}

void CWindow::Update(int nTick)
{
	m_pViewer->Update(nTick);
}