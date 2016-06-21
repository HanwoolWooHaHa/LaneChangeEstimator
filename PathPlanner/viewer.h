#pragma once

#include <../QtOpenGL/QGLWIDGET>

class CDatabase;

class CViewer : public QGLWidget
{
	Q_OBJECT

public:
	explicit CViewer( int nWidth, int nHeight, QWidget* parent = 0 );
	~CViewer() {}

	void Initialize( void );
	void Update( int tick );

private:
	enum{ OWN=0, CHANGING, KEEPING, ALL };

	void drawVehicle( int type, int index );
	void drawLine( void );
	void drawLineMarkings(void);
	void drawHighway(double dGlobalOwnX, double dGlobalOwnY, double dAttitude);
	void drawingPredictionArea(double dGlobalOwnX, double dGlobalOwnY, double dAttitude);

	void renderVehicle(double dPosX, double dPosY, double dAttitude, const QColor& color);

	void renderVehicle(int nIndex, double dPosX, double dPosY, double dAttitude, bool bTarget);

	void cylinder(double dPosX, double dPosY, double dPosZ, float radius, float width, int sides);
	
	CDatabase* m_pDatabase;

	int m_nTick;
	float m_fZoom;
	QPoint m_lastPt;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
	void wheelEvent(QWheelEvent *event);
	//void mousePressEvent(QMouseEvent *event);
	//void mouseMoveEvent(QMouseEvent *event);
};