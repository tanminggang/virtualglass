

#include "mainwindow.h"

MainWindow :: MainWindow()
{
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);

	centralLayout = new QHBoxLayout(centralWidget);
	setupLibrary();
	setupEditors();
	setupConnections();

	setWindowTitle(windowTitle());
	show();

	seedEverything();
	editorStack->setCurrentIndex(EMPTY_MODE); 
	emit someDataChanged();
	whatToDoLabel->setText("Click a library item at left to edit/view.");
}

QString MainWindow :: windowTitle()
{
	QString title = tr("VirtualGlass");
	QFile inFile(":/version.txt");
	if (inFile.open(QIODevice::ReadOnly)) {
		QTextStream in(&inFile);
		QString revision = in.readLine();
		QString date = in.readLine();
		title += " - r" + revision + " built on " + date;
	}
	return title;
}

void MainWindow :: seedEverything()
{
	// Load color stuff
	editorStack->setCurrentIndex(COLORBAR_MODE);
	colorEditorWidget->seedColors();
	emit someDataChanged();

	// Load pull template types
	editorStack->setCurrentIndex(PULLPLAN_MODE);
	emit someDataChanged();
	pullPlanEditorWidget->seedTemplates();

	// Load pickup and piece template types
	editorStack->setCurrentIndex(PIECE_MODE);
	emit someDataChanged();
	pieceEditorWidget->seedTemplates();
}

void MainWindow :: unhighlightAllLibraryWidgets()
{
	QLayoutItem* w;
	for (int j = 0; j < colorBarLibraryLayout->count(); ++j)
	{
		w = colorBarLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<AsyncColorBarLibraryWidget*>(w->widget()));
	}
	for (int j = 0; j < pullPlanLibraryLayout->count(); ++j)
	{
		w = pullPlanLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<AsyncPullPlanLibraryWidget*>(w->widget()));
	}
	for (int j = 0; j < pieceLibraryLayout->count(); ++j)
	{
		w = pieceLibraryLayout->itemAt(j);
		unhighlightLibraryWidget(dynamic_cast<AsyncPieceLibraryWidget*>(w->widget()));
	}
}

void MainWindow :: keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)
		deleteCurrentEditingObject();
}

void MainWindow :: deleteCurrentEditingObject()
{
	QLayoutItem* w;

	switch (editorStack->currentIndex())
	{
		case COLORBAR_MODE:
		{
			if (colorBarLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				w = colorBarLibraryLayout->itemAt(i);
				if (dynamic_cast<AsyncColorBarLibraryWidget*>(w->widget()) == colorEditorWidget->getLibraryWidget())
				{
					// this may be a memory leak, the library widget is never explicitly deleted
					w = colorBarLibraryLayout->takeAt(i); 
					delete w->widget();
					delete w;
					break;	
				}
			}

			colorEditorBarLibraryWidget = dynamic_cast<AsyncColorBarLibraryWidget*>(
								colorBarLibraryLayout->itemAt(
									MIN(colorBarLibraryLayout->count()-1, i))->widget());
			colorEditorWidget->setLibraryWidget(colorEditorBarLibraryWidget);
			emit someDataChanged();
			break;
		}
		case PULLPLAN_MODE:
		{
			if (pullPlanLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				w = pullPlanLibraryLayout->itemAt(i);
				PullPlan* p = dynamic_cast<AsyncPullPlanLibraryWidget*>(w->widget())->getPullPlan();
				if (p == pullPlanEditorWidget->getPlan())
				{
					// this may be a memory leak, the library widget is never explicitly deleted
					w = pullPlanLibraryLayout->takeAt(i);
					delete w->widget();
					delete w;
					break;
				}
			}

			pullPlanEditorPlanLibraryWidget = dynamic_cast<AsyncPullPlanLibraryWidget*>(
							  	pullPlanLibraryLayout->itemAt(
									MIN(pullPlanLibraryLayout->count()-1, i))->widget());
			pullPlanEditorWidget->setPlan(pullPlanEditorPlanLibraryWidget->getPullPlan());
			emit someDataChanged();
			break;
		}
		case PIECE_MODE:
		{
			if (pieceLibraryLayout->count() == 1)
				return;

			int i;
			for (i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				w = pieceLibraryLayout->itemAt(i);
				Piece* p = dynamic_cast<AsyncPieceLibraryWidget*>(w->widget())->getPiece();
				if (p == pieceEditorWidget->getPiece())
				{
					// this may be a memory leak, the library widget is never explicitly deleted
					w = pieceLibraryLayout->takeAt(i);
					delete w->widget();
					delete w;
					break;
				}
			}

			pieceEditorPieceLibraryWidget = dynamic_cast<AsyncPieceLibraryWidget*>(
								pieceLibraryLayout->itemAt(
									MIN(pieceLibraryLayout->count()-1, i))->widget());
			pieceEditorWidget->setPiece(pieceEditorPieceLibraryWidget->getPiece());
			emit someDataChanged();
			break;
		}
	}
	
}

