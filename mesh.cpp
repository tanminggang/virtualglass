
#include "mesh.h"


Point rotate(Point pre, float theta)
{
        Point post;

        post.x = cos(theta) * pre.x - sin(theta) * pre.y;
        post.y = sin(theta) * pre.x + cos(theta) * pre.y;

        return post;
}

void applyFlattenTransform(Geometry* geometry, Cane* transformNode)
{
        for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
        {
                applyFlattenTransform(&(geometry->vertices[v]), transformNode);
        }
}

void applyFlattenTransform(Vertex* v, Cane* transformNode)
{
        float rectangleRatio = transformNode->amts[0];
	float rectangleTheta = transformNode->amts[1];
	float flatness = transformNode->amts[2];

        Point vert;
        vert.x = v->position.x;
        vert.y = v->position.y;
        Point vertRectCoords = rotate(vert, -rectangleTheta);

        float radius = length(vertRectCoords.xy);
        float theta = atan2(vertRectCoords.y, vertRectCoords.x);
        if (theta < 0)
                theta += 2*PI;
        float arc_length = radius * theta;

        /*
        The vertex lies on a circle that is mapped to a rectangle
        with equal perimeter. The rectangleRatio is width/height.
        */
        float rectY = PI * radius / (1.0 + rectangleRatio);
        float rectX = rectY * rectangleRatio;

        // Use half width/height
        rectY /= 2.0;
        rectX /= 2.0;

        Point goalRectangle;
        if (arc_length < rectY)
	{
                goalRectangle.x = rectX;
                goalRectangle.y = arc_length;
	}
        else if ((arc_length - rectY) < 2 * rectX)
	{
                goalRectangle.x = rectX - (arc_length - rectY);
                goalRectangle.y = rectY;
        }
        else if ((arc_length - rectY - 2 * rectX) < 2 * rectY)
        {
                goalRectangle.x = -rectX;
                goalRectangle.y = rectY - (arc_length - rectY - 2 * rectX);
        }
        else if ((arc_length - rectY - 2 * rectX - 2 * rectY) < 2 * rectX)
        {
                goalRectangle.x = -rectX + (arc_length - rectY - 2 * rectX - 2 * rectY);
                goalRectangle.y = -rectY;
        }
        else
        {
                goalRectangle.x = rectX;
                goalRectangle.y = -rectY + (arc_length - rectY - 2 * rectX - 2 * rectY - 2 * rectX);
        }

        // Transform back to normal coordinates from rectangle coordinates.
        goalRectangle = rotate(goalRectangle, rectangleTheta);
        v->position.x = flatness * goalRectangle.x + vert.x * (1 - flatness);
        v->position.y = flatness * goalRectangle.y + vert.y * (1 - flatness);
}

void applyMoveTransform(Geometry* geometry, Cane* parentNode, int subcane)
{
        // Find group for subcane
        Group* subcaneGroup;
        for (uint32_t g = 0; g < geometry->groups.size(); ++g)
                if (geometry->groups[g].tag == (uint32_t) subcane)
                {
                        subcaneGroup = &(geometry->groups[g]);
                        break;
                }

        // Apply transformation to only these vertices
        for (uint32_t v = subcaneGroup->vertex_begin; v < subcaneGroup->vertex_begin + subcaneGroup->vertex_size; ++v)
        {
                applyMoveTransform(&(geometry->vertices[v]), parentNode, subcane);
        }
}

void applyMoveTransform(Vertex* v, Cane* parentNode, int subcane)
{
        Point locationInParent = parentNode->subcaneLocations[subcane];

        // Z location
        float preTheta = atan2(v->position.y, v->position.x);
        float postTheta = preTheta + locationInParent.z;
        float r = length(v->position.xy);
        v->position.x = r * cos(postTheta);
        v->position.y = r * sin(postTheta);

        // XY location
        v->position.x += locationInParent.x;
        v->position.y += locationInParent.y;
}

void applyPullTransform(Geometry* geometry, Cane* transformNode)
{
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyPullTransform(&(geometry->vertices[v]), transformNode);
	}
}


