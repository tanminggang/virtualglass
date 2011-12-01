
#include "pickupplan.h"

PickupPlan :: PickupPlan(int pickupTemplate)
{
	setTemplate(new PickupTemplate(pickupTemplate));
}

PickupPlan* PickupPlan :: copy()
{
	PickupPlan* c = new PickupPlan(this->pickupTemplate->type);

	c->pickupTemplate = this->pickupTemplate->copy();
	c->subplans.clear();
	for (unsigned int i = 0; i < this->subplans.size(); ++i)
	{
		c->subplans.push_back(this->subplans[i]);
	}

	return c;
}

void PickupPlan :: setTemplate(PickupTemplate* newTemplate)
{
        // load these into the pull plan
        this->pickupTemplate = newTemplate;
        this->subplans.clear();
        for (unsigned int i = 0; i < this->getTemplate()->subpulls.size(); ++i)
        {
                Color* color = new Color();
                color->r = color->g = color->b = 1.0;
                color->a = 0.4;
		switch (this->pickupTemplate->subpulls[i]->shape)
		{
			case CIRCLE_SHAPE:
				this->subplans.push_back(new PullPlan(CIRCLE_BASE_TEMPLATE, true, color));
				break;
			case SQUARE_SHAPE:
				this->subplans.push_back(new PullPlan(SQUARE_BASE_TEMPLATE, true, color));
				break;
		}
        }
}

PickupTemplate* PickupPlan :: getTemplate()
{
	return this->pickupTemplate;
}







