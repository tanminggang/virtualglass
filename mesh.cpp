
#include <vector>
#include <utility>
#include "constants.h"
#include "glasscolor.h"
#include "pullplan.h"
#include "pickupplan.h"
#include "piece.h"
#include "subpulltemplate.h"
#include "mesh.h"
#include "spline.h"

using namespace MeshInternal;

using std::vector;
using std::pair;

void generateMesh(Piece* piece, Geometry* pieceGeometry, Geometry* pickupGeometry, unsigned int quality)
{
	if (piece == NULL || pieceGeometry == NULL)
		return;

	if (pickupGeometry == NULL)
	{
		// just do everything in the piece geometry
		pieceGeometry->clear();
		generateMesh(piece->pickup, pieceGeometry, quality);
		casePickup(pieceGeometry, piece);
		applyPieceTransform(pieceGeometry, piece);
		pieceGeometry->compute_normals_from_triangles();
	}
	else
	{
		// compute pickup geometry and copy it into piece geometry
		// to save all the work of computing the same thing again

		// clear geometry
		pickupGeometry->clear();
		pieceGeometry->clear();
		// compute pickup geometry
		generateMesh(piece->pickup, pickupGeometry, quality);
		pickupGeometry->compute_normals_from_triangles();
		// copy into piece geometry and apply piece transform there
		pieceGeometry->vertices = pickupGeometry->vertices;
		pieceGeometry->triangles = pickupGeometry->triangles;
		pieceGeometry->groups = pickupGeometry->groups;
		casePickup(pieceGeometry, piece);
		applyPieceTransform(pieceGeometry, piece);
		pieceGeometry->compute_normals_from_triangles();
	}
}

void generateMesh(PullPlan* plan, Geometry* geometry, unsigned int quality)
{
	geometry->clear();
	vector<ancestor> ancestors;
	recurseMesh(plan, geometry, ancestors, 2.0, quality, true);

	// Make skinnier to more closely mimic the canes found in pickups
	for (uint32_t v = 0; v < geometry->vertices.size(); ++v)
	{
		applyResizeTransform(geometry->vertices[v], 0.5);
	}
	geometry->compute_normals_from_triangles();
}

void generateMesh(GlassColor* gc, Geometry* geometry, unsigned int quality)
{
	PullPlan dummyPlan(PullTemplate::BASE_CIRCLE);
	dummyPlan.setOutermostCasingColor(gc);

	geometry->clear();
	vector<ancestor> ancestors;
	recurseMesh(&dummyPlan, geometry, ancestors, 2.0, quality, true);
	geometry->compute_normals_from_triangles();
}


