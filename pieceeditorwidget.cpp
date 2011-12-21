
#include "pieceeditorwidget.h"

PieceEditorWidget :: PieceEditorWidget(QWidget* parent) : QWidget(parent)
{
	Color* color = new Color;
	color->r = color->g = color->b = color->a = 1.0;
	this->piece = new Piece(TUMBLER_PIECE_TEMPLATE);

	this->pickupViewWidget = new PickupPlanEditorViewWidget(piece->pickup, this);	
	this->niceViewWidget = new NiceViewWidget(PIECE_MODE, this);
	niceViewWidget->setGeometry(&geometry);

	setupLayout();
	setupConnections();
}

void PieceEditorWidget :: updateEverything()
{
        // update pickup stuff
        int value = piece->pickup->getTemplate()->getParameter(0);
        pickupTemplateParameter1SpinBox->setValue(value);
        pickupViewWidget->setPickup(piece->pickup);

        // update pickup stuff
        for (int i = 0; i < pickupTemplateLibraryLayout->count(); ++i)
        {
                if (i + FIRST_PICKUP_TEMPLATE == piece->pickup->getTemplate()->type)
                        highlightLibraryWidget(dynamic_cast<PickupTemplateLibraryWidget*>(
                                dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget()));
                else
                        unhighlightLibraryWidget(dynamic_cast<PickupTemplateLibraryWidget*>(
                                dynamic_cast<QWidgetItem *>(pickupTemplateLibraryLayout->itemAt(i))->widget()));
        }

        // update piece stuff
        geometry.clear();
        mesher.generateMesh(piece, &geometry);
        niceViewWidget->repaint();
        pieceTemplateParameter1Label->setText(piece->getTemplate()->parameterNames[0]);
        pieceTemplateParameter1Slider->setSliderPosition(piece->getTemplate()->parameterValues[0]);
        pieceTemplateParameter2Label->setText(piece->getTemplate()->parameterNames[1]);
        pieceTemplateParameter2Slider->setSliderPosition(piece->getTemplate()->parameterValues[1]);

        // update pickup stuff
        for (int i = 0; i < pieceTemplateLibraryLayout->count(); ++i)
        {
                if (i + FIRST_PIECE_TEMPLATE == piece->getTemplate()->type)
                        highlightLibraryWidget(dynamic_cast<PieceTemplateLibraryWidget*>(
                                dynamic_cast<QWidgetItem *>(pieceTemplateLibraryLayout->itemAt(i))->widget()));
                else
                        unhighlightLibraryWidget(dynamic_cast<PieceTemplateLibraryWidget*>(
                                dynamic_cast<QWidgetItem *>(pieceTemplateLibraryLayout->itemAt(i))->widget()));
        }
}

void PieceEditorWidget :: unhighlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
}