void MainWindow :: mouseReleaseEvent(QMouseEvent* event)
{
	// If this is a drag and not the end of a click, don't process (dropEvent will do it instead)
	if ((event->pos() - dragStartPosition).manhattanLength() > QApplication::startDragDistance())
	{
		return;
	}

	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));
	AsyncPieceLibraryWidget* plw = dynamic_cast<AsyncPieceLibraryWidget*>(childAt(event->pos()));

	if (cblw != NULL)
	{
		unhighlightAllLibraryWidgets();
		colorEditorBarLibraryWidget = cblw;
		colorEditorWidget->setLibraryWidget(cblw);
		editorStack->setCurrentIndex(COLORBAR_MODE);
		emit someDataChanged();
	}
	else if (plplw != NULL)
	{
		unhighlightAllLibraryWidgets();
		pullPlanEditorPlanLibraryWidget = plplw;
		pullPlanEditorWidget->setPlan(plplw->getPullPlan());
		editorStack->setCurrentIndex(PULLPLAN_MODE);
		emit someDataChanged();
	}
	else if (plw != NULL)
	{
		unhighlightAllLibraryWidgets();
		pieceEditorPieceLibraryWidget = plw;
		pieceEditorWidget->setPiece(plw->getPiece());
		editorStack->setCurrentIndex(PIECE_MODE);
		emit someDataChanged();
	}
}


void MainWindow :: mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
		this->dragStartPosition = event->pos();
}

void MainWindow :: mouseMoveEvent(QMouseEvent* event)
{
	void* plan = NULL;
	QPixmap pixmap;

	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;

	AsyncColorBarLibraryWidget* cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(childAt(event->pos()));
	AsyncPullPlanLibraryWidget* plplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(childAt(event->pos()));
	int type;
	if (cblw != NULL)
	{
		plan = cblw->getPullPlan();
		pixmap = *cblw->getEditorPixmap();
		type = COLOR_BAR_MIME;
	}
	else if (plplw != NULL)
	{
		plan = plplw->getPullPlan();
		pixmap = *plplw->getEditorPixmap();
		type = PULL_PLAN_MIME;
	}
	else
		return;

	char buf[500];
	sprintf(buf, "%p %d", plan, type);
	QByteArray pointerData(buf);
	QMimeData* mimeData = new QMimeData;
	mimeData->setText(pointerData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);

	drag->exec(Qt::CopyAction);

}

void MainWindow :: dragMoveEvent(QDragMoveEvent* event)
{
	event->acceptProposedAction();
}