namespace MeshInternal
{

void applySubplanTransform(Geometry& geometry, ancestor a)
{
	for (uint32_t v = 0; v < geometry.vertices.size(); ++v)
		applySubplanTransform(geometry.vertices[v], a);
}

void applyResizeTransform(Vertex& v, float scale)
{
	// Adjust diameter
	v.position.x *= scale; 
	v.position.y *= scale; 
}

// Does move, resize, and twist
void applySubplanTransform(Vertex& v, ancestor a)
{
	SubpullTemplate& subTemp = a.parent->subs[a.child];
	Point2D locationInParent = subTemp.location;
	float diameter = subTemp.diameter;

	// Adjust diameter
	v.position.x *= diameter / 2.0; 
	v.position.y *= diameter / 2.0; 

	// Adjust to location in parent
	v.position.x += locationInParent.x;
	v.position.y += locationInParent.y;

	// Adjust location in parent for twist
	float r = sqrt(locationInParent.x * locationInParent.x + locationInParent.y * locationInParent.y);
	float preTheta = atan2(locationInParent.y, locationInParent.x); 
	float postTheta = a.parent->twist * TWO_PI * v.position.z / 10.0 + preTheta;
	v.position.x += (r * cos(postTheta) - r * cos(preTheta));
	v.position.y += (r * sin(postTheta) - r * sin(preTheta));
}

void applyPickupTransform(Vertex& v, SubpickupTemplate& spt)
{
	// Shrink width to correct width
	v.position.x *= spt.width * 2.5; 
	v.position.y *= spt.width * 2.5; 

	// Change orientation if needed
	float tmp;
	switch (spt.orientation)
	{
		case HORIZONTAL_PICKUP_CANE_ORIENTATION:
			tmp = v.position.x;
			v.position.x = v.position.z;
			v.position.z = -tmp;
			break;
		case VERTICAL_PICKUP_CANE_ORIENTATION:
			break;
		case MURRINE_PICKUP_CANE_ORIENTATION:
			tmp = v.position.y;
			v.position.y = v.position.z;
			v.position.z = -tmp;
			break;
	}

	// Offset by location
	v.position.x = v.position.x + spt.location.x * 5.0;
	v.position.z = v.position.z + spt.location.y * 5.0;
	v.position.y = v.position.y + spt.location.z * 5.0;
}

void applyPieceTransform(Vertex& v, float twist, Spline& spline)
{
	float diskTheta = PI * v.position.x / 5.0 + twist * TWO_PI * v.position.z / 10.0;
	float caneLoc = v.position.y;
	float t = (v.position.z - -5.0) / 10.0;

	Point2D p1 = spline.get(t);
	Point2D p2 = spline.get(t + 0.01);
	// remove artifacts at piece base due to bevel-like angling of pickup slab
	float fix = 0.05;
	if (t < fix)
		p2.y = 1.0 / fix * (fix - t) * p1.y + 1.0 / fix * t * p2.y;

	float delta_theta = atan2(p2.y - p1.y, p2.x - p1.x);
	delta_theta -= PI/2;

	v.position.x = (p1.x + cos(delta_theta) * -caneLoc) * cos(diskTheta);
	v.position.y = (p1.x + cos(delta_theta) * -caneLoc) * sin(diskTheta);
	v.position.z = p1.y + -caneLoc * sin(delta_theta);
}

void casePickup(Geometry* geometry, Piece* piece)
{
	// allow canes to be invisible now, since casing will be added around them
	// and clear "visibility" is an index of refraction thing between air and glass
	for (unsigned int i = 0; i < geometry->groups.size(); ++i)
		geometry->groups[i].ensureVisible = false;

	// base thickness of casing off of representative (first) cane in pickup
	float thickness;
	if (piece->pickup->subs[0].orientation == MURRINE_PICKUP_CANE_ORIENTATION)
		thickness = piece->pickup->subs[0].length*2.5;	
	else
		thickness = piece->pickup->subs[0].width*2.5;

	meshPickupCasingSlab(geometry, piece->pickup->casingGlassColor->getColor(), 0.0, thickness);
	//meshPickupCasingSlab(geometry, pickup->underlayGlassColor->getColor(), thickness + 0.1, 0.05);
	//meshPickupCasingSlab(geometry, pickup->overlayGlassColor->getColor(), -(thickness + 0.1), 0.05);

}

void applyPieceTransform(Geometry* geometry, Piece* piece)
{
	for (uint32_t i = 0; i < geometry->vertices.size(); ++i)
		applyPieceTransform(geometry->vertices[i], piece->twist, piece->spline);
}

void applySubplanTransforms(Vertex& v, vector<ancestor>& ancestors)
{
	for (unsigned int i = ancestors.size() - 1; i < ancestors.size(); --i)
		applySubplanTransform(v, ancestors[i]);
}

void meshPickupCasingSlab(Geometry* geometry, Color color, float y, float thickness)
{
	unsigned int slabResolution = 100;

	uint32_t first_vert = geometry->vertices.size();
	uint32_t first_triangle = geometry->triangles.size();

	Vector2f p;
	vector< Vector2f > points;

	// generate a basis set of points		
	for (unsigned int i = 0; i < slabResolution; ++i)
	{
		p.x = 4.999 - 9.998* i / (slabResolution-1);
		p.y = y + thickness + 0.01;
		points.push_back(p);
	}
	for (unsigned int i = 0; i < slabResolution; ++i)
	{
		p.x = -4.999 + 9.998 * i / (slabResolution-1);
		p.y = y + -thickness - 0.01;
		points.push_back(p);
	}

	// generate pull vertex set for slabs
	unsigned int base = geometry->vertices.size();
	for (unsigned int i = 0; i < slabResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point3D p;
			p.xy = points[j];
			if (i == 0)
				p.z = -5.001;
			else
				p.z = -5.001 + 10.002 * i / (slabResolution-1);
			Point3D n;
			//This is a terrible normal estimate, but I guess it gets re-estimated anyway.
			n.x = p.x;
			n.y = p.y;
			n.z = 0.0f;
			geometry->vertices.push_back(Vertex(p,n));
		}
	}
	// mesh slabs
	for (unsigned int i = 0; i + 1 < slabResolution; ++i)
	{
		// outer slab
		for (unsigned int j = slabResolution; j < 2 * slabResolution - 1; ++j)
		{
			uint32_t p1 = base + i * 2 * slabResolution + j;
			uint32_t p2 = base + (i+1) * 2 * slabResolution + j;
			uint32_t p3 = base + i * 2 * slabResolution + j+1;
			uint32_t p4 = base + (i+1) * 2 * slabResolution + j+1;

			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
		// inner slab
		for (unsigned int j = 0; j < slabResolution-1; ++j)
		{
			uint32_t p1 = base + i * 2 * slabResolution + j;
			uint32_t p2 = base + (i+1) * 2 * slabResolution + j;
			uint32_t p3 = base + i * 2 * slabResolution + j+1;
			uint32_t p4 = base + (i+1) * 2 * slabResolution + j+1;

			geometry->triangles.push_back(Triangle(p2, p1, p4));
			geometry->triangles.push_back(Triangle(p1, p3, p4));
		}
	}

	// generate vertex set for bottom strip
	base = geometry->vertices.size();
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point3D p;
		p.xy = points[j];
		p.z = -4.99;

		Point3D n;
		n.x = 0.0; n.y = 0.0; n.z = -1.0;
		geometry->vertices.push_back(Vertex(p, n));
	}
	// mesh bottom strip connecting the two slabs
	for (unsigned int j = 0; j < slabResolution; ++j)
	{
		uint32_t p1 = base + j;
		uint32_t p2 = base + j+1;
		uint32_t p3 = base + 2 * slabResolution-1 - j;
		uint32_t p4 = base + 2 * slabResolution-1 - (j+1);

		geometry->triangles.push_back(Triangle(p2, p1, p4));
		geometry->triangles.push_back(Triangle(p1, p3, p4));
	}

