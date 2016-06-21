#include "closeView.h"
#include "database.h"

#include <gl/GLU.h>
#include <QWheelEvent>
#include <qmath.h>

/*********************************************************************/
CCloseView::CCloseView(QWidget *parent) : QGLWidget(parent)
{
	m_pDatabase = CDatabase::GetInstance();

	this->setFixedSize( 400, 300 );
	m_nTick = 0;
}
/*********************************************************************/
/* Public functions */
void CCloseView::Update( int tick )
{
	m_nTick = tick;
	update();
}

void CCloseView::Initialize( void )
{
	m_fZoom = 60.0;
}
/*********************************************************************/
/* Private member functions */
void CCloseView::initializeGL()
{
    qglClearColor( QColor(160,216,239) );
    glEnable( GL_DEPTH_TEST );
}

void CCloseView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glLoadIdentity();
    glOrtho(-1 , 1 , -1 , 1 , 2 , 4);
}

void CCloseView::wheelEvent(QWheelEvent *event)
{
	float delta = (float)event->delta();

    m_fZoom += (delta/120.0);

    updateGL();
}

void CCloseView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
	gluPerspective(90.0, (double)width() / (double)height(), 1.0, 80.0);

    double dGlobalPosX = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_X);
	double dGlobalPosY = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_Y);
	double dAttitude = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_ATT);

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
		double dVehicleLength = 2.29;

		double dAttitude2 = qDegreesToRadians(dAttitude);
		double dViewEndX = 50.0 * qCos(dAttitude2);
		double dViewEndY = 50.0 * qSin(dAttitude2);

		double dViewStartX = dVehicleLength * qCos(dAttitude2);
		double dViewStartY = dVehicleLength * qSin(dAttitude2);

		gluLookAt(dGlobalPosX + dViewStartX, dGlobalPosY + dViewStartY, 1.2, dGlobalPosX + dViewEndX, dGlobalPosY + dViewEndY, 0.0, 0.0, 0.0, 1.0);
	}

	glMatrixMode(GL_MODELVIEW);
	
	// 3. draw the highway
	drawHighway(dGlobalPosX, dGlobalPosY, dAttitude);
	drawLine();

	// 4. draw vehicles
	drawVehicle( OWN, 0 );

	// 5. draw adjacent vehicles
#if defined(SELECTED_VEHICLE)
	int nNumChangingVehicle = m_pDatabase->GetNumChangingVehicles();
	for( int n=0; n<nNumChangingVehicle; n++ )
	{
		drawVehicle( CHANGING, n );
	}

	int nNumKeepingVehicle = m_pDatabase->GetNumKeepingVehicles();
	for( int n=0; n<nNumKeepingVehicle; n++ )
	{
		drawVehicle( KEEPING, n );
	}
#elif defined(ALL_VEHICLE)
	int nNumAdjacentVehicles = m_pDatabase->GetNumAdjacentVehicles();
	for (int n = 0; n < nNumAdjacentVehicles; n++)
	{
		drawVehicle(ALL, n);
	}
#endif

    glFlush();
}

/*********************************************************************/
void CCloseView::drawHighway(double dGlobalOwnX, double dGlobalOwnY, double dAttitude)
{
	glPushMatrix();
	glLoadIdentity();

	glTranslated(dGlobalOwnX, dGlobalOwnY, 0.0);
	glRotatef(dAttitude, 0.0, 0.0, 1.0);

	qglColor(Qt::darkGray);

	// 윗면
	glBegin(GL_POLYGON);

	glVertex3f(-1000.0, -10.0, 0.0);
	glVertex3f(-1000.0, 10.0, 0.0);
	glVertex3f(1000.0, 10.0, 0.0);
	glVertex3f(1000.0, -10.0, 0.0);
	
	glEnd();
}

void CCloseView::drawLine(void)
{
	// 車線点群をすべて描く
	glLoadIdentity();
	qglColor(Qt::white);

	//glPointSize(3.0);
	//glBegin(GL_POINTS);

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xFCFC);
	glLineWidth(6.0);

	glBegin(GL_LINES);

	int nMax = m_pDatabase->GetNumMarks();

	for (int n = 0; n<NUM_LANE; n++)
	{
		for (int i = 0; i<nMax-1; i++)
		{
			double dPosX1 = m_pDatabase->GetData(CDatabase::LANE, n, i, 0);
			double dPosY1 = m_pDatabase->GetData(CDatabase::LANE, n, i, 1);

			double dPosX2 = m_pDatabase->GetData(CDatabase::LANE, n, i+1, 0);
			double dPosY2 = m_pDatabase->GetData(CDatabase::LANE, n, i+1, 1);

			GLdouble line[2][3] = { { dPosX1, dPosY1, 0.0 }, { dPosX2, dPosY2, 0.0 } };

			glVertex3dv(line[0]);
			glVertex3dv(line[1]);
		}
	}

	glEnd();

