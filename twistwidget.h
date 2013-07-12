

#ifndef TWISTWIDGET_H
#define TWISTWIDGET_H

#include <QWidget>

class QDoubleSpinBox;
class QSlider;
class QPushButton;

class TwistWidget : public QWidget
{
	Q_OBJECT

	public:
		TwistWidget(float* twist, unsigned int range, QWidget* parent=0);
		void setTwist(float* twist);

	protected:
		bool eventFilter(QObject* obj, QEvent* event);

	signals:
		void valueChanged();
		void valueChangeEnded();

	public slots:
		void updateEverything();

	private slots:
		void sliderValueChanged(int);
		void sliderChangeEnded();
		void spinValueChanged(double);

	private:
		QDoubleSpinBox* spin;
		QSlider* slider;
		float* twist;
		unsigned int range;
};

#endif


