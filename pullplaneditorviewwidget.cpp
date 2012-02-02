
#include "pullplaneditorviewwidget.h"

PullPlanEditorViewWidget :: PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent) : QWidget(parent)
{
	// setup draw widget
	setAcceptDrops(true);
	setFixedSize(500, 500);
	this->plan = plan;
	fill_rule = SINGLE_FILL_RULE;
	isDraggingCasing = false;
	casingHighlighted = false;
}

int PullPlanEditorViewWidget :: getFillRule()
{
	return fill_rule;
}

void PullPlanEditorViewWidget :: setFillRule(int r)
{
	fill_rule = r;
}

void PullPlanEditorViewWidget :: mousePressEvent(QMouseEvent* event)
{
	float x = (event->pos().x() - width()/2);
	float y = (event->pos().y() - height()/2);
	float radius = sqrt(x * x + y * y) / (width()/2 - 10); 

	if (fabs(radius - (1.0 - plan->getCasingThickness())) < 20)
	{
		isDraggingCasing = true; 
	}
}

void PullPlanEditorViewWidget :: mouseMoveEvent(QMouseEvent* event)
{
	float x = (event->pos().x() - width()/2);
	float y = (event->pos().y() - height()/2);
	if (isDraggingCasing)
	{
		float radius = sqrt(x * x + y * y) / (width()/2 - 10); 
		plan->setCasingThickness(MAX(1.0 - radius, 0));
		emit someDataChanged();
	}
}

void PullPlanEditorViewWidget :: mouseReleaseEvent(QMouseEvent* /*event*/)
{
	isDraggingCasing = false;
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void PullPlanEditorViewWidget :: dragMoveEvent(QDragMoveEvent* event)
{
	PullPlan* draggedPlan;
	int type;
	sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &draggedPlan, &type);

	subplansHighlighted.clear();
	casingHighlighted = false;

	populateHighlightedSubplans(event->pos().x(), event->pos().y(), draggedPlan, type);
	if (subplansHighlighted.size() == 0)
	{
		populateIsCasingHighlighted(event->pos().x(), event->pos().y(), type);
	}
	emit someDataChanged();
}

void PullPlanEditorViewWidget :: dropEvent(QDropEvent* event)
{
	PullPlan* droppedPlan;
	int type;
	sscanf(event->mimeData()->text().toAscii().constData(), "%p %d", &droppedPlan, &type);

	populateHighlightedSubplans(event->pos().x(), event->pos().y(), droppedPlan, type);
	if (subplansHighlighted.size() > 0)
	{
		event->accept();
		for (unsigned int i = 0; i < subplansHighlighted.size(); ++i)
		{
			plan->subs[subplansHighlighted[i]].plan = droppedPlan;
		}
	}
	else
	{
		populateIsCasingHighlighted(event->pos().x(), event->pos().y(), type);
		if (casingHighlighted)
		{
			event->accept();
			plan->setCasingColor(droppedPlan->getCasingColor());
		}
	}

	subplansHighlighted.clear();
	casingHighlighted = false;
	emit someDataChanged();

}


void PullPlanEditorViewWidget :: populateHighlightedSubplans(int x, int y, PullPlan* droppedPlan, int type)
{
	subplansHighlighted.clear();

	if (!(type == COLOR_BAR_MIME || type == PULL_PLAN_MIME))
		return;

	if (droppedPlan->hasDependencyOn(plan)) // don't allow circular DAGs
		return;

	int drawSize = width() - 20;
	// check to see if the drop was in a subpull
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		SubpullTemplate* subpull = &(plan->subs[i]);

		// Determine if drop hit the subplan
		bool hit = false;
		float dx = fabs(x - (drawSize/2 * subpull->location.x + drawSize/2 + 10));
		float dy = fabs(y - (drawSize/2 * subpull->location.y + drawSize/2 + 10));
		switch (subpull->shape)
		{
			case CIRCLE_SHAPE:
				if (pow(double(dx*dx + dy*dy), 0.5) < (subpull->diameter/2.0)*drawSize/2)
					hit = true;
				break;
			case SQUARE_SHAPE:
				if (MAX(dx, dy) < (subpull->diameter/2.0)*drawSize/2)
					hit = true;
				break;
		}

		if (!hit)
			continue;

		// If the dropped plan is a complex plan and its casing shape doesn't match the shape of the
		// subplan, reject
		if (type == PULL_PLAN_MIME)
		{
			if (subpull->shape != droppedPlan->getCasingShape())
			{
				continue;
			}
		}

		// If the shift button is down, fill in the entire group
		switch (fill_rule)
		{
			case SINGLE_FILL_RULE:
			{
				subplansHighlighted.push_back(i);	
				break;
			}
			case ALL_FILL_RULE:
			{
				for (unsigned int j = 0; j < plan->subs.size(); ++j)
					subplansHighlighted.push_back(j);
				break;
			}
			case GROUP_FILL_RULE:
			{
				int group = plan->subs[i].group;
				for (unsigned int j = i; j < plan->subs.size(); ++j)
				{
					if (plan->subs[j].group == group)
						subplansHighlighted.push_back(j);
				}
				break;
			}
			case EVERY_OTHER_FILL_RULE:
			{
				int group = plan->subs[i].group;
				bool parity = true;
				for (unsigned int j = i; j < plan->subs.size(); ++j)
				{
					if (plan->subs[j].group == group)
					{
						if (parity)
							subplansHighlighted.push_back(j);
						parity = !parity;
					}
				}
				break;
			}
			case EVERY_THIRD_FILL_RULE:
			{
				int group = plan->subs[i].group;
				int triarity = 0;
				for (unsigned int j = i; j < plan->subs.size(); ++j)
				{
					if (plan->subs[j].group == group)
					{
						if (triarity == 0)
							subplansHighlighted.push_back(j);
						triarity = (triarity + 1) % 3;
					}
				}
				break;
			}
		}
	}
}


