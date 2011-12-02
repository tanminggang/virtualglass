#include "qgraphicshighlighteffect.h"

/*QGraphicsHighlightEffect::QGraphicsHighlightEffect(QObject *parent) :
	QGraphicsEffect(parent)
{
}*/

QGraphicsHighlightEffect::QGraphicsHighlightEffect( qreal offset ) : QGraphicsEffect(),
	mColor( 255, 255, 0, 255 ),  // yellow, semi-transparent
	mOffset( offset, offset )
{
	isActive = false;
}

QRectF QGraphicsHighlightEffect::boundingRectFor( const QRectF &sourceRect) const
{
	return sourceRect.adjusted( -mOffset.x(), -mOffset.y(), mOffset.x(), mOffset.y() );
}

void QGraphicsHighlightEffect :: setStyleSheet(bool enableBorder)
{
	if (enableBorder)
		emit styleSheetString("border: 1px solid "+color().name()+";");
	else
		emit styleSheetString("border: 0px solid "+color().name()+";");
}

void QGraphicsHighlightEffect::setActiveMain(bool active)
{
	if (!active && this->isActive)
		mColor = QColor(255,255,0,255);
	else if (active && !this->isActive)
		mColor = QColor(0,0,255,255);
	this->isActive=active;
}

void QGraphicsHighlightEffect::draw( QPainter *painter )
{
	QPoint offset;
	QPixmap pixmap;

	// if ( sourceIsPixmap() ) // doesn't seems to work, return false
	{
		// No point in drawing in device coordinates (pixmap will be scaled anyways).
		pixmap = sourcePixmap( Qt::LogicalCoordinates, &offset);  //, mode );
	}

	QRectF bound = boundingRectFor( pixmap.rect() );

	painter->save();
	painter->setPen( Qt::NoPen );
	painter->setBrush( mColor );
	QPointF p( offset.x()-mOffset.x(), offset.y()-mOffset.y() );
	bound.moveTopLeft( p );
	painter->drawRoundedRect( bound, 5, 5, Qt::RelativeSize );
	int g=200;
	painter->setBrush(QColor(g,g,g,255));
	QRectF rect = pixmap.rect();
	rect.setWidth(rect.width()-4.5);
	rect.setHeight(rect.height()-4.5);
	//rect.moveTopLeft(p);
	painter->drawRect(rect);
	painter->drawPixmap( offset, pixmap );
	painter->restore();
	//painter->drawPixmap( offset, pixmap );
}