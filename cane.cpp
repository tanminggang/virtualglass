/*
Initializes a cane node (i.e. a constructed top-level
definition of a cane).
*/

#include "cane.h"

Cane :: Cane(int type)
{
	reset();
	this->type = type;
}

// Resets the object to the default values of everything
void Cane :: reset()
{
	int i;

	type = UNASSIGNED_CANETYPE;
	for (i = 0; i < MAX_AMT_TYPES; ++i)
	{
		amts[i] = 0.0;
	}

	subcaneCount = 0;
	for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
	{
		subcanes[i] = NULL;
		subcaneLocations[i] = make_vector(0.0f, 0.0f, 0.0f);
	}

	color.r = color.g = color.b = color.a = 1.0;
}

// Returns the number of nodes in the cane's DAG
int Cane :: leafNodes()
{
	if (this->subcaneCount == 0)
	{
		return 1;
	}
	else
	{
		int total = 0;
		for (int i = 0; i < this->subcaneCount; ++i)
			total += this->subcanes[i]->leafNodes();
		return total;
	}
}

// Copies the information in a cane object into
// the destination cane object
void Cane :: shallowCopy(Cane* dest)
{
	int i;

	dest->type = this->type;
	for (i = 0; i < MAX_AMT_TYPES; ++i)
	{
		dest->amts[i] = this->amts[i];
	}

	dest->subcaneCount = this->subcaneCount;
	for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
	{
		if (this->subcanes[i] != NULL)
		{
			dest->subcanes[i] = this->subcanes[i]->deepCopy();
			dest->subcaneLocations[i].x = this->subcaneLocations[i].x;
			dest->subcaneLocations[i].y = this->subcaneLocations[i].y;
		}
		else
			dest->subcanes[i] = NULL;
	}
	dest->color = this->color;
}

void Cane :: pullLinear(float twistFactor, float stretchFactor)
{
	if (this->type != PULL_CANETYPE)
	{
		Cane* copy = new Cane(UNASSIGNED_CANETYPE);
		this->shallowCopy(copy);
		this->reset();
		this->type = PULL_CANETYPE;
		this->subcaneCount = 1;
		this->subcanes[0] = copy;
		this->amts[0] = 0.0; // twist amount
		this->amts[1] = 1.0; // stretch amount
	}
	this->amts[0] += twistFactor;
	this->amts[0] += stretchFactor;
}

void Cane :: pullIntuitive(float twistFactor, float stretchFactor)
{
	// The amount twist and stretch are changed are functions
	// of the amount already present. The exact function is
	// determined by feel/playing with the tool.
	if (this->type != PULL_CANETYPE)
	{
		Cane* copy = new Cane(UNASSIGNED_CANETYPE);
		this->shallowCopy(copy);
		this->reset();
		this->type = PULL_CANETYPE;
		this->subcaneCount = 1;
		this->subcanes[0] = copy;
		this->amts[0] = 0.0; // twist amount
		this->amts[1] = 1.0; // stretch amount
	}
	if (this->amts[0] < 0.0)
	{
		if (this->amts[0] > -1.0)
			this->amts[0] -= 8 * -twistFactor;
		else
			this->amts[0] *= (1.0 + -twistFactor);
	}
	else
	{
		if (this->amts[0] < 1.0)
			this->amts[0] += 8 * twistFactor;
		else
			this->amts[0] *= (1.0 + twistFactor);
	}
	this->amts[1] *= (1.0 + stretchFactor);
}

void Cane :: pullIntuitive(int subcane, float twistFactor, float stretchFactor)
{
	// The amount twist and stretch are changed are functions
	// of the amount already present. The exact function is
	// determined by feel/playing with the tool.
	Cane* workingCane = this->subcanes[subcane];
	if (workingCane->type != PULL_CANETYPE)
	{
		Cane* copy = new Cane(UNASSIGNED_CANETYPE);
		workingCane->shallowCopy(copy);
		workingCane->reset();
		workingCane->type = PULL_CANETYPE;
		workingCane->subcaneCount = 1;
		workingCane->subcanes[0] = copy;
		workingCane->amts[0] = 0.0; // twist amount
		workingCane->amts[1] = 1.0; // stretch amount
	}
	if (workingCane->amts[0] < 0.0)
	{
		if (workingCane->amts[0] > -1.0)
			workingCane->amts[0] -= 8 * -twistFactor;
		else
			workingCane->amts[0] *= (1.0 + -twistFactor);
	}
	else
	{
		if (workingCane->amts[0] < 1.0)
			workingCane->amts[0] += 8 * twistFactor;
		else
			workingCane->amts[0] *= (1.0 + twistFactor);
	}
	workingCane->amts[1] *= (1.0 + stretchFactor);
}

/*
Cane::flatten() creates a new root node that deforms the cane
from a circular shape to an approximation of a rectangle.
The `rectangle_ratio' specifies the relative dimensions of
the rectangle the cane will be flattened into. A ratio of
1.0 is square, 2.0 is a rectangle twice as wide as it is tall, etc.
`rectangle_theta' specifies the orientation of the x-axis of the
rectangle relative to the global cane x-axis. `flatness' specifies
how closely the cane is squished into the goal rectangle. A
flatness of 0 means the cane remains circular, while a ratio of 1 means
the cane is deformed into a perfect rectangle.
*/
void Cane :: flatten(float rectangle_ratio, float rectangle_theta, float flatness)
{
	if (this->type != FLATTEN_CANETYPE || this->amts[1] != rectangle_theta)
	{
		Cane* copy = new Cane(UNASSIGNED_CANETYPE);
		this->shallowCopy(copy);
		this->reset();
		this->type = FLATTEN_CANETYPE;
		this->subcaneCount = 1;
		this->subcanes[0] = copy;
		this->amts[0] = 1.0; // rectangle_ratio
		this->amts[1] = rectangle_theta;
		this->amts[2] = 0.0; // flatness
	}
	this->amts[0] *= (1.0 + rectangle_ratio);
	this->amts[2] += flatness;
	this->amts[2] = MIN(1.0, MAX(0.0, amts[2]));
}