void MainWindow :: setupConnections()
{
	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newColorBarButton, SIGNAL(pressed()), this, SLOT(newColorBar()));
	connect(colorEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPullPlanButton, SIGNAL(pressed()), this, SLOT(newPullPlan()));
	connect(pullPlanEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));

	connect(newPieceButton, SIGNAL(pressed()), this, SLOT(newPiece()));
	connect(pieceEditorWidget, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void MainWindow :: setupLibrary()
{
	QWidget* bigOleLibraryWidget = new QWidget(centralWidget);
	centralLayout->addWidget(bigOleLibraryWidget);

	QVBoxLayout* libraryAreaLayout = new QVBoxLayout(bigOleLibraryWidget);
	bigOleLibraryWidget->setLayout(libraryAreaLayout);

	QScrollArea* libraryScrollArea = new QScrollArea(bigOleLibraryWidget);
	libraryAreaLayout->addWidget(libraryScrollArea, 1);
	libraryScrollArea->setBackgroundRole(QPalette::Dark);
	libraryScrollArea->setWidgetResizable(true);
	libraryScrollArea->setFixedWidth(370);
	libraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	libraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* libraryWidget = new QWidget(libraryScrollArea);
	libraryScrollArea->setWidget(libraryWidget);

	QGridLayout* superlibraryLayout = new QGridLayout(libraryWidget);
	libraryWidget->setLayout(superlibraryLayout);

	newColorBarButton = new QPushButton("New Color", libraryWidget);
	newPullPlanButton = new QPushButton("New Cane", libraryWidget);
	newPieceButton = new QPushButton("New Piece", libraryWidget);
	superlibraryLayout->addWidget(newColorBarButton, 0, 0);
	superlibraryLayout->addWidget(newPullPlanButton, 0, 1);
	superlibraryLayout->addWidget(newPieceButton, 0, 2);

	colorBarLibraryLayout = new QVBoxLayout(libraryWidget);
	colorBarLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pullPlanLibraryLayout = new QVBoxLayout(libraryWidget);
	pullPlanLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	pieceLibraryLayout = new QVBoxLayout(libraryWidget);
	pieceLibraryLayout->setDirection(QBoxLayout::BottomToTop);
	superlibraryLayout->addLayout(colorBarLibraryLayout, 1, 0, Qt::AlignTop);
	superlibraryLayout->addLayout(pullPlanLibraryLayout, 1, 1, Qt::AlignTop);
	superlibraryLayout->addLayout(pieceLibraryLayout, 1, 2, Qt::AlignTop);

	// make three qlabels for a legend
	QGridLayout* legendLayout = new QGridLayout(libraryWidget);
	QLabel* l1 = new QLabel("Used By Selected");
	l1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l1->setStyleSheet("border: 2px dashed " + QColor(0, 139, 69, 255).name() + ";");
	QLabel* l2 = new QLabel("Selected");
	l2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l2->setStyleSheet("border: 3px solid " + QColor(0, 0, 255, 255).name() + ";");
	QLabel* l3 = new QLabel("Uses Selected");
	l3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	l3->setStyleSheet("border: 2px dotted " + QColor(200, 100, 0, 255).name() + ";");
	legendLayout->addWidget(l1,0,1);
	legendLayout->addWidget(l2,0,2);
	legendLayout->addWidget(l3,0,3);
	libraryAreaLayout->addLayout(legendLayout, 0);

	QLabel* descriptionLabel = new QLabel("Library - click to edit or drag to add.",
		libraryWidget);
	descriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	libraryAreaLayout->addWidget(descriptionLabel, 0);
}

void MainWindow :: setupEditors()
{
	editorStack = new QStackedWidget(centralWidget);
	centralLayout->addWidget(editorStack, 2);

	setupEmptyPaneEditor();
	editorStack->addWidget(emptyEditorPage);

	setupColorEditor();
	editorStack->addWidget(colorEditorWidget);

	setupPullPlanEditor();
	editorStack->addWidget(pullPlanEditorWidget);

	setupPieceEditor();
	editorStack->addWidget(pieceEditorWidget);
}

void MainWindow :: setupPieceEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pieceEditorWidget = new PieceEditorWidget(editorStack);
	pieceEditorPieceLibraryWidget = new AsyncPieceLibraryWidget(pieceEditorWidget->getPiece());
	pieceLibraryLayout->addWidget(pieceEditorPieceLibraryWidget);
}

