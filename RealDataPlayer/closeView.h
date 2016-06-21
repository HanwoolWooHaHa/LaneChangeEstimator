#pragma once

#include <../QtOpenGL/QGLWIDGET>

class CDatabase;

class CCloseView : public QGLWidget
{
	Q_OBJECT

public:
	explicit CCloseView( QWidget* parent = 0 );
	~CCloseView() {}

	void Initialize( void );
	void Update( int tick );

private:
	enum{ OWN=0, CHANGING, KEEPING, ALL };

	void drawVehicle( int type, int index );
	void drawLine( void );
	void drawHighway(double dGlobalOwnX, double dGlobalOwnY, double dAttitude);
	void renderVehicle(double dPosX, double dPosY, double dAttitude, const QColor& color);
	void cylinder(double dPosX, double dPosY, double dPosZ, float radius, float width, int sides);
	
	CDatabase* m_pDatabase;

	int m_nTick;
	double m_dOwnVehicleCube[8][3];
	float m_fZoom;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
	void wheelEvent(QWheelEvent *event);
};