	// generate vertex set for top strip connecting the two slabs
	base = geometry->vertices.size();
	for (unsigned int j = 0; j < points.size(); ++j)
	{
		Point3D p;
		p.xy = points[j];
		p.z = 4.99;

		Point3D n;
		n.x = 0.0; n.y = 0.0; n.z = 1.0;
		geometry->vertices.push_back(Vertex(p, n));
	}
	// mesh top strip connecting the two slabs	
	for (unsigned int j = 0; j < slabResolution-1; ++j)
	{
		uint32_t p1 = base + j;
		uint32_t p2 = base + j+1;
		uint32_t p3 = base + 2 * slabResolution-1 - j;
		uint32_t p4 = base + 2 * slabResolution-1 - (j+1);

		geometry->triangles.push_back(Triangle(p1, p2, p4));
		geometry->triangles.push_back(Triangle(p1, p4, p3));
	}
	
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color, true));
}

void getTemplatePoints(vector<Vector2f>* points, unsigned int angularResolution, 
	enum GeometricShape shape, float radius)
{
	Vector2f p;

	// force angularResolution to be the closest multiple of 4
	// so square mesh has consistent resolution along each side
	assert(angularResolution % 4 == 0);

	points->clear();
	// all shapes start at angle -PI/4 (i.e. x = 1.0, y = -1.0)
	// this consistency is necessary for meshBaseCasing() which stitches the tops
	// of them together	
	switch (shape)
	{
		case CIRCLE_SHAPE:
			for (unsigned int i = 0; i < angularResolution; ++i)
			{
				p.x = cos(TWO_PI * i / angularResolution - PI/4);
				p.y = sin(TWO_PI * i / angularResolution - PI/4);
				points->push_back(p);
			}
			break;
		case SQUARE_SHAPE:
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = 1.0;
				p.y = -1.0 + 8.0 * i / angularResolution;
				points->push_back(p);
			}
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = 1.0 - 8.0 * i / angularResolution;
				p.y = 1.0;
				points->push_back(p);
			}
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = -1.0;
				p.y = 1.0 - 8.0 * i / angularResolution;
				points->push_back(p);
			}
			for (unsigned int i = 0; i < angularResolution / 4; ++i)
			{
				p.x = -1.0 + 8.0 * i / angularResolution;
				p.y = -1.0;
				points->push_back(p);
			}
			break;
	}

	// scale for radius 
	for (unsigned int i = 0; i < points->size(); ++i) {
		(*points)[i].x *= radius;
		(*points)[i].y *= radius;
	}

} 

