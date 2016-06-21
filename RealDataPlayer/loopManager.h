#pragma once

#include <QWidget>

class CCloseView;
class CBirdView;
class CEstimator;
class CExtractor;

/**
* @class	CWindow
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2015/12/23 \n
* 			    Last revision date: 2015/12/23 HanwoolWoo
* @brief	
*/
class CWindow : public QWidget
{
    Q_OBJECT

public:
    CWindow();

    void Initialize( void );
    void Update( int tick );

private:
    CCloseView* m_pCloseView;
	CBirdView* m_pBirdView;
};

/**
* @class	CLoopManager
* @author	Hanwool Woo
* @version	1.10
* @date	Creation date: 2015/12/23 \n
* 			    Last revision date: 2015/12/23 HanwoolWoo
* @brief	
*/
class CLoopManager : public QObject
{
	Q_OBJECT

public:
    explicit CLoopManager( QObject* parent = 0 );
	~CLoopManager();

	void ResetTime( void ) { m_nTick = 0; m_bLoopFlag = false; }

	void ShowWindow( void );

	void SetLoopFlag( bool flag ) { m_bLoopFlag = flag; }
	bool GetLoopFlag( void ) { return m_bLoopFlag; }

	CWindow* pWindow;

private:
	CEstimator* m_pEstimator;
	CExtractor* m_pExtractor;

public slots:
    void DoWork();
    void TimeoutHandler();

private:
	

	const int DELTA_T;
	int m_nTick;
	bool m_bLoopFlag;
};