void MainWindow :: setupEmptyPaneEditor()
{
	emptyEditorPage = new QWidget(editorStack);
	QHBoxLayout* editorLayout = new QHBoxLayout(emptyEditorPage);
	emptyEditorPage->setLayout(editorLayout);
	whatToDoLabel = new QLabel("Loading...", emptyEditorPage);
	whatToDoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	editorLayout->addWidget(whatToDoLabel, 0);
}

void MainWindow :: setupColorEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	colorEditorBarLibraryWidget = new AsyncColorBarLibraryWidget(new PullPlan(CIRCLE_BASE_PULL_TEMPLATE), "R-100", this);
	colorEditorWidget = new ColorEditorWidget(colorEditorBarLibraryWidget, editorStack);
	colorBarLibraryLayout->addWidget(colorEditorBarLibraryWidget);
}

void MainWindow :: setupPullPlanEditor()
{
	// Setup data objects - the current plan and library widget for this plan
	pullPlanEditorWidget = new PullPlanEditorWidget(editorStack);
	pullPlanEditorPlanLibraryWidget = new AsyncPullPlanLibraryWidget(pullPlanEditorWidget->getPlan());
	pullPlanLibraryLayout->addWidget(pullPlanEditorPlanLibraryWidget);
}

void MainWindow :: newPiece()
{
	// Create the new piece
	Piece* newEditorPiece = new Piece(TUMBLER_PIECE_TEMPLATE);

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	pieceEditorPieceLibraryWidget = new AsyncPieceLibraryWidget(newEditorPiece);
	pieceLibraryLayout->addWidget(pieceEditorPieceLibraryWidget);

	pieceEditorWidget->setPiece(newEditorPiece);

	// Load up the right editor
	editorStack->setCurrentIndex(PIECE_MODE);

	emit someDataChanged();
}

void MainWindow :: newColorBar()
{
	PullPlan* newEditorBar = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE);

	// Create the new library entry
	unhighlightAllLibraryWidgets();
	colorEditorBarLibraryWidget = new AsyncColorBarLibraryWidget(newEditorBar, "R-100");
	colorBarLibraryLayout->addWidget(colorEditorBarLibraryWidget);
	colorEditorWidget->setLibraryWidget(colorEditorBarLibraryWidget);

	// Give the new plan to the editor
	colorEditorWidget->setLibraryWidget(colorEditorBarLibraryWidget);

	// Load up the right editor
	editorStack->setCurrentIndex(COLORBAR_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}

void MainWindow :: newPullPlan()
{
	PullPlan *newEditorPlan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE);
	newEditorPlan->setTemplateType(CASED_CIRCLE_PULL_TEMPLATE);
	emit newPullPlan(newEditorPlan);
}

void MainWindow :: newPullPlan(PullPlan* newPlan)
{
	unhighlightAllLibraryWidgets();
	pullPlanEditorPlanLibraryWidget = new AsyncPullPlanLibraryWidget(newPlan);
	pullPlanLibraryLayout->addWidget(pullPlanEditorPlanLibraryWidget);

	// Give the new plan to the editor
	pullPlanEditorWidget->setPlan(newPlan);

	// Load up the right editor
	editorStack->setCurrentIndex(PULLPLAN_MODE);

	// Trigger GUI updates
	emit someDataChanged();
}