void meshCylinderWall(Geometry* geometry, enum GeometricShape shape, float length, float radius, 
	unsigned int angularResolution, unsigned int axialResolution, bool flipOrientation)
{
	vector< Vector2f > points;
	assert(angularResolution % 4 == 0);
	getTemplatePoints(&points, angularResolution, shape, radius);

	// Create wall vertices row by row
	unsigned int base = geometry->vertices.size();
	for (unsigned int i = 0; i < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			Point3D p;
			p.xy = points[j];
			if (i == 0)
				p.z = 0.0;
			else
				p.z = 5.0 * length * i / (axialResolution-1);
			Point3D n;
			//This is a terrible normal estimate, but I guess it gets re-estimated anyway.
			n.x = p.x;
			n.y = p.y;
			n.z = 0.0f;
			geometry->vertices.push_back(Vertex(p,n));
		}
	}
	//Generate triangles linking wall vertices in adjacent rows
	for (unsigned int i = 0; i + 1 < axialResolution; ++i)
	{
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			uint32_t p1 = base + i * points.size() + j;
			uint32_t p2 = base + (i+1) * points.size() + j;
			uint32_t p3 = base + i * points.size() + (j+1) % points.size();
			uint32_t p4 = base + (i+1) * points.size() + (j+1) % points.size();
			// Four points that define a (non-flat) quad are used
			// to create two triangles.
			if (flipOrientation)
			{	
				geometry->triangles.push_back(Triangle(p1, p2, p4));
				geometry->triangles.push_back(Triangle(p3, p1, p4));
			}
			else
			{
				geometry->triangles.push_back(Triangle(p2, p1, p4));
				geometry->triangles.push_back(Triangle(p1, p3, p4));
			}
		}
	}
} 

// goals for resolutions with respect to quality:
// quality ranges from 1 to 10: 1 is low, 10 is high
// 5 gives something decent with mild artifacts
// 1 gives something really ugly but low triangle count
// 10 gives something with no artifacts
unsigned int computeAngularResolution(float finalDiameter, unsigned int quality)
{
	// standard "full" cane diameter is 1.0 
	unsigned int r = MAX(finalDiameter*14 + quality/1.2, 4); 
	return (r / 4) * 4; // round down to nearest multiple of 4
}

unsigned int computeAxialResolution(float length, unsigned int quality)
{
	// standard "full" cane length is 2.0
	unsigned int r = MAX(length * quality * 5, 5);
	return r;
}

void meshBaseCasing(Geometry* geometry, vector<ancestor>& ancestors, Color color, 
	enum GeometricShape outerShape, enum GeometricShape innerShape, float length, float outerRadius, 
	float innerRadius, unsigned int quality, bool ensureVisible)
{
	unsigned int angularResolution = computeAngularResolution(totalShrink(ancestors), quality);
	unsigned int axialResolution = computeAxialResolution(length, quality);
	
	uint32_t first_vert = geometry->vertices.size();
	uint32_t first_triangle = geometry->triangles.size();

	// assuming meshCylinderWall vertices end with the top row  
	unsigned int outerPointsBottomStart = geometry->vertices.size();
	meshCylinderWall(geometry, outerShape, length, outerRadius, angularResolution, axialResolution);
	unsigned int outerPointsTopStart = geometry->vertices.size() - angularResolution;
	unsigned int innerPointsBottomStart = geometry->vertices.size();
	meshCylinderWall(geometry, innerShape, length, innerRadius, angularResolution, axialResolution, true);
	unsigned int innerPointsTopStart = geometry->vertices.size() - angularResolution;

	for (unsigned int i = 0; i < angularResolution; ++i)
	{
		uint32_t p1 = innerPointsTopStart + i;
		uint32_t p2 = innerPointsTopStart + (i+1) % angularResolution;
		uint32_t p3 = outerPointsTopStart + i;
		uint32_t p4 = outerPointsTopStart + (i+1) % angularResolution;
		geometry->triangles.push_back(Triangle(p4, p1, p3));
		geometry->triangles.push_back(Triangle(p2, p1, p4));

		p1 = innerPointsBottomStart + i;
		p2 = innerPointsBottomStart + (i+1) % angularResolution;
		p3 = outerPointsBottomStart + i;
		p4 = outerPointsBottomStart + (i+1) % angularResolution;
		geometry->triangles.push_back(Triangle(p1, p4, p3));
		geometry->triangles.push_back(Triangle(p1, p2, p4));
	}

	assert(geometry->valid());

	// Actually do the transformations on the basic canonical cylinder mesh
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		applySubplanTransforms(geometry->vertices[v], ancestors);
	}
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color, ensureVisible));
}

