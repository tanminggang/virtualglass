#ifndef CANELIBRARYWIDGET_H
#define CANELIBRARYWIDGET_H

#include "asyncrenderwidget.h"
#include "shape.h"

class Cane;

class CaneLibraryWidget : public AsyncRenderWidget
{
	Q_OBJECT

	public:
		CaneLibraryWidget(Cane *_cane, MainWindow* _window);
		Cane *cane;
		const QPixmap *dragPixmap();
		bool isDraggable();
		GlassMime::Type mimeType();

	public slots:
		void updatePixmaps();

	private:
		QPixmap _dragPixmap;
		void updateDragPixmap();
};

#endif 
