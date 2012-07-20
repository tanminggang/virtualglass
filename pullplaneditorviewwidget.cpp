
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setMinimumSize(200, 200);
	this->plan = plan;
	isDraggingCasing = false;
	casingHighlighted = false;
}


QRect PullPlanEditorViewWidget :: usedRect()
{
	return QRect(ulX, ulY, squareSize, squareSize);
}

void PullPlanEditorViewWidget :: resizeEvent(QResizeEvent* event)
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

float PullPlanEditorViewWidget :: adjustedX(float rawX)
{
	return rawX - ulX;
}

float PullPlanEditorViewWidget :: adjustedY(float rawY)
{
	return rawY - ulY;
}

float PullPlanEditorViewWidget :: rawX(float adjustedX)
{
	return adjustedX + ulX;
}

float PullPlanEditorViewWidget :: rawY(float adjustedY)
{
	return adjustedY + ulY;
}

float PullPlanEditorViewWidget :: getShapeRadius(int shape, float x, float y)
{
	switch (shape)
	{
		case CIRCLE_SHAPE:
			return sqrt(x * x + y * y); 
		case SQUARE_SHAPE:
			return MAX(fabs(x), fabs(y));
	}

	return -1;
}

bool PullPlanEditorViewWidget :: isOnCasing(int casingIndex, float x, float y)
{
	return fabs(plan->getCasingThickness(casingIndex) - getShapeRadius(plan->getCasingShape(casingIndex), x, y)) < 0.02; 
}

void PullPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	float x = (adjustedX(event->pos().x()) - squareSize/2) / float(squareSize/2-10);
	float y = (adjustedY(event->pos().y()) - squareSize/2) / float(squareSize/2-10);

	// Check for casing resize
	for (unsigned int i = 0; i < plan->getCasingCount() - 1; ++i) 
	{
		if (isOnCasing(i, x, y))
		{
			isDraggingCasing = true; 
			draggedCasingIndex = i;
			return;
		}	
	}

	// Check for convenience subplan-to-subplan drag
	PullPlan* selectedSubplan = getSubplanAt(x, y);
	if (selectedSubplan != NULL)
	{
		int type;
		QPixmap pixmap;

		if (selectedSubplan->isBase())
		{
			type = COLOR_BAR_MIME;
			AsyncColorBarLibraryWidget cblw(selectedSubplan);		
			pixmap = *(cblw.getDragPixmap());
		}
		else
		{
			type = PULL_PLAN_MIME;
			AsyncPullPlanLibraryWidget plplw(selectedSubplan);		
			pixmap = *(plplw.getDragPixmap());
		}

	        char buf[500];
		encodeMimeData(buf, selectedSubplan, type);
		QByteArray pointerData(buf);
		QMimeData* mimeData = new QMimeData;
		mimeData->setText(pointerData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);

		drag->exec(Qt::CopyAction);
	}	
}

PullPlan* PullPlanEditorViewWidget :: getSubplanAt(float x, float y)
{
        // Recursively call drawing on subplans
        for (unsigned int i = 0; i < plan->subs.size(); ++i)
        {
                SubpullTemplate* sub = &(plan->subs[i]);
		if (getShapeRadius(sub->shape, x - sub->location.x, y - sub->location.y) < sub->diameter/2.0)
			return sub->plan;
	}			

	return NULL;
}

