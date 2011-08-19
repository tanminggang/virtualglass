/*
The Controller class is a wrapper object for the
entire VirtualGlass problem. Creating a Controller
object and invoking Controller.run() starts the
application.
*/

#include "controller.h"

Controller::Controller(int argc, char **argv)
{
	app = new QApplication(argc, argv);
	model = new Model();
	mainWindow = new MainWindow(model);

	connect(model, SIGNAL(textMessage(QString)), mainWindow, SLOT(displayTextMessage(QString)));
	connect(model, SIGNAL(modeChanged(int)), mainWindow, SLOT(modeChanged(int)));
	connect(model, SIGNAL(caneChanged()), mainWindow, SLOT(updatePreview()));
	connect(model, SIGNAL(projectionChanged()), mainWindow, SLOT(projectionChanged()));

	connect(model, SIGNAL(caneChanged()), mainWindow->openglWidget, SLOT(caneChanged()));
	connect(model, SIGNAL(projectionChanged()), mainWindow->openglWidget, SLOT(projectionChanged()));

	QSignalMapper* modeSignalMapper = new QSignalMapper(model);
	modeSignalMapper->setMapping(mainWindow->pull_button, PULL_MODE);
	modeSignalMapper->setMapping(mainWindow->bundle_button, BUNDLE_MODE);
	modeSignalMapper->setMapping(mainWindow->case_button, CASING_MODE);
	modeSignalMapper->setMapping(mainWindow->flatten_button, FLATTEN_MODE);
	modeSignalMapper->setMapping(mainWindow->snap_button, SNAP_MODE);
	connect(mainWindow->pull_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(mainWindow->bundle_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(mainWindow->case_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(mainWindow->flatten_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(mainWindow->snap_button, SIGNAL(clicked()), modeSignalMapper, SLOT(map()));
	connect(modeSignalMapper, SIGNAL(mapped(int)), mainWindow, SLOT(newMode(int)));
	connect(mainWindow, SIGNAL(setNewMode(int,bool,Cane*)), model, SLOT(setMode(int,bool,Cane*)));

	connect(mainWindow->undo_button, SIGNAL(pressed()), model, SLOT(undo()));
	connect(mainWindow->redo_button, SIGNAL(pressed()), model, SLOT(redo()));
	connect(mainWindow->clear_button, SIGNAL(pressed()), model, SLOT(clearCurrentCane()));

	connect(mainWindow, SIGNAL(setCaneSig(Cane*)), model, SLOT(setCane(Cane*)));

	// Connect mainwindow buttons to mainwindow
	connect(mainWindow->save_button, SIGNAL(pressed()), mainWindow, SLOT(saveCaneToLibrary()));

	// Connect openglwidget to mainwindow
	connect(mainWindow->openglWidget, SIGNAL(operationInfoSig(QString,int)), mainWindow,
			SLOT(displayTextMessage(QString,int)));

}

int Controller::startUp()
{
	mainWindow->showMaximized();
	mainWindow->seedLibrary();
	model->setMode(LOOK_MODE);
	return app->exec();
}





