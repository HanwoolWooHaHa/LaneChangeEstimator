#ifndef REALDATAPLAYER_H
#define REALDATAPLAYER_H

#include <QtWidgets/QMainWindow>
#include "ui_realdataplayer.h"

class CDatabase;
class CThread;
class CLoopManager;

class RealDataPlayer : public QMainWindow
{
	Q_OBJECT

public:
	RealDataPlayer(QWidget *parent = 0);
	~RealDataPlayer();

private:
	Ui::RealDataPlayerClass ui;
	CDatabase* m_pDatabase;
	CThread* m_pThread;
	CLoopManager* m_pLoopManager;

	private slots:
	void LoadFile();
	void StartButtonEntered();
	void ResetButtonEntered();
};

#endif // REALDATAPLAYER_H
