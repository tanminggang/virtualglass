
#ifndef PIECEEDITORWIDGET_H
#define PIECEEDITORWIDGET_H

#include <QtGui>
#include "piece.h"
#include "pullplan.h"
#include "overlaycolorwidget.h"
#include "pickupplaneditorviewwidget.h"
#include "asyncpiecelibrarywidget.h"
#include "pickuptemplatelibrarywidget.h"
#include "piecetemplatelibrarywidget.h"
#include "qgraphicshighlighteffect.h"
#include "niceviewwidget.h"
#include "geometry.h"
#include "mesh.h"

class PieceEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		PieceEditorWidget(QWidget* parent=0);
		Piece* getPiece();
		void updateLibraryWidgetPixmaps(AsyncPieceLibraryWidget* w);
		void seedTemplates();
		void setPiece(Piece* p);
		void setPieceTemplate(PieceTemplate* t);
		void setPickupTemplateType(int templateType);
		void setPickupParameter(int param, int value);
		void mousePressEvent(QMouseEvent*);

	signals:
		void someDataChanged();

	public slots:
		void updateEverything();

	private slots:
                void pickupViewWidgetDataChanged();
                void fillRuleComboBoxChanged(int);
		void pieceTemplateParameterSlider1Changed(int);
		void pieceTemplateParameterSlider2Changed(int);
		void pieceTemplateParameterSlider3Changed(int);
		void pickupParameter1SpinBoxChanged(int);
		void pickupParameter1SliderChanged(int);
		void pickupParameter2SpinBoxChanged(int);
		void pickupParameter2SliderChanged(int);

	private:
		Geometry geometry;
		Mesher mesher;
		Piece* piece;
		PickupPlanEditorViewWidget* pickupViewWidget;	
		NiceViewWidget* niceViewWidget;

		OverlayColorWidget* overlayWidget;
		OverlayColorWidget* underlayWidget;
                QLabel overlayColorLabel;
                QLabel underlayColorLabel;
                QComboBox* fillRuleComboBox;

		vector<QLabel*> pickupParamLabels;
		vector<QSpinBox*> pickupParamSpinboxes;
		vector<QSlider*> pickupParamSliders;
		vector<QWidget*> pickupParamWidgets;
		vector<QLabel*> pieceParamLabels;
		vector<QSlider*> pieceParamSliders;
		vector<QWidget*> pieceParamWidgets;

		QHBoxLayout* pickupTemplateLibraryLayout;
		QHBoxLayout* pieceTemplateLibraryLayout;
		QButtonGroup* shapeButtonGroup;
		QSlider* casingThicknessSlider;
		QHBoxLayout* templateLibraryLayout;
		QPushButton* addCasingButton;

		void setupLayout();
		void setupConnections();
		void highlightLibraryWidget(PickupTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PickupTemplateLibraryWidget* w);
		void highlightLibraryWidget(PieceTemplateLibraryWidget* w);
		void unhighlightLibraryWidget(PieceTemplateLibraryWidget* w);
};


#endif

