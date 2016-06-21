#pragma once

#include <QWidget>

class CViewer;
class CNavigator;

/**
* @class	CWindow
* @author	Hanwool Woo
* @version	0.1
* @date	Creation date: 2016/06/17 \n
* 			    Last revision date: 2016/06/17 HanwoolWoo
* @brief
*/
class CWindow : public QWidget
{
	Q_OBJECT

public:
	CWindow( int nWindowWidth, int nWindowHeight );

	void Initialize(void);
	void Update(int tick);

private:
	CViewer* m_pViewer;
};

/**
* @class	CLoopManager
* @author	Hanwool Woo
* @version	0.1
* @date	Creation date: 2016/06/17 \n
* 			    Last revision date: 2016/06/17 HanwoolWoo
* @brief	
*/
class CLoopManager : public QObject
{
	Q_OBJECT

public:
    explicit CLoopManager( int nWindowWidth, int nWindowHeight, QObject* parent = 0 );
	~CLoopManager();

	void ResetTime( void ) { m_nTick = 0; m_bLoopFlag = false; }

	void ShowWindow( void );

	void SetLoopFlag( bool flag ) { m_bLoopFlag = flag; }
	bool GetLoopFlag( void ) { return m_bLoopFlag; }

	CWindow* pWindow;

private:
	CNavigator* m_pNavigator;

public slots:
    void DoWork();
    void TimeoutHandler();

private:
	

	const int DELTA_T;
	int m_nTick;
	bool m_bLoopFlag;
};