void MainWindow :: unhighlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(AsyncColorBarLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(AsyncPullPlanLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: unhighlightLibraryWidget(AsyncPieceLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
}

void MainWindow :: highlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(AsyncColorBarLibraryWidget* w, int dependancy)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(dependancy);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(AsyncPullPlanLibraryWidget* w, int dependancy)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(dependancy);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: highlightLibraryWidget(AsyncPieceLibraryWidget* w, int dependancy)
{
	w->graphicsEffect()->setEnabled(false);
	((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(dependancy);
	w->graphicsEffect()->setEnabled(true);
}

void MainWindow :: updateEverything()
{
	switch (editorStack->currentIndex())
	{
		case COLORBAR_MODE:
			colorEditorWidget->updateEverything();
			break;
		case PULLPLAN_MODE:
			pullPlanEditorWidget->updateEverything();
			break;
		case PIECE_MODE:
			pieceEditorWidget->updateEverything();
			break;
	}

	updateLibrary();
}

void MainWindow :: updateLibrary()
{
	unhighlightAllLibraryWidgets();

	switch (editorStack->currentIndex())
	{
		case COLORBAR_MODE:
		{
			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pplw->getPullPlan()->hasDependencyOn(
					colorEditorBarLibraryWidget->getPullPlan()->getOutermostCasingColor()))
					highlightLibraryWidget(pplw, USES_DEPENDANCY);				
			}

			AsyncPieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<AsyncPieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(
					colorEditorBarLibraryWidget->getPullPlan()->getOutermostCasingColor()))
					highlightLibraryWidget(plw, USES_DEPENDANCY);
			}

			highlightLibraryWidget(colorEditorBarLibraryWidget, IS_DEPENDANCY);
			break;
		}
		case PULLPLAN_MODE:
		{
			pullPlanEditorWidget->updateLibraryWidgetPixmaps(pullPlanEditorPlanLibraryWidget);

			AsyncColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(
					cblw->getPullPlan()->getOutermostCasingColor()))
					highlightLibraryWidget(cblw, IS_USED_BY_DEPENDANCY);
			}

			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
						dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pullPlanEditorWidget->getPlan()->hasDependencyOn(pplw->getPullPlan()))
						highlightLibraryWidget(pplw, IS_USED_BY_DEPENDANCY);
				else if (pplw->getPullPlan()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
						highlightLibraryWidget(pplw, USES_DEPENDANCY);
			}

			AsyncPieceLibraryWidget* plw;
			for (int i = 0; i < pieceLibraryLayout->count(); ++i)
			{
				plw = dynamic_cast<AsyncPieceLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pieceLibraryLayout->itemAt(i))->widget());
				if (plw->getPiece()->hasDependencyOn(pullPlanEditorWidget->getPlan()))
					highlightLibraryWidget(plw, USES_DEPENDANCY);
			}

			highlightLibraryWidget(pullPlanEditorPlanLibraryWidget, IS_DEPENDANCY);
			break;
		}
		case PIECE_MODE:
		{
			pieceEditorWidget->updateLibraryWidgetPixmaps(pieceEditorPieceLibraryWidget);

			AsyncColorBarLibraryWidget* cblw;
			for (int i = 0; i < colorBarLibraryLayout->count(); ++i)
			{
				cblw = dynamic_cast<AsyncColorBarLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(colorBarLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(
					cblw->getPullPlan()->getOutermostCasingColor()))
					highlightLibraryWidget(cblw, IS_USED_BY_DEPENDANCY);
			}

			AsyncPullPlanLibraryWidget* pplw;
			for (int i = 0; i < pullPlanLibraryLayout->count(); ++i)
			{
				pplw = dynamic_cast<AsyncPullPlanLibraryWidget*>(
					dynamic_cast<QWidgetItem *>(pullPlanLibraryLayout->itemAt(i))->widget());
				if (pieceEditorWidget->getPiece()->hasDependencyOn(pplw->getPullPlan()))
					highlightLibraryWidget(pplw, IS_USED_BY_DEPENDANCY);
			}

			highlightLibraryWidget(pieceEditorPieceLibraryWidget, IS_DEPENDANCY);
			break;
		}
	}
}