void Cane :: flatten(int subcane, float rectangle_ratio, float rectangle_theta, float flatness)
{
	Cane* workingCane = this->subcanes[subcane];
	if (workingCane->type != FLATTEN_CANETYPE || workingCane->amts[1] != rectangle_theta)
	{
		Cane* copy = new Cane(UNASSIGNED_CANETYPE);
		workingCane->shallowCopy(copy);
		workingCane->reset();
		workingCane->type = FLATTEN_CANETYPE;
		workingCane->subcaneCount = 1;
		workingCane->subcanes[0] = copy;
		workingCane->amts[0] = 1.0; // rectangle_ratio
		workingCane->amts[1] = rectangle_theta;
		workingCane->amts[2] = 0.0; // flatness
	}
	workingCane->amts[0] *= (1.0 + rectangle_ratio);
	workingCane->amts[2] += flatness;
	workingCane->amts[2] = MIN(1.0, MAX(0.0, workingCane->amts[2]));
}

void Cane :: createBundle()
{
	Cane* copy = new Cane(UNASSIGNED_CANETYPE);
	this->shallowCopy(copy);
	this->reset();
	this->type = BUNDLE_CANETYPE;
	this->subcaneCount = 1;
	this->subcanes[0] = copy;
	//
	//Cane* c = new Cane(BASE_CIRCLE_CANETYPE);
	//c->color.r = 1.0;
	//c->color.g = 1.0;
	//c->color.b = 1.0;
	//c->color.a = 0.95;
	//this->amts[0] = 0.0;
	//this->amts[1] = 10.0;

	//this->subcanes[1] = c;
}

void Cane :: moveCane(int subcane, float delta_x, float delta_y)
{
	subcaneLocations[subcane].x += delta_x;
	subcaneLocations[subcane].y += delta_y;
}

void Cane :: moveCane(int subcane, float delta_z)
{
	subcaneLocations[subcane].z += delta_z;
}

void Cane :: add(Cane* addl)
{
	if (this->type != BUNDLE_CANETYPE)
		createBundle();

	// Add the new cane to the bundle
	subcanes[subcaneCount] = addl;
	subcaneLocations[subcaneCount].x = 0;
	subcaneLocations[subcaneCount].y = 0;
	subcaneCount += 1;
}

void Cane :: deleteCane(int subcane)
{
	if (subcane<0 || subcane>=subcaneCount)
		return;
	subcaneCount--;
	for (int i=subcane;i<subcaneCount;i++)
	{
		subcanes[i]=subcanes[i+1];
		subcaneLocations[i]=subcaneLocations[i+1];
	}
}

Cane* Cane :: deepCopy()
{
	int i;
	Cane* copy;

	copy = new Cane(this->type);

	for (i = 0; i < MAX_AMT_TYPES; ++i)
	{
		copy->amts[i] = this->amts[i];
	}

	copy->subcaneCount = this->subcaneCount;
	for (i = 0; i < MAX_SUBCANE_COUNT; ++i)
	{
		if (this->subcanes[i] != NULL)
		{
			copy->subcanes[i] = this->subcanes[i]->deepCopy();
			copy->subcaneLocations[i].x = this->subcaneLocations[i].x;
			copy->subcaneLocations[i].y = this->subcaneLocations[i].y;
		}
		else
			copy->subcanes[i] = NULL;
	}
	copy->color = this->color;

	return copy;
}

void Cane :: setColor(Color color)
{
	this->color = color;
}

bool isNaN(float n)
{
	return n!=n;
}

float changeIfNaN(float n,float base)
{
	if (isNaN(n))
		return base;
	else
		return n;
}

float zeroIfNaN(float n)
{
	return changeIfNaN(n,0);
}

std::string Cane :: yamlRepresentation()
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Type";
	out << YAML::Value << type;

	out << YAML::Key << "Amounts";
	out << YAML::Value << YAML::BeginSeq;
	for (unsigned int j=0;j<sizeof(amts);j++){
		out << zeroIfNaN(amts[j]);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "SubCaneCount";
	out << YAML::Value << subcaneCount;

	out << YAML::Key << "SubCaneLocations";
	out << YAML::Value << YAML::BeginSeq;
	for (int j=0;j<subcaneCount;j++)
	{
		Point loc = subcaneLocations[j];
		out << YAML::BeginSeq;

		out << zeroIfNaN(loc.x) << zeroIfNaN(loc.y) << zeroIfNaN(loc.z);
		out << YAML::EndSeq;
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Color";
	out << YAML::Value << YAML::BeginSeq;
	out << color.r << color.g << color.b << color.a;
	out << YAML::EndSeq;

	out << YAML::Key << "SubCanes";
	out << YAML::Value << YAML::BeginSeq;
	for (int j=0;j<subcaneCount;j++){
		Cane* cane = subcanes[j];

		out << YAML::Literal << cane->yamlRepresentation();
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	return out.c_str();
}
