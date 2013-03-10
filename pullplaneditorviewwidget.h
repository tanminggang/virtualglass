
#ifndef PULLPLANEDITORVIEWWIDGET_H
#define PULLPLANEDITORVIEWWIDGET_H

#include <QWidget>
#include <set>
#include "primitives.h"
#include "shape.h"

class GlassColor;
class PullPlan;
class PullPlanLibraryWidget;
class GlassColorLibraryWidget;

using std::set;

class PullPlanEditorViewWidget : public QWidget
{
	Q_OBJECT

	public:
		PullPlanEditorViewWidget(PullPlan* plan, QWidget* parent=0);
		void setPullPlan(PullPlan* plan);
		QRect usedRect();
		static QPixmap renderPullPlan(PullPlan* plan);
	
	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void dropEvent(QDropEvent* de);
		void dragLeaveEvent(QDragLeaveEvent* dle);
		void dragEnterEvent(QDragEnterEvent* dee);
		void dragMoveEvent(QDragMoveEvent* dme);
		void paintEvent(QPaintEvent *event);
		void resizeEvent(QResizeEvent* event);

	private:
		void paintShape(Point2D upperLeft, float size, enum GeometricShape s, QPainter* p);
		void setBoundaryPainter(QPainter* painter, bool outermostLevel);
		void drawSubplan(Point2D upperLeft, float width, float height, PullPlan* plan, 
			bool highlightThis, bool outermostLevel, QPainter* painter);
		void updateHighlightedSubplansAndCasings(QDragMoveEvent* event);
		bool isOnCasing(int casingIndex, Point2D loc);
		float getShapeRadius(enum GeometricShape s, Point2D loc);
		void setMinMaxCasingRadii(float* min, float* max);
		PullPlan* getSubplanAt(Point2D loc);
		int getSubplanIndexAt(Point2D loc);
		int getCasingIndexAt(Point2D loc);
		Point2D mouseToCaneCoords(float x, float y);

		PullPlan* plan;

		bool isDraggingCasing;
		unsigned int draggedCasingIndex;

		Color highlightColor;
		set<unsigned int> casingsHighlighted;
		set<unsigned int> subplansHighlighted; 

		Point2D drawUpperLeft;
		float squareSize;

	public slots:
		void updateEverything();
};


#endif

