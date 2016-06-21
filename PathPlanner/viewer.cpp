#include "viewer.h"
#include "database.h"

#include <gl\glut.h>
#include <QWheelEvent>
#include <qmath.h>

/*********************************************************************/
CViewer::CViewer(int nWidth, int nHeight, QWidget *parent) : QGLWidget(parent)
{
	m_pDatabase = CDatabase::GetInstance();

	this->setFixedSize(nWidth, nHeight);
	m_nTick = 0;
}
/*********************************************************************/
/* Public functions */
void CViewer::Update( int tick )
{
	m_nTick = tick;
	update();
}

void CViewer::Initialize( void )
{
	m_fZoom = 17.0;
	m_lastPt.setX( 0.0);
	m_lastPt.setY( 0.0);
}
/*********************************************************************/
/* Private member functions */
void CViewer::initializeGL()
{
    qglClearColor(QColor(160, 216, 239));
    glEnable( GL_DEPTH_TEST );
}

void CViewer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glLoadIdentity();
    glOrtho(-1 , 1 , -1 , 1 , 2 , 4);
}

void CViewer::wheelEvent(QWheelEvent *event)
{
	float delta = (float)event->delta();

    m_fZoom += (delta/120.0);

    updateGL();
}

#if 0
void CViewer::mousePressEvent(QMouseEvent *event)
{
	m_lastPt = event->pos();
}

void CViewer::mouseMoveEvent(QMouseEvent *event)
{
	float dx = (event->x() - m_lastPt.x()) / 10.0f;
	float dy = (event->y() - m_lastPt.y())/ 10.0f;

	if (event->buttons() && Qt::LeftButton) 
    {
		glRotatef(dy*0.1, 1.0f, 0.0f, 0.0f);
		glRotatef(dx*0.1, 0.0f, 1.0f, 0.0f);
    }
}
#endif

void CViewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
	gluPerspective(30.0, (double)width() / (double)height(), 1.0, 1000.0);


	double dGlobalPosX = m_pDatabase->GetData(CDatabase::SIMULATION, m_nTick, 0, PACKET_SIMUL_POSX);
	double dGlobalPosY = m_pDatabase->GetData(CDatabase::SIMULATION, m_nTick, 0, PACKET_SIMUL_POSY);
	double dAttitude = 0.0; // m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_ATT);

	// 2. set the view point
	if( m_nTick == 0 )
	{
		gluLookAt( 25.0, 15.0, 20.0, 0.0, 0.0, 0.0,  0.0, 0.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glFlush();
		return;
	}
	else
	{
		//! Global座標系における自車の真上から見た視点
		//double dRadian = qDegreesToRadians(dAttitude);
		//gluLookAt( dGlobalPosX, dGlobalPosY, m_fZoom, dGlobalPosX, dGlobalPosY, 0.0, qCos(dRadian), qSin(dRadian), 0.0);
		//gluLookAt(dGlobalPosX, dGlobalPosY, m_fZoom, dGlobalPosX, dGlobalPosY, 0.0, 0.0, 1.0, 0.0);

		gluLookAt(dGlobalPosX - m_fZoom, dGlobalPosY - m_fZoom, m_fZoom, dGlobalPosX, dGlobalPosY, 0.0, -0.4962, -0.4962, 0.7017);
	}

	glMatrixMode(GL_MODELVIEW);
	
	// 3. draw the highway
	drawHighway(dGlobalPosX, dGlobalPosY, dAttitude);

	// 4. draw vehicles
	drawVehicle( OWN, 0 );

	int nNumAdjacentVehicles = m_pDatabase->GetNumAdjacentVehicles();
	for (int n = 1; n < nNumAdjacentVehicles; n++)
	{
		drawVehicle(ALL, n);
	}

    glFlush();
}
/*********************************************************************/
void CViewer::drawHighway(double dGlobalOwnX, double dGlobalOwnY, double dAttitude)
{
	glPushMatrix();
	glLoadIdentity();

	qglColor(Qt::darkGray);

	glBegin(GL_POLYGON);

	//! highway
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 3 * LANE_WIDTH, 0.0);
	glVertex3f(2000.0, 3 * LANE_WIDTH, 0.0);
	glVertex3f(2000.0, 0.0, 0.0);

	glEnd();


	
	//! markings
	for (int i = 0; i < 200; i++)
	{
		glBegin(GL_POLYGON);

		glVertex3f(i * 10.0, 3 * LANE_WIDTH + 2.0, 0.0);
		glVertex3f(i * 10.0, 3 * LANE_WIDTH + 3.0, 0.0);
		glVertex3f(i * 10.0 + 1.0, 3 * LANE_WIDTH + 3.0, 0.0);
		glVertex3f(i * 10.0 + 1.0, 3 * LANE_WIDTH + 2.0, 0.0);

		glEnd();
	}

	

	drawLine();
}