/*
In applyPullTransform() and unapplyPullTransform(), the
twist component is applied by computing the current location
of the vertex in polar coordinates, the amount that the angle
this location changes according to the transformation and
the location of the point along the z axis, and the resulting
location of the point in polar coordinates.

These angles are called preTheta, transformTheta, and postTheta,
respectively. They are called the same thing in both apply*()
and unapply*().
*/
void applyPullTransform(Vertex* v, Cane* transformNode)
{
	float twist = transformNode->amts[0];
	float stretch = transformNode->amts[1];

	// Apply twist first
	float preTheta = atan2(v->position.y, v->position.x);
	float r = length(v->position.xy);
	float transformTheta = stretch * twist * v->position.z;
	float postTheta = preTheta + transformTheta;
	v->position.x = r * cos(postTheta);
	v->position.y = r * sin(postTheta);

	// Then apply stretch
	v->position.x /= sqrt(stretch);
	v->position.y /= sqrt(stretch);
	v->position.z *= stretch;
}

/*
This function applies a sequence of transformations to a 3D point.
This is used during the meshing process to determine the location of
a vertex after it has been moved via the transformations described
by its ancestors in the cane DAG.
*/
Vertex applyTransforms(Vertex v, Cane** ancestors, int ancestorCount)
{
	/*
        The transformations are applied back to front to match how they
        are loaded into the array. Because the transform array is created by
        a depth-first traversal of the cane DAG, transforms lower in the DAG
        (i.e. closer to the leaves) are added later. However, they
        represent the first operations done on the cane, so need to be applied
        first.
        */
	for (int i = ancestorCount - 1; i >= 0; --i)
	{
		/*
                Each cane node has a type and an amount.
                Depending upon the type, the amount fields
                take on different meanings. For instance, a twist transform
                uses amts[0] to mean the magnitude of the twist.
                just a single real-valued parameter).
                The BUNDLE_CANETYPE is an exception, in that it simply uses
                the location of the subcane to determine how to move the points.
                */
                int subcaneIndex=-1;
		switch (ancestors[i]->type)
		{
		case BUNDLE_CANETYPE:
			// Find where subcane lives and apply translation
			// to move it to this location
			for (int j = 0; j < ancestors[i]->subcaneCount; ++j)
			{
				if (ancestors[i]->subcanes[j] == ancestors[i+1])
				{
					subcaneIndex = j;
					break;
				}
			}
                        applyMoveTransform(&v, ancestors[i], subcaneIndex);
			break;
			// amts[0] is twist, amts[1] is stretch
		case PULL_CANETYPE:
			applyPullTransform(&v, ancestors[i]);
			//TODO: normal transform
			break;
			// amts[0] describes the width-to-height ratio of the goal rectangle
			// amts[1] describes the orientation w.r.t global XY
			// amts[2] describes how close of an approximation to the rectangle is achieved
		case FLATTEN_CANETYPE:
			applyFlattenTransform(&v, ancestors[i]);
			break;
		default: // BASE_CIRCLE_CANETYPE
			break;
		}
	}
	return v;
}

/*
This function is used in the process of reparameterizing the mesh after
a stretch, to avoid losing vertices when the cane is cut down to just
the z-region between 0 and 1.
*/
float computeTotalStretch(Cane** ancestors, int ancestorCount)
{
	float totalStretch;
	int i;

	totalStretch = 1;
	for (i = 0; i < ancestorCount; ++i)
	{
		if (ancestors[i]->type == PULL_CANETYPE)
			totalStretch *= ancestors[i]->amts[1];
	}

	return totalStretch;
}

