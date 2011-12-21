
#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <QtGui>
#include "pullplan.h"
#include "qgraphicshighlighteffect.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"
#include "coloreditorviewwidget.h"
#include "colorbarlibrarywidget.h"

class ColorEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorWidget(QWidget* parent=0);
		Color* getColor();
		void setColor(float r, float g, float b, float a);
		PullPlan* getColorBar();
		void setColorBar(PullPlan* b);
		void updateLibraryWidgetPixmaps(ColorBarLibraryWidget* w);
		void seedColors();

	signals:
		void someDataChanged();

	public slots:
		void updateEverything();
		void viewWidgetDataChanged();

	private:
		Geometry geometry;
		Mesher mesher;
		PullPlan* colorBar;	
		NiceViewWidget* niceViewWidget;
		ColorEditorViewWidget* viewWidget;

		void setupLayout();
		void setupConnections();
};


#endif