#if 0
	// 自車が走行している車線の両白線を描く
	glLoadIdentity();
	qglColor(Qt::yellow);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xFCFC);
	glLineWidth(4.0);

	glBegin(GL_LINES);

	int nCurrentLane = m_pDatabase->GetCurrentLane();
	double dOwnPosX = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_X);
	double dOwnPosY = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_Y);

	for (int i = 0; i<nMax - 1; i++)
	{
		double dPtX1 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane, i, 0);
		double dPtY1 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane, i, 1);

		double dPtX2 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane, i + 1, 0);
		double dPtY2 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane, i + 1, 1);

		double dDistance = qSqrt((dOwnPosX - dPtX1)*(dOwnPosX - dPtX1) + (dOwnPosY - dPtY1)*(dOwnPosY - dPtY1));
		if (dDistance > LASER_SENS_AREA)
			continue;

		GLdouble line[2][3] = { { dPtX1, dPtY1, 0.0 }, { dPtX2, dPtY2, 0.0 } };

		glVertex3dv(line[0]);
		glVertex3dv(line[1]);
	}

	for (int i = 0; i<nMax - 1; i++)
	{
		double dPtX1 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane + 1, i, 0);
		double dPtY1 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane + 1, i, 1);

		double dPtX2 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane + 1, i + 1, 0);
		double dPtY2 = m_pDatabase->GetData(CDatabase::LANE, nCurrentLane + 1, i + 1, 1);

		double dDistance = qSqrt((dOwnPosX - dPtX1)*(dOwnPosX - dPtX1) + (dOwnPosY - dPtY1)*(dOwnPosY - dPtY1));
		if (dDistance > LASER_SENS_AREA)
			continue;

		GLdouble line[2][3] = { { dPtX1, dPtY1, 0.0 }, { dPtX2, dPtY2, 0.0 } };

		glVertex3dv(line[0]);
		glVertex3dv(line[1]);
	}

	glEnd();
#endif
}

void CCloseView::drawVehicle(int type, int index)
{
	double dGlobalX = 0.0;
	double dGlobalY = 0.0;
	double dAttitude = 0.0;
	QColor color;

	int nColumn = 0;
	int nNumChangingVehicle = m_pDatabase->GetNumChangingVehicles();

	switch (type)
	{
	case OWN:
		dGlobalX = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_X);
		dGlobalY = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_Y);
		dAttitude = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_ATT);
		color = Qt::yellow;
		break;

	case CHANGING:
		nColumn = index * 3 + 5;
		dGlobalX = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, nColumn);
		dGlobalY = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, nColumn + 1);
		//dAttitude = m_pDatabase->GetData( CDatabase::MEASUREMENT, m_nTick, nColumn+2 );
		dAttitude = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_ATT);
		color = Qt::red;
		break;

	case KEEPING:
		nColumn = index * 3 + nNumChangingVehicle * 3 + 6;
		dGlobalX = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, nColumn);
		dGlobalY = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, nColumn + 1);
		//dAttitude = m_pDatabase->GetData( CDatabase::MEASUREMENT, m_nTick, nColumn+2 );
		dAttitude = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_ATT);
		color = Qt::green;
		break;

	case ALL:
		nColumn = index * 3 + 5;
		dGlobalX = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, nColumn);
		dGlobalY = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, nColumn + 1);
		dAttitude = m_pDatabase->GetData(CDatabase::MEASUREMENT, m_nTick, PACKET_OWN_GLOBAL_ATT);
		int nClass = m_pDatabase->GetEstimationResultData(m_nTick, index, RESULT_PACKET_CLASS);
		if (nClass == 2)
			color = Qt::red;
		else
			color = Qt::green;
		break;
	}

	//! 車を指定の位置に描く
	renderVehicle(dGlobalX, dGlobalY, dAttitude, color);
}

void CCloseView::renderVehicle(double dPosX, double dPosY, double dAttitude, const QColor& color)
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
void CCloseView::cylinder(double dPosX, double dPosY, double dPosZ, float radius, float width, int sides)
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