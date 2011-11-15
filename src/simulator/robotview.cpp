#include "robotview.h"

RobotView::RobotView() : QGraphicsView()
{
	setDragMode(QGraphicsView::ScrollHandDrag);
// 	setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	setMinimumSize(600, 400);
}

RobotView::~RobotView()
{
}

void RobotView::wheelEvent(QWheelEvent *event)
{
     scaleView(pow((double)2, -event->delta() / 240.0));
}

void RobotView::scaleView(double scaleFactor)
{
     qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
     if (factor < 0.07 || factor > 100)
         return;

     scale(scaleFactor, scaleFactor);
}
