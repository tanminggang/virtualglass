

#include "subpickuptemplate.h"

SubpickupTemplate :: SubpickupTemplate(PullPlan* _plan, Point _location, int _orientation, float _length, 
	float _width, enum GeometricShape _shape)
{
	plan = _plan;
	location = _location;
	orientation = _orientation;
	length = _length;
	width = _width;
	shape = _shape;
}