void CViewer::drawLine( void )
{
	// 車線点群をすべて描く
	glLoadIdentity();
	qglColor(Qt::white);

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xFCFC);
	glLineWidth(6.0);

	glBegin(GL_LINES);

	GLdouble line[2][3] = { { 0.0, LANE_WIDTH, 0.0 }, { 2000.0, LANE_WIDTH, 0.0 } };

	glVertex3dv(line[0]);
	glVertex3dv(line[1]);

	GLdouble line2[2][3] = { { 0.0, 2 * LANE_WIDTH, 0.0 }, { 2000.0, 2 * LANE_WIDTH, 0.0 } };

	glVertex3dv(line2[0]);
	glVertex3dv(line2[1]);

	glEnd();
}

void CViewer::drawLineMarkings(void)
{
	// 車線点群をすべて描く
	glLoadIdentity();
	qglColor(Qt::white);

	glPointSize(6);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);

	int nMax = m_pDatabase->GetNumMarks();

	for (int n = 0; n<NUM_LANE; n++)
	{
		for (int i = 0; i<nMax; i++)
		{
			double dPosX = m_pDatabase->GetData(CDatabase::LANE, n, i, 0);
			double dPosY = m_pDatabase->GetData(CDatabase::LANE, n, i, 1);

			glVertex3f(dPosX, dPosY, 0.1);
		}
	}
	glEnd();
}

void CViewer::drawVehicle( int nType, int nIndex )
{
	double dGlobalX = 0.0;
	double dGlobalY = 0.0;
	double dAttitude = 0.0;
	QColor color;

	
	dGlobalX = m_pDatabase->GetData(CDatabase::SIMULATION, m_nTick, nIndex, PACKET_SIMUL_POSX);
	dGlobalY = m_pDatabase->GetData(CDatabase::SIMULATION, m_nTick, nIndex, PACKET_SIMUL_POSY);

	if (dGlobalX < 0.0)
		return;


	switch (nType)
	{
	case OWN: color = Qt::red; break;
	case ALL: color = Qt::yellow; break;
	}
	
	

	//! 車を指定の位置に描く
	renderVehicle(dGlobalX, dGlobalY, dAttitude, color);
}

void renderVehicle(int nIndex, double dPosX, double dPosY, double dAttitude, bool bTarget)
{
	QColor color;
	double dPotentialValue = 0.0;

	if (bTarget)
	{
		color = Qt::red;
	}
	else
	{
		color = Qt::green;
	}
}

