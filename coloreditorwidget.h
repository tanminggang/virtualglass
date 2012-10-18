
#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <vector>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QString>

class GlassColor;
class PullPlan;
class Mesher;
class Geometry;
class PureColorLibraryWidget;
class NiceViewWidget;
class QVBoxLayout;
class QComboBox;
class QSlider;

using std::vector;
             
class ColorEditorWidget : public QWidget
{
	Q_OBJECT

	public:
		ColorEditorWidget(GlassColor* _color, QWidget* parent=0);
		void setGlassColor(GlassColor* _color);
		GlassColor* getGlassColor();

	signals:
		void someDataChanged();

	protected:
		void mousePressEvent(QMouseEvent* event);

	public slots:
		void updateEverything();
                void alphaSliderPositionChanged(int);
		void sourceComboBoxChanged(int);		

	private:
		Geometry* geometry;
		Mesher* mesher;
		GlassColor* glassColor;	
		QComboBox* sourceComboBox;
		NiceViewWidget* niceViewWidget;
		QVBoxLayout* editorLayout;
                QSlider* alphaSlider;
		vector<QScrollArea*> colorLibraryScrollAreas;
		vector<QVBoxLayout*> colorLibraryLayouts;

		void setupLayout();
		void setupConnections();
		void loadColors(QString fileName);
};


#endif