void PieceEditorWidget :: highlightLibraryWidget(PieceTemplateLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
        ((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
        w->graphicsEffect()->setEnabled(true);
}

void PieceEditorWidget :: unhighlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
}

void PieceEditorWidget :: highlightLibraryWidget(PickupTemplateLibraryWidget* w)
{
        w->graphicsEffect()->setEnabled(false);
        ((QGraphicsHighlightEffect*) w->graphicsEffect())->setHighlightType(IS_DEPENDANCY);
        w->graphicsEffect()->setEnabled(true);
}



void PieceEditorWidget :: pieceTemplateParameterSlider2Changed(int)
{
        int value = pieceTemplateParameter2Slider->sliderPosition();

        if (value == piece->getTemplate()->parameterValues[1])
                return;
        piece->getTemplate()->parameterValues[1] = value;
        emit someDataChanged();
}

void PieceEditorWidget :: pieceTemplateParameterSlider1Changed(int)
{
        int value = pieceTemplateParameter1Slider->sliderPosition();

        if (value == piece->getTemplate()->parameterValues[0])
                return;
        piece->getTemplate()->parameterValues[0] = value;
        emit someDataChanged();
}

void PieceEditorWidget :: pickupTemplateParameter1SpinBoxChanged(int)
{
        int value = pickupTemplateParameter1SpinBox->value();

        if (value == piece->pickup->getTemplate()->getParameter(0))
                return;

        piece->pickup->getTemplate()->setParameter(0, value);
        piece->pickup->setTemplate(piece->pickup->getTemplate()); // just push changes through
        emit someDataChanged();
}

void PieceEditorWidget :: setupLayout()
{
        QHBoxLayout* piecePageLayout = new QHBoxLayout(this);
        this->setLayout(piecePageLayout);

        QVBoxLayout* leftLayout = new QVBoxLayout(this);
        piecePageLayout->addLayout(leftLayout);
        leftLayout->addWidget(pickupViewWidget);

        // Setup pickup template scrolling library
        QWidget* pickupTemplateLibraryWidget = new QWidget(this);
        pickupTemplateLibraryLayout = new QHBoxLayout(pickupTemplateLibraryWidget);
        pickupTemplateLibraryLayout->setSpacing(10);
        pickupTemplateLibraryWidget->setLayout(pickupTemplateLibraryLayout);

        QScrollArea* pickupTemplateLibraryScrollArea = new QScrollArea(this);
        pickupTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        pickupTemplateLibraryScrollArea->setWidget(pickupTemplateLibraryWidget);
        pickupTemplateLibraryScrollArea->setWidgetResizable(true);
        pickupTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pickupTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pickupTemplateLibraryScrollArea->setFixedHeight(130);
        leftLayout->addWidget(pickupTemplateLibraryScrollArea, 0);

        pickupTemplateParameter1Label = new QLabel(piece->pickup->getTemplate()->getParameterName(0));
        pickupTemplateParameter1SpinBox = new QSpinBox(this);
        pickupTemplateParameter1SpinBox->setRange(6, 40);
        pickupTemplateParameter1SpinBox->setSingleStep(1);
        pickupTemplateParameter1SpinBox->setValue(1);

        QHBoxLayout* parameter1Layout = new QHBoxLayout(this);
        leftLayout->addLayout(parameter1Layout, 0);
        parameter1Layout->addWidget(pickupTemplateParameter1Label, 0);
        parameter1Layout->addWidget(pickupTemplateParameter1SpinBox, 0);
        parameter1Layout->addStretch(1);

        leftLayout->addStretch(1);
        QLabel* pieceEditorDescriptionLabel = new QLabel("Pickup editor - drag canes in.", this);
        pieceEditorDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        leftLayout->addWidget(pieceEditorDescriptionLabel, 0);

        // Right layout
        QVBoxLayout* rightLayout = new QVBoxLayout(this);
        piecePageLayout->addLayout(rightLayout, 1);
        rightLayout->addWidget(niceViewWidget, 10);

        QWidget* pieceTemplateLibraryWidget = new QWidget(this);
        pieceTemplateLibraryLayout = new QHBoxLayout(pieceTemplateLibraryWidget);
        pieceTemplateLibraryLayout->setSpacing(10);
        pieceTemplateLibraryWidget->setLayout(pieceTemplateLibraryLayout);

        QScrollArea* pieceTemplateLibraryScrollArea = new QScrollArea(this);
        pieceTemplateLibraryScrollArea->setBackgroundRole(QPalette::Dark);
        pieceTemplateLibraryScrollArea->setWidget(pieceTemplateLibraryWidget);
        pieceTemplateLibraryScrollArea->setWidgetResizable(true);
        pieceTemplateLibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pieceTemplateLibraryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pieceTemplateLibraryScrollArea->setFixedHeight(130);
        rightLayout->addWidget(pieceTemplateLibraryScrollArea, 0);

        pieceTemplateParameter1Label = new QLabel(piece->getTemplate()->parameterNames[0]);
        pieceTemplateParameter1Slider = new QSlider(Qt::Horizontal, this);
        pieceTemplateParameter1Slider->setRange(0, 100);
        pieceTemplateParameter1Slider->setTickPosition(QSlider::TicksBothSides);
        pieceTemplateParameter1Slider->setSliderPosition(0);

        QHBoxLayout* pieceParameter1Layout = new QHBoxLayout(this);
        rightLayout->addLayout(pieceParameter1Layout);
        pieceParameter1Layout->addWidget(pieceTemplateParameter1Label);
        pieceParameter1Layout->addWidget(pieceTemplateParameter1Slider);

        pieceTemplateParameter2Label = new QLabel(piece->getTemplate()->parameterNames[1]);
        pieceTemplateParameter2Slider = new QSlider(Qt::Horizontal, this);
        pieceTemplateParameter2Slider->setRange(0, 100);
        pieceTemplateParameter2Slider->setTickPosition(QSlider::TicksBothSides);
        pieceTemplateParameter2Slider->setSliderPosition(0);

        QHBoxLayout* parameter2Layout = new QHBoxLayout(this);
        rightLayout->addLayout(parameter2Layout);
        parameter2Layout->addWidget(pieceTemplateParameter2Label);
        parameter2Layout->addWidget(pieceTemplateParameter2Slider);

        QLabel* niceViewDescriptionLabel = new QLabel("Piece editor.", this);
        niceViewDescriptionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        rightLayout->addWidget(niceViewDescriptionLabel, 0);
}


void PieceEditorWidget :: mousePressEvent(QMouseEvent* event)
{
	PickupTemplateLibraryWidget* pktlw = dynamic_cast<PickupTemplateLibraryWidget*>(childAt(event->pos()));
	PieceTemplateLibraryWidget* ptlw = dynamic_cast<PieceTemplateLibraryWidget*>(childAt(event->pos()));

	if (pktlw != NULL)
        {
                if (pktlw->getPickupTemplateType() != piece->pickup->getTemplate()->type)
                {
			piece->pickup->setTemplate(new PickupTemplate(pktlw->getPickupTemplateType()));
			emit someDataChanged();
		}
        }
	else if (ptlw != NULL)
	{
                if (ptlw->getPieceTemplateType() != piece->getTemplate()->type)
                {
			piece->setTemplate(new PieceTemplate(ptlw->getPieceTemplateType()));
			emit someDataChanged();
		}
	}
}

void PieceEditorWidget :: setupConnections()
{
        connect(pickupTemplateParameter1SpinBox, SIGNAL(valueChanged(int)),
                this, SLOT(pickupTemplateParameter1SpinBoxChanged(int)));
        connect(pieceTemplateParameter1Slider, SIGNAL(valueChanged(int)),
                this, SLOT(pieceTemplateParameterSlider1Changed(int)));
        connect(pieceTemplateParameter2Slider, SIGNAL(valueChanged(int)),
                this, SLOT(pieceTemplateParameterSlider2Changed(int)));

	connect(this, SIGNAL(someDataChanged()), this, SLOT(updateEverything()));
}

void PieceEditorWidget :: seedTemplates()
{
        char filename[100];
        for (int i = FIRST_PICKUP_TEMPLATE; i <= LAST_PICKUP_TEMPLATE; ++i)
        {
                sprintf(filename, ":/images/pickuptemplate%d.png", i);
                PickupTemplateLibraryWidget *pktlw = new PickupTemplateLibraryWidget(
                        QPixmap::fromImage(QImage(filename)), i);
                pickupTemplateLibraryLayout->addWidget(pktlw);
        }

        for (int i = FIRST_PIECE_TEMPLATE; i <= LAST_PIECE_TEMPLATE; ++i)
        {
                sprintf(filename, ":/images/piecetemplate%d.png", i);
                PieceTemplateLibraryWidget *ptlw = new PieceTemplateLibraryWidget(
                        QPixmap::fromImage(QImage(filename)), i);
                pieceTemplateLibraryLayout->addWidget(ptlw);
        }
}

void PieceEditorWidget :: updateLibraryWidgetPixmaps(PieceLibraryWidget* w)
{
	w->updatePixmap(QPixmap::fromImage(niceViewWidget->renderImage()).scaled(100, 100));
}

void PieceEditorWidget :: setPickupTemplateParameter(int param, int value)
{
	piece->pickup->getTemplate()->setParameter(param, value);
	emit someDataChanged();
}

void PieceEditorWidget :: setPickupSubplans(PullPlan* s)
{
	piece->pickup->subplans.clear();
        for (unsigned int i = 0; i < piece->pickup->getTemplate()->subtemps.size(); ++i)
        {
                piece->pickup->subplans.push_back(s);
        }
	emit someDataChanged();
}


void PieceEditorWidget :: setPieceTemplate(PieceTemplate* t)
{
	piece->setTemplate(t);
	emit someDataChanged();
}

void PieceEditorWidget :: setPickupTemplate(PickupTemplate* t)
{
	piece->pickup->setTemplate(t);
	emit someDataChanged();
}

void PieceEditorWidget :: setPiece(Piece* p)
{
	piece = p;
	emit someDataChanged();	
}

Piece* PieceEditorWidget :: getPiece()
{
	return piece;
}