void PullPlanEditorViewWidget :: setMinMaxCasingRadii(float* min, float* max)
{
	/*
	Goal here is to deal with casings of different shapes, and set upper and 
	lower bounds for the radius of a particular casing based upon how much it
	can change before bumping into the inscribed (next smallest) or 
	circumscribed (next largest) casing. 

	The major situation/issue to deal with is adjusting by sqrt(2) in the case
	that the casing is square and is surrounded by two circle casings or vice versa.
	We also allow adjacent casings of different shapes to get really close (0.01)
	while casing of the same shape need to be spaced a little (0.05) for UX reasons:
	if they are too close, it becomes hard to tell there are two casings, or to 
	click on them reliably to resize them.
	*/
	
	int csi, csi_minus_1, csi_plus_1;
	float cti_minus_1, cti_plus_1;

	if (draggedCasingIndex == 0) 
	{
		csi = plan->getCasingShape(0);
		csi_minus_1 = csi;
		csi_plus_1 = plan->getCasingShape(1);
	
		cti_minus_1 = 0.0;
		cti_plus_1 = plan->getCasingThickness(1);	
        }
	else 
	{
		csi = plan->getCasingShape(draggedCasingIndex);
		csi_minus_1 = plan->getCasingShape(draggedCasingIndex-1);
		csi_plus_1 = plan->getCasingShape(draggedCasingIndex+1);
		cti_minus_1 = plan->getCasingThickness(draggedCasingIndex-1);
		cti_plus_1 = plan->getCasingThickness(draggedCasingIndex+1);
        }
		
	if (csi == CIRCLE_SHAPE && csi_minus_1 == SQUARE_SHAPE)
		*min = cti_minus_1 * sqrt(2.0) + 0.01;
	else if (csi == SQUARE_SHAPE && csi_minus_1 == CIRCLE_SHAPE)
		*min = cti_minus_1 + 0.01;
	else
		*min = cti_minus_1 + 0.02;

	if (csi == SQUARE_SHAPE && csi_plus_1 == CIRCLE_SHAPE)
		*max = cti_plus_1 / sqrt(2.0) - 0.01;
	else if (csi == CIRCLE_SHAPE && csi_plus_1 == SQUARE_SHAPE)
		*max = cti_plus_1 - 0.01;
	else
		*max = cti_plus_1 - 0.02;
} 

void PullPlanEditorViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	// bug in here?
	if (!isDraggingCasing)
		return;

	float x = (adjustedX(event->pos().x()) - squareSize/2);
	float y = (adjustedY(event->pos().y()) - squareSize/2);
	float radius = getShapeRadius(plan->getCasingShape(draggedCasingIndex), x, y) / (squareSize/2 - 10);

	float min;
	float max;

	setMinMaxCasingRadii(&min, &max);	
	plan->setCasingThickness(MIN(MAX(radius, min), max), draggedCasingIndex);

	emit someDataChanged();
}

void PullPlanEditorViewWidget :: mouseReleaseEvent(QMouseEvent* /*event*/)
{
	isDraggingCasing = false;
}

void PullPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
        decodeMimeData(event->mimeData()->text().toAscii().constData(), &draggedPlan, &draggedPlanType);
	isDraggingPlan = true;
}

void PullPlanEditorViewWidget :: dragLeaveEvent(QDragLeaveEvent* /*event*/)
{
	subplansHighlighted.clear();
	casingHighlighted = false;
	isDraggingPlan = false;
}


void PullPlanEditorViewWidget :: updateHighlightedSubplansAndCasings()
{
	QPoint mouse = mapFromGlobal(QCursor::pos());
	int x = adjustedX(mouse.x());
	int y = adjustedY(mouse.y());

        subplansHighlighted.clear();
        casingHighlighted = false;

        populateHighlightedSubplans(x, y);
        if (subplansHighlighted.size() > 0) 
        {
                switch (draggedPlanType) 
                {
                        case COLOR_BAR_MIME:
                                draggingColor = *(draggedPlan->getOutermostCasingColor());
                                break;
                        default:
                                draggingColor.r = draggingColor.g = draggingColor.b = draggingColor.a = 1.0;
                                break;
                }
        }
        else
        {
                populateHighlightedCasings(x, y);
		if (casingHighlighted)
			draggingColor = *(draggedPlan->getOutermostCasingColor());
        }
}

void PullPlanEditorViewWidget :: dragMoveEvent(QDragMoveEvent* /*event*/)
{
	updateHighlightedSubplansAndCasings();
	repaint();
}

void PullPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	if (subplansHighlighted.size() > 0)
	{
		event->accept();
		for (unsigned int i = 0; i < subplansHighlighted.size(); ++i)
		{
			plan->subs[subplansHighlighted[i]].plan = draggedPlan;
		}
	}
	else if (casingHighlighted)
	{
		event->accept();
		// at this point we have already checked that the dropped plan is a color bar
		// (that was done in updateHighlightedSubplansAndCasings()). So now we can just
		// take the color w/o thinking.
		plan->setCasingColor(draggedPlan->getOutermostCasingColor(), casingHighlightIndex);
	}

	isDraggingPlan = false;
	subplansHighlighted.clear();
	casingHighlighted = false;
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: populateHighlightedSubplans(int x, int y)
{
	subplansHighlighted.clear();

	if (!isDraggingPlan || !(draggedPlanType == COLOR_BAR_MIME || draggedPlanType == PULL_PLAN_MIME))
		return;

	if (draggedPlan->hasDependencyOn(plan)) // don't allow circular DAGs
		return;

	int drawSize = squareSize - 20;
	// check to see if the drop was in a subpull
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->subs[i]);

		// Determine if drop hit the subplan
		float dx = x - (drawSize/2 * subpull->location.x + drawSize/2 + 10);
		float dy = y - (drawSize/2 * subpull->location.y + drawSize/2 + 10);
		bool hit = (getShapeRadius(subpull->shape, dx, dy) < (subpull->diameter/2.0) * drawSize/2);
		
		if (!hit)
			continue;

		// If the dropped plan is a complex plan and its casing shape doesn't match the shape of the
		// subplan, reject
		if (draggedPlanType == PULL_PLAN_MIME)
		{
			if (subpull->shape != draggedPlan->getOutermostCasingShape())
			{
				continue;
			}
		}

		// If the shift button is down, fill in the entire group
		subplansHighlighted.push_back(i);
	}
	
	// if user is hovering over a subplan and the shift key is currently held down, fill in all subplans
	// Note that this needs to wait until another event (say, a drag move) occurs to catch the shift button being down.
	// The crazy thing is, on Windows a drag *blocks* the event loop, preventing the whole application from
	// getting a keyPressEvent() until the drag is completed. So reading keyboardModifiers() actually 
	// lets you notice that the shift key is down earlier, i.e. during the drag, which is the only time you care anyway.
	if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) && subplansHighlighted.size() > 0)
	{
		subplansHighlighted.clear();	
		for (unsigned int i = 0; i < plan->subs.size(); ++i)
		{
			subplansHighlighted.push_back(i);
		}
	}
}


void PullPlanEditorViewWidget :: populateHighlightedCasings(int x, int y)
{
	casingHighlighted = false;

	if (!isDraggingPlan || draggedPlanType != COLOR_BAR_MIME)
		return; 

	// Deal w/casing
	float drawSize = (squareSize - 20);
	float distanceFromCenter;
	for (unsigned int i = 1; i < plan->getCasingCount(); ++i) {
		switch (plan->getCasingShape(i)) {
			case CIRCLE_SHAPE:
				distanceFromCenter = sqrt(pow(x - (drawSize/2.0 + 10.0), 2.0)
					+ pow(y - (drawSize/2.0 + 10.0), 2.0));
				if (distanceFromCenter <= drawSize/2 * plan->getCasingThickness(i))
				{
					casingHighlighted = true;
					casingHighlightIndex = i;
					return;
				}
				break;
			case SQUARE_SHAPE:
				if (MAX(fabs(x - squareSize/2.0), fabs(y - squareSize/2.0)) < drawSize/2 * plan->getCasingThickness(i))
				{
					casingHighlighted = true;
					casingHighlightIndex = i;
					return;
				}
				break;
		}
	}
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan) {

	this->plan = plan;
	updateHighlightedSubplansAndCasings();
}


void PullPlanEditorViewWidget :: setBoundaryPainter(QPainter* painter, bool outermostLevel) {

	if (outermostLevel)
	{
		QPen pen;
		pen.setWidth(3);
		pen.setColor(Qt::black);
		painter->setPen(pen);
	}
	else
	{
		QPen pen;
		pen.setWidth(1);
		pen.setColor(Qt::black);
		painter->setPen(pen);
	}

}