float totalShrink(vector<ancestor>& ancestors)
{
	float shrink = 1.0; // initial radius	

	for (unsigned int i = ancestors.size() - 1; i < ancestors.size(); --i)
	{
		shrink *= (ancestors[i].parent->subs[ancestors[i].child].diameter * 0.5); 
	}

	return shrink;
}


/*
The cane should have length between 0.0 and 1.0 and is scaled up by a factor of 5.
*/
void meshBaseCane(Geometry* geometry, vector<ancestor>& ancestors, 
	Color color, enum GeometricShape shape, float length, float radius, unsigned int quality, bool ensureVisible)
{
	unsigned int angularResolution = computeAngularResolution(totalShrink(ancestors), quality);
	unsigned int axialResolution = computeAxialResolution(length, quality);

	uint32_t first_vert = geometry->vertices.size();
	uint32_t first_triangle = geometry->triangles.size();

	meshCylinderWall(geometry, shape, length, radius, angularResolution, axialResolution);

	// now mesh top and bottom
	vector< Vector2f > points;
	getTemplatePoints(&points, angularResolution, shape, radius);

	// Build top and bottom triangles (two concentric rings) 
	vector< Vector3ui > layer1Tris;
	layer1Tris.resize(points.size());
	for (unsigned int i = 0; i < points.size(); ++i) {
		layer1Tris[i].c[0] = -1;
		layer1Tris[i].c[1] = i;
		layer1Tris[i].c[2] = (i+1) % points.size();
	}

	vector< Vector3ui > layer2Tris;
	layer2Tris.resize(2*points.size());
	for (unsigned int i = 0; i < points.size(); ++i) {
		layer2Tris[2*i].c[0] = i-points.size();
		layer2Tris[2*i].c[1] = i;
		layer2Tris[2*i].c[2] = (i+1) % points.size();
		layer2Tris[2*i+1].c[0] = i-points.size();
		layer2Tris[2*i+1].c[1] = (i+1) % points.size();
		layer2Tris[2*i+1].c[2] = (i+1) % points.size() - points.size();
	}

	/*
	Build top and bottom walls (disks) of cylinder by generating
	vertices and linking them with just-constructed triangle lists
	*/
	for (int side = 0; side <= 1; ++side) 
	{
		float z;
		int o1, o2;

		if (side)
		{
			o1 = 1;
			o2 = 2;
			z = 5.0 * length;
		}
		else
		{
			o1 = 2;
			o2 = 1;
			z = 0.0;
		}

		Point3D n;
		n.x = 0.0; 
		n.y = 0.0; 
		n.z = (side ? 1.0:-1.0);

		// throw down first layer of points and triangles (central layer)
		Point3D p;
		p.x = p.y = 0.0;
		p.z = z;
		geometry->vertices.push_back(Vertex(p, n));

		uint32_t base = geometry->vertices.size();
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			p.xy = points[j];
			p.x *= 0.5; // fit this ring of points inside full cylinder cap
			p.y *= 0.5;
			p.z = z;
			geometry->vertices.push_back(Vertex(p, n));
		}
		for (unsigned int j = 0; j < layer1Tris.size(); ++j)
		{
			geometry->triangles.push_back(Triangle(base + layer1Tris[j].c[0], base + layer1Tris[j].c[o1], base + layer1Tris[j].c[o2]));
		}

		// throw down second layer of points and triangles (outer ring layer)
		base = geometry->vertices.size();
		for (unsigned int j = 0; j < points.size(); ++j)
		{
			p.xy = points[j];
			p.z = z;
			geometry->vertices.push_back(Vertex(p, n));
		}
		for (unsigned int j = 0; j < layer2Tris.size(); ++j)
		{
			geometry->triangles.push_back(Triangle(base + layer2Tris[j].c[0], base + layer2Tris[j].c[o1], 
				base + layer2Tris[j].c[o2]));
		}
	}
	assert(geometry->valid());

	// Actually do the transformations on the basic canonical cylinder mesh
	for (uint32_t v = first_vert; v < geometry->vertices.size(); ++v)
	{
		applySubplanTransforms(geometry->vertices[v], ancestors);
	}
	geometry->groups.push_back(Group(first_triangle, geometry->triangles.size() - first_triangle, 
		first_vert, geometry->vertices.size() - first_vert, color, ensureVisible));
}