/*
meshCircularBaseCane() creates a mesh for a radius 1, length 1 cylindrical piece of cane,
and applies a sequences of transforms (coming from a depth-first traversal of the cane ending
with this leaf base cane). The triangles are added to the end of the array passed in.

The resolution refers to the dual resolution modes used by the GUI, and the actual number of
triangles for these resolutions are set in constants.h
*/
float meshCircularBaseCane(Geometry *geometry, Cane** ancestors, int ancestorCount,
						   int resolution, Cane *group_cane, uint32_t group_tag, float radius, bool computeRadius)
{
	unsigned int angularResolution, axialResolution;
	float total_stretch, transformedRadius;

	switch (resolution)
	{
	case LOW_RESOLUTION:
		angularResolution = LOW_ANGULAR_RESOLUTION;
		axialResolution = LOW_AXIAL_RESOLUTION;
		break;
	case HIGH_RESOLUTION:
		angularResolution = HIGH_ANGULAR_RESOLUTION;
		axialResolution = HIGH_AXIAL_RESOLUTION;
		break;
	default:
		exit(1);
	}

	total_stretch = computeTotalStretch(ancestors, ancestorCount);
	float stretchResParam = 1.0 / (1.0 + total_stretch / 20.0);
	angularResolution = (int) (angularResolution * stretchResParam + 8 * (1 - stretchResParam)); // hack adaptive meshing

	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	/*
        Draw the walls of the cylinder. Note that the z location is
        adjusted by the total stretch experienced by the cane so that
        the z values range between 0 and 1.
        */
	//Generate verts:
	for (unsigned int i = 0; i < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < angularResolution; ++j)
		{
			Point p;
			Point n;

			p.x = radius * cos(2 * PI * ((float) j) / angularResolution);
			p.y = radius * sin(2 * PI * ((float) j) / angularResolution);
			p.z = ((float) i) / ((axialResolution-1) * total_stretch);
			n.x = p.x;
			n.y = p.y;
			n.z = 0.0f;
			geometry->vertices.push_back(Vertex(p,n));
		}
	}
	//Generate triangles linking them:
	for (unsigned int i = 0; i + 1 < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < angularResolution; ++j)
		{
			uint32_t p1 = first_vert + i * angularResolution + j;
			uint32_t p2 = first_vert + (i+1) * angularResolution + j;
			uint32_t p3 = first_vert + i * angularResolution + (j+1) % angularResolution;
			uint32_t p4 = first_vert + (i+1) * angularResolution + (j+1) % angularResolution;
			// Four points that define a (non-flat) quad are used
			// to create two triangles.
			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
	}
	assert(geometry->valid());

	/*
        Draw the cylinder bottom, then top.
        The mesh uses a set of n-2 triangles with a common vertex
        to draw a regular n-gon.
        */
	for (int side = 0; side <= 1; ++side) {
		float z = (side?1.0:0.0);
		float nz = (side?1.0:-1.0);
		uint32_t base = geometry->vertices.size();
		for (unsigned int j = 0; j < angularResolution; ++j)
		{
			Point p;
			p.x = radius * cos(2 * PI * ((float) j) / angularResolution);
			p.y = radius * sin(2 * PI * ((float) j) / angularResolution);
			p.z = z / total_stretch;
			Point n;
			n.x = 0.0; n.y = 0.0; n.z = nz;
			geometry->vertices.push_back(Vertex(p, n));
		}
		if (side)
		{
			for (unsigned int j = 1; j + 1 < angularResolution; ++j)
			{
				geometry->triangles.push_back(Triangle(base, base + j, base + j + 1));
			}
		}
		else
		{
			for (unsigned int j = 1; j + 1 < angularResolution; ++j)
			{
				geometry->triangles.push_back(Triangle(base, base + j + 1, base + j));
			}
		}
	}
	assert(geometry->valid());

	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorCount);
	}
	geometry->compute_normals_from_triangles();
	geometry->groups.push_back(Group(first_triangle,
                geometry->triangles.size() - first_triangle, first_vert, geometry->vertices.size() - first_vert, group_cane, group_tag));

	if (!computeRadius)
		return 0.0;

	// Compute radius of resulting cane
	transformedRadius = 0;
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		transformedRadius = MAX(transformedRadius, geometry->vertices[v].position.x * geometry->vertices[v].position.x
								+ geometry->vertices[v].position.y * geometry->vertices[v].position.y);
	}
	transformedRadius = sqrt(transformedRadius);

	return transformedRadius;
}