void PullPlanEditorViewWidget :: paintShape(float x, float y, float size, int shape, QPainter* painter)
{
        int roundedX, roundedY;

        roundedX = floor(rawX(x) + 0.5);
        roundedY = floor(rawY(y) + 0.5);

	switch (shape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(roundedX, roundedY, size, size);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(roundedX, roundedY, size, size);
			break;
	}
	
}


void PullPlanEditorViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, bool highlightThis, int mandatedShape, bool outermostLevel, QPainter* painter) {

	// Fill the subplan area with some `cleared out' color
	painter->setBrush(QColor(200, 200, 200));
	painter->setPen(Qt::NoPen);
	paintShape(x, y, drawWidth, mandatedShape, painter);

	// Do casing colors outermost to innermost to get concentric rings of each casing's color
	// Skip outermost casing (that is done by your parent) and innermost (that is the `invisible'
	// casing for you to resize your subcanes)
	for (unsigned int i = plan->getCasingCount() - 1; plan->getCasingCount() > i && i > 0; --i) 
	{
		int casingWidth = drawWidth * plan->getCasingThickness(i);
		int casingHeight = drawHeight * plan->getCasingThickness(i);
		int casingX = x + drawWidth / 2 - casingWidth / 2;
		int casingY = y + drawHeight / 2 - casingHeight / 2;

		// Fill with solid neutral grey (in case fill is transparent)
		painter->setBrush(QColor(200, 200, 200));
		painter->setPen(Qt::NoPen); // Will draw boundary after all filling is done
		paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
		
		// Fill with actual casing color (highlighting white or some other color)
		if (outermostLevel && casingHighlighted && i == casingHighlightIndex)	
		{
			painter->setBrush(QColor(255*draggingColor.r, 255*draggingColor.g, 255*draggingColor.b, 
				255*draggingColor.a));
		}
		else
		{
			painter->setBrush(QColor(255*plan->getCasingColor(i)->r, 255*plan->getCasingColor(i)->g, 
				255*plan->getCasingColor(i)->b, 255*plan->getCasingColor(i)->a));
		}
		setBoundaryPainter(painter, outermostLevel);
		paintShape(casingX, casingY, casingWidth, plan->getCasingShape(i), painter);
	}

	// If you're supposed to become highlighted, do it. Note: this is not a casing highlight, that
	// was already (just) done when processing casing...this is a subcane being highlighted.
	if (highlightThis)
	{
		painter->setBrush(QColor(255*draggingColor.r, 255*draggingColor.g, 255*draggingColor.b, 
			255*draggingColor.a));
		painter->setPen(Qt::NoPen);
		paintShape(x, y, drawWidth, mandatedShape, painter);
		return;
	}

	// If you're a color bar, just fill region with color.
	if (plan->isBase())
	{
		Color* c = plan->getOutermostCasingColor();
		painter->setBrush(QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
		painter->setPen(Qt::NoPen);
		paintShape(x, y, drawWidth, mandatedShape, painter);
		return;
	}

	// Recursively call drawing on subplans
	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		if (outermostLevel) {
			bool highlighted = false;
			for (unsigned int j = 0; j < subplansHighlighted.size(); ++j) {
				if (subplansHighlighted[j] == i)
					highlighted = true;
			}
			if (highlighted) {
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					true, plan->subs[i].shape, false, painter);
			}
			else {
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					false, plan->subs[i].shape, false, painter);

			}
		}
		else {
			drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
				false, plan->subs[i].shape, 
				false, painter);
		}
		
		setBoundaryPainter(painter, outermostLevel);
		painter->setBrush(Qt::NoBrush);
		paintShape(rX, rY, rWidth, plan->subs[i].shape, painter);
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;

	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, squareSize - 20, squareSize - 20, plan, false, 
		plan->getOutermostCasingShape(), true, &painter);

	setBoundaryPainter(&painter, true);
	paintShape(10, 10, squareSize - 20, plan->getOutermostCasingShape(), &painter);

	painter.end();
}



