
#include <QColor>
#include <QString>
#include <QWidget>
#include "templatelibrarywidget.h"

TemplateLibraryWidget :: TemplateLibraryWidget(QPixmap _view, QWidget* _parent) : QLabel(_parent)
{
	setFixedSize(100, 100);
	setScaledContents(true);
	setPixmap(_view);
	setHighlighted(false);
}

void TemplateLibraryWidget::setHighlighted(bool isHighlighted)
{
	QColor color;
	QString style;

	if (isHighlighted)
		color = QColor(0, 0, 255);	
	else
		color = QColor(200, 200, 200);

	style.append("border: 2px solid " + color.name() + ";");
	setStyleSheet(style);
}



