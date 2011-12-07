
#include "pullplan.h"

PullPlan :: PullPlan(int pullTemplate, Color* color)
{
	// initialize all the variables
	this->pullTemplate = new PullTemplate(pullTemplate);
	this->color = color;
	this->twist = 0.0;
	this->libraryWidget = NULL;
}

bool PullPlan :: hasDependencyOn(PullPlan* plan)
{
	if (this == plan)
		return true;
	if (this->pullTemplate->isBase())
		return false;

        bool childrenAreDependent = false;
        for (unsigned int i = 0; i < subplans.size(); ++i)
        {
                if (subplans[i]->hasDependencyOn(plan))
                {
                        childrenAreDependent = true;
                        break;
                }
        }	

	return childrenAreDependent;
}

bool PullPlan :: hasDependencyOn(Color* color)
{
	if (this->color == color) 
		return true;
	if (this->pullTemplate->isBase())
		return false;

	bool childrenAreDependent = false;
	for (unsigned int i = 0; i < subplans.size(); ++i)
	{
		if (subplans[i]->hasDependencyOn(color))
		{
			childrenAreDependent = true;
			break;
		}
	}
	
	return childrenAreDependent;
}


void PullPlan :: setLibraryWidget(PullPlanLibraryWidget* plplw)
{
	this->libraryWidget = plplw;
}

PullPlanLibraryWidget* PullPlan :: getLibraryWidget()
{
	return this->libraryWidget;
}

void PullPlan :: setTemplate(PullTemplate* newTemplate)
{
	// Find reference circular and square subcanes
        Color* color;
	color = new Color();
	color->r = color->g = color->b = 1.0;
	color->a = 0.0;
	PullPlan* circlePullPlan = new PullPlan(CIRCLE_BASE_PULL_TEMPLATE, color);
	PullPlan* squarePullPlan = new PullPlan(SQUARE_BASE_PULL_TEMPLATE, color);

	for (unsigned int i = 0; i < MIN(pullTemplate->subtemps.size(), subplans.size()); ++i)
	{
		if (this->pullTemplate->subtemps[i].shape == CIRCLE_SHAPE
			&& (circlePullPlan->color->a < 0.0001 || circlePullPlan == NULL))
			circlePullPlan = this->subplans[i];
		else if (this->pullTemplate->subtemps[i].shape == SQUARE_SHAPE
			&& (squarePullPlan->color->a < 0.0001 || squarePullPlan == NULL))
			squarePullPlan = this->subplans[i];
	}

	// create the new subplans based on template
	this->pullTemplate = newTemplate;
	this->subplans.clear();
	for (unsigned int i = 0; i < newTemplate->subtemps.size(); ++i)
	{
		switch (newTemplate->subtemps[i].shape)
		{
			case CIRCLE_SHAPE:
				subplans.push_back(circlePullPlan);
				break;
			case SQUARE_SHAPE:
				subplans.push_back(squarePullPlan);
				break;
		}
	}
}

PullTemplate* PullPlan :: getTemplate()
{
	return this->pullTemplate;
}

const QPixmap* PullPlan :: getEditorPixmap()
{
	return &(this->editorPixmap);
}

void PullPlan :: updatePixmap(QPixmap editorPixmap)
{
	this->editorPixmap = editorPixmap;
}






