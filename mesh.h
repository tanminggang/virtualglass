

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "pullplan.h"
#include "geometry.h"
#include "subpulltemplate.h"
#include "pickupplan.h"
#include "subpickuptemplate.h"
#include "piece.h"

class Mesher
{

	public:
		Mesher();
		void generateMesh(PickupPlan* plan, Geometry *geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices);
		void generateMesh(Piece* piece, Geometry *geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices);
		void generateMesh(PullPlan* plan, Geometry *geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices, 
			float start, float end, bool addCasing = false, int groupIndex = -1);

	private:
		// Methods
		void meshPolygonalBaseCane(Geometry* geometry, vector<PullPlan*> ancestors, vector<int> ancestorIndices, 
			PullPlan* plan, float start, float end, uint32_t group_tag);
		void applyMoveAndResizeTransform(Vertex* v, PullPlan* parentPlan, int subplan);
		void applyMoveAndResizeTransform(Geometry* geometry, PullPlan* parentPlan, int subplan);
		void applyTwistTransform(Vertex* v, PullPlan* p);
		void applyTwistTransform(Geometry* geometry, PullPlan* p);
		Vertex applyTransforms(Vertex p, vector<PullPlan*> ancestors, vector<int> ancestorIndices);
		void applyPickupTransform(Vertex* p, SubpickupTemplate* spt);
		void applyTumblerTransform(Vertex* p, vector<int> parameterValues);
		void applyBowlTransform(Vertex* p, vector<int> parameterValues);

		// Variables
		PullPlan* circleCasing;
		PullPlan* squareCasing;
};
#endif

