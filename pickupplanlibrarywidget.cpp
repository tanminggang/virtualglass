
#include "pickupplanlibrarywidget.h"
#include "pullplan.h"

PickupPlanLibraryWidget :: PickupPlanLibraryWidget(QPixmap niceViewPixmap, QPixmap editorPixmap, PickupPlan* plan,
	QWidget* parent): QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setFixedSize(100, 100);
	setScaledContents(true);
	setMouseTracking(true);

	setPixmap(niceViewPixmap);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
	this->pickupPlan = plan;
	this->pickupPlan->setLibraryWidget(this);
	this->editorPixmap = editorPixmap;

	setGraphicsEffect(new QGraphicsColorizeEffect());
}

PickupPlan* PickupPlanLibraryWidget :: getPickupPlan()
{
	return pickupPlan;
}

void PickupPlanLibraryWidget :: updatePixmaps(QPixmap niceViewPixmap, QPixmap editorPixmap)
{
		setPixmap(niceViewPixmap);
	this->editorPixmap = editorPixmap;
}

const QPixmap* PickupPlanLibraryWidget :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