void PullPlanEditorViewWidget :: populateIsCasingHighlighted(int x, int y, int type)
{
	casingHighlighted = false;

	if (type != COLOR_BAR_MIME)
		return; 

	// Deal w/casing
	int drawSize = width() - 20;
	float distanceFromCenter;
	switch (plan->getCasingShape())
	{
		case CIRCLE_SHAPE:
			distanceFromCenter = sqrt(pow(double(x - drawSize/2 + 10), 2.0)
				+ pow(double(y - drawSize/2 + 10), 2.0));
			if (distanceFromCenter <= drawSize/2)
			{
				casingHighlighted = true;
				return;
			}
			break;
		case SQUARE_SHAPE:
			if (10 <= x && x <= drawSize
				&& 10 <= y && y <= drawSize)
			{
				casingHighlighted = true;
				return;
			}
			break;
	}
}

void PullPlanEditorViewWidget :: setPullPlan(PullPlan* plan)
{
	this->plan = plan;
}


void PullPlanEditorViewWidget :: drawSubplan(float x, float y, float drawWidth, float drawHeight, 
	PullPlan* plan, bool highlightThis, int mandatedShape, 
	int borderLevels, QPainter* painter)
{
	// Fill the subplan area with some `cleared out' color
	painter->setBrush(QColor(200, 200, 200));
	painter->setPen(Qt::NoPen);
	switch (mandatedShape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(x, y, drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(x, y, drawWidth, drawHeight);
			break;
	}

	// If it's a base color, fill region with color
	if (plan->isBase())
	{
		Color* c = plan->getCasingColor();
		painter->setBrush(QColor(255*c->r, 255*c->g, 255*c->b, 255*c->a));
		painter->setPen(Qt::NoPen);

		switch (mandatedShape)
		{
			case CIRCLE_SHAPE:
				painter->drawEllipse(x, y, drawWidth, drawHeight);
				break;
			case SQUARE_SHAPE:
				painter->drawRect(x, y, drawWidth, drawHeight);
				break;
		}
	}

	// Draw casing shape
	painter->setBrush(Qt::NoBrush);
	if (MIN(drawWidth, drawHeight) < 10)
	{
                painter->setPen(Qt::NoPen);
	}
	else if (borderLevels == 2)
	{
		QPen pen;
		pen.setWidth(5);
		if (highlightThis)
			pen.setColor(Qt::white);
		else
			pen.setColor(Qt::black);
		painter->setPen(pen);
	}
	else if (borderLevels == 1)
	{
		QPen pen;
		pen.setWidth(3);
		if (highlightThis)
			pen.setColor(Qt::white);
		else
			pen.setColor(Qt::black);
		painter->setPen(pen);
	}
	else
	{
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::black);
		pen.setStyle(Qt::DotLine);
		painter->setPen(pen);
	}
	painter->setBrush(QColor(255*plan->getCasingColor()->r, 255*plan->getCasingColor()->g, 
		255*plan->getCasingColor()->b, 255*plan->getCasingColor()->a));
	switch (mandatedShape)
	{
		case CIRCLE_SHAPE:
			painter->drawEllipse(x, y, drawWidth, drawHeight);
			break;
		case SQUARE_SHAPE:
			painter->drawRect(x, y, drawWidth, drawHeight);
			break;
	}

	if (plan->isBase())
		return;

	// Recurse. Draw unhighlighted subplans first
	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		if (borderLevels == 2)
		{
			bool highlighted = false;
			for (unsigned int j = 0; j < subplansHighlighted.size(); ++j)
			{
				if (subplansHighlighted[j] == i)
					highlighted = true;
			}
			if (!highlighted)
			{
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					false, plan->subs[i].shape, 
					borderLevels-1, painter);
			}
		}
		else
		{
			drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
				false, plan->subs[i].shape, 
				borderLevels-1, painter);
		}
	}

	// Recurse. Now draw highlighted subplans 
	for (unsigned int i = plan->subs.size()-1; i < plan->subs.size(); --i)
	{
		SubpullTemplate* sub = &(plan->subs[i]);

		float rX = x + (sub->location.x - sub->diameter/2.0) * drawWidth/2 + drawWidth/2;
		float rY = y + (sub->location.y - sub->diameter/2.0) * drawWidth/2 + drawHeight/2;
		float rWidth = sub->diameter * drawWidth/2;
		float rHeight = sub->diameter * drawHeight/2;

		if (borderLevels == 2)
		{
			bool highlighted = false;
			for (unsigned int j = 0; j < subplansHighlighted.size(); ++j)
			{
				if (subplansHighlighted[j] == i)
					highlighted = true;
			}
			if (highlighted)
			{
				drawSubplan(rX, rY, rWidth, rHeight, plan->subs[i].plan, 
					true, plan->subs[i].shape, 
					borderLevels-1, painter);
			}
		}
	}
}

void PullPlanEditorViewWidget :: paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(event->rect(), QColor(200, 200, 200));
	drawSubplan(10, 10, width() - 20, height() - 20, plan, casingHighlighted, plan->getCasingShape(), 2, &painter);
	painter.end();
}