void generateMesh(PickupPlan* pickup, Geometry *geometry, unsigned int quality)
{
	if (pickup == NULL)
		return;

	geometry->clear();
	
	// first, generate all the plans that are used (both plan and length, 
	// both of which affect mesh). 
	vector< pair<SubpickupTemplate, Geometry> > plan_and_geom;
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		bool found = false;
		for (unsigned int j = 0; j < plan_and_geom.size(); ++j)
		{
			if (plan_and_geom[j].first.plan == pickup->subs[i].plan
				&& plan_and_geom[j].first.length == pickup->subs[i].length)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			pair<SubpickupTemplate, Geometry> newPair(pickup->subs[i], Geometry());
			vector<ancestor> ancestors;
			recurseMesh(newPair.first.plan, &(newPair.second), ancestors, newPair.first.length, quality, true);
			plan_and_geom.push_back(newPair);
		}
	}

	// next, use the generated cane set as a basis for all the subplans
	// if a cane appears multiple times, make multiple copies, applying
	// the specific pickup transformations for it, according to where it 
	// lives in the pickup.
	for (unsigned int i = 0; i < pickup->subs.size(); ++i)
	{
		Geometry* subGeom;
		bool found = false;
		for (unsigned int j = 0; j < plan_and_geom.size(); ++j)
		{
			if (plan_and_geom[j].first.plan == pickup->subs[i].plan
				&& plan_and_geom[j].first.length == pickup->subs[i].length)
			{
				found = true;
				subGeom = &(plan_and_geom[i].second); 
				break;
			}
		}
		assert(found);

		uint32_t startVert = geometry->vertices.size();
		for (unsigned int j = 0; j < subGeom->vertices.size(); ++j)
		{
			Vertex v = subGeom->vertices[j];
			// apply the transformation specific to location in pickup 
			applyPickupTransform(v, pickup->subs[i]); 
			geometry->vertices.push_back(v);
		}	
		uint32_t startTri = geometry->triangles.size();
		for (unsigned int j = 0; j < subGeom->triangles.size(); ++j)
		{
			Triangle t = subGeom->triangles[j];
			t.v1 += startVert;
			t.v2 += startVert;
			t.v3 += startVert;
			geometry->triangles.push_back(t);
		}
		for (unsigned int j = 0; j < subGeom->groups.size(); ++j)
		{
			Group g = subGeom->groups[j];
			g.vertex_begin += startVert;
			g.triangle_begin += startTri;
			geometry->groups.push_back(g);
		}
	}
}

// recurseMesh() turns a cane into geometry that can be rendered.
// since canes can be nested, recurseMesh() processes these nestings recursively.
// keeping a running stack of ancestors nodes in the dependancy DAG
// and applying them when a leaf node is encountered.
void recurseMesh(PullPlan* plan, Geometry *geometry, vector<ancestor>& ancestors, float length, 
	unsigned int quality, bool isTopLevel)
{
	if (plan == NULL)
		return;

	// Recurse through to children 
	ancestor me = {plan, 0};
	for (unsigned int i = 0; i < plan->subs.size(); ++i)
	{
		me.child = i;
		ancestors.push_back(me);
		recurseMesh(plan->subs[i].plan, geometry, ancestors, length, quality, false);
		ancestors.pop_back();
	}

	// if you're the outermost casing in the root node of the cane,
	// you get a special flag that causes your alpha to be rounded up to some minimum
	// value to fake incidence of refraction
	bool ensureVisible = (ancestors.size() == 0) && isTopLevel; 
	for (unsigned int i = 0; i < plan->getCasingCount(); ++i) 
	{
		bool outermostLayer = (i == plan->getCasingCount()-1);

		if (i == 0)
		{
			// punting on actually doing this geometry right and just making it a cylinder
			// (that intersects its subcanes)
			meshBaseCane(geometry, ancestors, plan->getCasingColor(0)->getColor(), 
				plan->getCasingShape(0), length-0.001, plan->getCasingThickness(0), quality, 
				ensureVisible && outermostLayer);
		}
		else
		{
			meshBaseCasing(geometry, ancestors, plan->getCasingColor(i)->getColor(), 
				plan->getCasingShape(i), plan->getCasingShape(i-1), length,
				plan->getCasingThickness(i), plan->getCasingThickness(i-1)+0.05, quality,
				ensureVisible && outermostLayer);
		}

	}
}

}




