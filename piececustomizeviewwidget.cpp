
#include <QResizeEvent>
#include <QPainter>
#include "piececustomizeviewwidget.h"
#include "piece.h"
#include "constants.h"

PieceCustomizeViewWidget :: PieceCustomizeViewWidget(Piece* _piece, QWidget* _parent) : QWidget(_parent)
{
	// setup draw widget
	setMinimumSize(200, 200);
	this->piece = _piece;
	this->viewSize = 22.0;	

	isDraggingControlPoint = false;
	draggedControlPointIndex = 0;
}

QRect PieceCustomizeViewWidget :: usedRect()
{
	return QRect(ulX, ulY, squareSize, squareSize);
}

void PieceCustomizeViewWidget :: wheelEvent(QWheelEvent *e)
{
	if (e->delta() > 0)
		viewSize *= 0.8; 
	else if (e->delta() < 0)	
		viewSize *= 1.2;
	updateEverything();
}

void PieceCustomizeViewWidget :: resizeEvent(QResizeEvent* event)
{
	int width, height;

	width = event->size().width();
	height = event->size().height();

	if (width > height) // wider than tall 
	{
		ulX = (width - height)/2.0;
		ulY = 0;
		squareSize = height; 
	}
	else  
	{
		ulX = 0;
		ulY = (height - width)/2.0;
		squareSize = width; 
	}
}

float PieceCustomizeViewWidget :: adjustedX(float rawX)
{
	float blowup = squareSize / viewSize;
	return (rawX - ulX) / blowup;
}

float PieceCustomizeViewWidget :: adjustedY(float rawY)
{
	float blowup = squareSize / viewSize;
	return (rawY - ulY) / blowup;
}

float PieceCustomizeViewWidget :: rawX(float adjustedX)
{
	float blowup = squareSize / viewSize;
	return adjustedX * blowup + ulX;
}

float PieceCustomizeViewWidget :: rawY(float adjustedY)
{
	float blowup = squareSize / viewSize;
	return adjustedY * blowup + ulY;
}

float PieceCustomizeViewWidget :: rawScale(float adjustedScale)
{
	float blowup = squareSize / viewSize;
	return adjustedScale * blowup; 
}

float PieceCustomizeViewWidget :: adjustedScale(float rawScale)
{
	float blowup = squareSize / viewSize;
	return rawScale / blowup; 
}

Point PieceCustomizeViewWidget :: controlPointRawLocation(unsigned int index)
{
	Point p;

	p.x = rawX(viewSize/2 - piece->spline.values()[index]); 
	p.y = rawY(viewSize/2 - 10 * index / (static_cast<float>(piece->spline.values().size()) - 1.0) + 5.0);

	return p;
} 

void PieceCustomizeViewWidget :: mousePressEvent(QMouseEvent* event)
{
	// get press position
	Point mouse;
	mouse.x = event->pos().x();
	mouse.y = event->pos().y();
	
	// check and see if it's on a control point
	for (unsigned int i = 0; i < piece->spline.values().size(); ++i)
	{
		Point p = controlPointRawLocation(i);
		if (fabs(mouse.x - p.x) + fabs(mouse.y - p.y) < 4 * MAX(squareSize / 100, 1))
		{
			isDraggingControlPoint = true;
			draggedControlPointIndex = i;
			return;
		}
	}	
}

void PieceCustomizeViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	if (!isDraggingControlPoint)
		return;

	Point mouse;
	mouse.x = event->pos().x();
	float delta = adjustedScale(mouse.x - controlPointRawLocation(draggedControlPointIndex).x);	
	piece->spline.set(draggedControlPointIndex, piece->spline.values()[draggedControlPointIndex] - delta);	
	emit someDataChanged();
}

void PieceCustomizeViewWidget :: mouseReleaseEvent(QMouseEvent*)
{
	isDraggingControlPoint = false;
}

void PieceCustomizeViewWidget :: updateEverything()
{
	this->repaint();	
}

void PieceCustomizeViewWidget :: setPiece(Piece* _piece)
{
	this->piece = _piece;
	this->viewSize = 22.0;
	updateEverything();
}

void PieceCustomizeViewWidget :: drawPiece()
{
	// draw the spline
	Spline& spline = piece->spline;

	// note that pixels are specified from upper left, so many Ys
	// are inverted. we assume a canvas of size 20 x 20 and adjust
	// using a "blowup" multiplier

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setColor(QColor(0, 0, 0));
	pen.setWidth(MAX(squareSize/100, 1) * 2);
	pen.setCapStyle(Qt::RoundCap);
	painter.setPen(pen);
	painter.setBrush(QColor(100, 100, 100));

	// draw spline
	QPointF start;
	QPointF end;
	float center = viewSize/2;
	for (float t = 0.0; t < 1.0; t += 0.01)
	{
		float t_delta = t + 0.01;	

		// draw right side
		start.setX(rawX(center + spline.get(t)));
		start.setY(rawY(center - t * 10.0 + 5.0));	
		end.setX(rawX(center + spline.get(t_delta)));
		end.setY(rawY(center - t_delta * 10.0 + 5.0));
		painter.drawLine(QLineF(start, end));

		// draw mirrored left side (y-values the same)
		start.setX(rawX(center - spline.get(t)));
		end.setX(rawX(center - spline.get(t_delta)));
		painter.drawLine(QLineF(start, end));
	}

	// draw control points
        painter.setBrush(QColor(0, 0, 0, 255));
        pen.setWidth(2);
        pen.setColor(Qt::white);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
	for (unsigned int i = 0; i < spline.values().size(); ++i)
	{
		Point p = controlPointRawLocation(i);
		start.setX(p.x);
		start.setY(p.y);
		int pointRadius = MAX(squareSize / 100, 1) * 2;
                painter.drawEllipse(start, pointRadius, pointRadius);
	}

	painter.end();
}

void PieceCustomizeViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	painter.end();
	drawPiece();
}