float mesh2DCircularBaseCane(Geometry *geometry, Cane** ancestors, int ancestorCount, int resolution,
							 Cane *group_cane, uint32_t group_tag, float radius, bool computeRadius)
{

	unsigned int angularResolution, axialResolution;
	//float total_stretch;
	float transformedRadius;

	switch (resolution)
	{
	case LOW_RESOLUTION:
		angularResolution = LOW_ANGULAR_RESOLUTION;
		axialResolution = LOW_AXIAL_RESOLUTION;
		break;
	case HIGH_RESOLUTION:
		angularResolution = HIGH_ANGULAR_RESOLUTION;
		axialResolution = HIGH_AXIAL_RESOLUTION;
		break;
	default:
		exit(1);
	}

	//need to know first vertex position so we can transform 'em all later
	uint32_t first_vert = geometry->vertices.size();
	//need to remember the first triangle so we can tag it later
	uint32_t first_triangle = geometry->triangles.size();

	/*
        Draw the cylinder top only.
        The mesh uses a set of n-2 triangles with a common vertex
        to draw a regular n-gon.
        */
	uint32_t base = geometry->vertices.size();
	for (unsigned int j = 0; j < angularResolution; ++j)
	{
		Point p;
		p.x = radius * cos(2 * PI * ((float) j) / angularResolution);
		p.y = radius * sin(2 * PI * ((float) j) / angularResolution);
		p.z = 0.0;
		Point n;
		n.x = 0.0; n.y = 0.0; n.z = 1.0;
		geometry->vertices.push_back(Vertex(p, n));
	}
	for (unsigned int j = 1; j + 1 < angularResolution; ++j)
	{
		geometry->triangles.push_back(Triangle(base, base + j, base + j + 1));
	}

	assert(geometry->valid());

	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		geometry->vertices[v] = applyTransforms(geometry->vertices[v], ancestors, ancestorCount);
	}
	geometry->groups.push_back(Group(first_triangle,
                geometry->triangles.size() - first_triangle, first_vert, geometry->vertices.size() - first_vert, group_cane, group_tag));

	if (!computeRadius)
		return 0.0;

	// Compute radius of resulting cane
	transformedRadius = 0;
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		transformedRadius = MAX(transformedRadius, geometry->vertices[v].position.x * geometry->vertices[v].position.x + geometry->vertices[v].position.y * geometry->vertices[v].position.y);
	}
	transformedRadius = sqrt(transformedRadius);

	return transformedRadius;
}


/*
generateMesh() is the top-level function for turning a cane into
a geometry that can be rendered. As generateMesh() is called recursively,
the transforms array is filled with with the transformations encountered at each node. When a
leaf is reached, these transformations are used to generate a complete mesh
for the leaf node.
*/
float generateMesh(Cane* c, Geometry *geometry, Cane** ancestors, int* ancestorCount,
        int resolution, bool casing, bool computeRadius, int groupIndex)
{
	int i, passCasing, passGroupIndex;
	float radius=0.0;

	if (c == NULL)
		return 0.0;

	// Make recursive calls depending on the type of the current node
	ancestors[*ancestorCount] = c;
	*ancestorCount += 1;
	if (c->type == BASE_CIRCLE_CANETYPE)
	{
		radius = meshCircularBaseCane(geometry, ancestors, *ancestorCount,
                        resolution, c, groupIndex, 1.0, computeRadius);
	}
	else
	{
		passCasing = (casing && c->type != CASING_CANETYPE);
		for (i = 0; i < c->subcaneCount; ++i)
		{
			if (groupIndex == -1)
				passGroupIndex = i;
			else
				passGroupIndex = groupIndex;

			radius = MAX(generateMesh(c->subcanes[i], geometry, ancestors, ancestorCount,
									  resolution, passCasing, computeRadius, passGroupIndex), radius);
		}
		if (casing && c->type == CASING_CANETYPE)
		{
			radius = MAX(meshCircularBaseCane(geometry, ancestors, *ancestorCount,
											  resolution, c, groupIndex, c->amts[0], computeRadius), radius);
		}
	}
	*ancestorCount -= 1;

	return radius;
}

float generate2DMesh(Cane* c, Geometry *geometry, Cane** ancestors, int* ancestorCount,
					 int resolution, bool casing, bool computeRadius, int groupIndex)
{
	int i, passCasing, passGroupIndex;
	float radius=0.0;

	if (c == NULL)
		return 0.0;

	// Make recursive calls depending on the type of the current node
	ancestors[*ancestorCount] = c;
	*ancestorCount += 1;
	if (c->type == BASE_CIRCLE_CANETYPE)
	{
		radius = mesh2DCircularBaseCane(geometry, ancestors, *ancestorCount,
										resolution, c, groupIndex, 1.0, computeRadius);
	}
	else
	{
		passCasing = (casing && c->type != CASING_CANETYPE);
		for (i = 0; i < c->subcaneCount; ++i)
		{
			if (groupIndex == -1)
				passGroupIndex = i;
			else
				passGroupIndex = groupIndex;

			radius = MAX(generate2DMesh(c->subcanes[i], geometry, ancestors, ancestorCount,
										resolution, passCasing, computeRadius, passGroupIndex), radius);
		}
		if (casing && c->type == CASING_CANETYPE)
		{
			radius = MAX(mesh2DCircularBaseCane(geometry, ancestors, *ancestorCount,
												resolution, c, groupIndex, c->amts[0], computeRadius), radius);
		}
	}
	*ancestorCount -= 1;

	return radius;
}