void CViewer::renderVehicle(double dPosX, double dPosY, double dAttitude, const QColor& color)
{
	glPushMatrix();

	//! タイヤを描く
	double dTirePos[4][3] = { { 1.35, 0.78, 0.362 }, { 1.35, -0.78, 0.362 }, { -1.35, 0.78, 0.362 }, { -1.35, -0.78, 0.362 } };
	for (int i = 0; i < 4; i++)
	{
		glLoadIdentity();
		glTranslated(dPosX, dPosY, 0.0);
		glRotatef(dAttitude, 0.0, 0.0, 1.0);

		cylinder(dTirePos[i][0], dTirePos[i][1], dTirePos[i][2], 0.362, 0.2, 20);
	}
	

	glLoadIdentity();
	glTranslated(dPosX, dPosY, 0.0);
	glRotatef(dAttitude, 0.0, 0.0, 1.0);

	//! 高さ指定
	double h1 = 1.705;
	double h2 = 1.2;
	double h3 = 1.0;
	double h4 = 1.0;
	double h0 = 0.362;

	//! sides
	double dx1 = 0.85;
	double dy1 = 0.78;
	double dx2 = 1.35;
	double dy2 = 0.78;
	double dx3 = 2.29;
	double dy3 = 0.6;
	double dx4 = 2.0;
	double dy4 = 0.78;
	double dx5 = 1.8;
	double dy5 = 0.78;

	double r1 = 0.1;
	double r2 = 0.05;
	double wx = 0.13;
	double wy = 0.17;

	// ----------------------- 옆면 4개
	qglColor(color);
	glBegin(GL_QUAD_STRIP);
	glVertex3f(-dx4, -dy4, h4);
	glVertex3f(-dx4, -dy4, h0);

	glVertex3f(-dx5, -dy5, h1);
	glVertex3f(-dx5, -dy5, h0);

	glVertex3f(dx1, -dy1, h1);
	glVertex3f(dx1, -dy1, h0);

	glVertex3f(dx2, -dy2, h2);
	glVertex3f(dx2, -dy2, h0);

	glVertex3f(dx3, -dy3, h3);
	glVertex3f(dx3, -dy3, h0);

	glVertex3f(dx3, dy3, h3);
	glVertex3f(dx3, dy3, h0);

	glVertex3f(dx2, dy2, h2);
	glVertex3f(dx2, dy2, h0);

	glVertex3f(dx1, dy1, h1);
	glVertex3f(dx1, dy1, h0);

	glVertex3f(-dx5, dy5, h1);
	glVertex3f(-dx5, dy5, h0);

	glVertex3f(-dx4, dy4, h4);
	glVertex3f(-dx4, dy4, h0);

	glEnd();

	// 윗면
	glBegin(GL_POLYGON);
	glVertex3f(dx1, dy1, h1);
	glVertex3f(-dx5, dy5, h1);
	glVertex3f(-dx5, -dy5, h1);
	glVertex3f(dx1, -dy1, h1);
	glEnd();

	// 본네트
	glBegin(GL_POLYGON);
	glVertex3f(dx2, dy2, h2);
	glVertex3f(dx3, dy3, h3);
	glVertex3f(dx3, -dy3, h3);
	glVertex3f(dx2, -dy2, h2);
	glEnd();

	// 뒷면
	glBegin(GL_POLYGON);
	glVertex3f(-dx4, dy4, h4);
	glVertex3f(-dx4, -dy4, h4);
	glVertex3f(-dx4, -dy4, h0);
	glVertex3f(-dx4, dy4, h0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(-dx4, dy4, h4);
	glVertex3f(-dx5, dy5, h1);
	glVertex3f(-dx5, -dy5, h1);
	glVertex3f(-dx4, -dy4, h4);
	glEnd();

	// 앞면
	glColor3ub(25, 25, 25);
	glBegin(GL_POLYGON);
	glVertex3f(dx1, dy1, h1);
	glVertex3f(dx2, dy2, h2);
	glVertex3f(dx2, -dy2, h2);
	glVertex3f(dx1, -dy1, h1);
	glEnd();

	// 모서리에 라인을 그려서 강조
	glColor3ub(0, 0, 0);
	//glColor3ub(86, 21, 10);
	glLineStipple(1, 0xFFFF);
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glVertex3f(dx1, dy1, h1);
	glVertex3f(-dx5, dy5, h1);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx4, dy4, h4);
	glVertex3f(-dx4, -dy4, h4);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx5, -dy5, h1);
	glVertex3f(dx1, -dy1, h1);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx1, -dy1, h1);
	glVertex3f(dx1, dy1, h1);
	glBegin(GL_LINES);
	glVertex3f(dx2, dy2, h2);
	glVertex3f(dx3, dy3, h3);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx3, dy3, h3);
	glVertex3f(dx3, -dy3, h3);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx3, -dy3, h3);
	glVertex3f(dx2, -dy2, h2);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx2, -dy2, h2);
	glVertex3f(dx2, dy2, h2);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx1, dy1, h1);
	glVertex3f(dx2, dy2, h2);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx2, -dy2, h2);
	glVertex3f(dx1, -dy1, h1);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx3, dy3, h3);
	glVertex3f(dx3, dy3, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx3, -dy3, h3);
	glVertex3f(dx3, -dy3, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx3, dy3, h0);
	glVertex3f(dx3, -dy3, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx4, dy4, h4);
	glVertex3f(-dx4, dy4, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx4, -dy4, h4);
	glVertex3f(-dx4, -dy4, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx4, dy4, h0);
	glVertex3f(-dx4, -dy4, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx4, dy4, h0);
	glVertex3f(dx2, dy2, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx4, -dy4, h0);
	glVertex3f(dx2, -dy2, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx2, -dy2, h0);
	glVertex3f(dx3, -dy3, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(dx2, dy2, h0);
	glVertex3f(dx3, dy3, h0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx5, dy5, h1);
	glVertex3f(-dx4, dy4, h4);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx5, -dy5, h1);
	glVertex3f(-dx4, -dy4, h4);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-dx5, -dy5, h1);
	glVertex3f(-dx5, dy5, h1);
	glEnd();
}

//円柱
void CViewer::cylinder(double dPosX, double dPosY, double dPosZ, float radius, float width, int sides)
{
	glColor3ub(15, 15, 15);
	glTranslated(dPosX, dPosY, dPosZ);

	double pi = 3.1415;
	//上面
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	for (double i = 0; i < sides; i++) {
		double t = pi * 2 / sides * (double)i;
		glVertex3d(radius * cos(t), width / 2, radius * sin(t));
	}
	glEnd();
	//側面
	qglColor(Qt::black);
	glBegin(GL_QUAD_STRIP);
	for (double i = 0; i <= sides; i = i + 1){
		double t = i * 2 * pi / sides;
		glNormal3f((GLfloat)cos(t), 0.0, (GLfloat)sin(t));
		glVertex3f((GLfloat)(radius*cos(t)), -width / 2, (GLfloat)(radius*sin(t)));
		glVertex3f((GLfloat)(radius*cos(t)), width / 2, (GLfloat)(radius*sin(t)));
	}
	glEnd();
	//下面
	glColor3ub(15, 15, 15);
	glNormal3d(0.0, -1.0, 0.0);
	glBegin(GL_POLYGON);
	for (double i = sides; i >= 0; --i) {
		double t = pi * 2 / sides * (double)i;
		glVertex3d(radius * cos(t), -width / 2, radius * sin(t));
	}
	glEnd();
}