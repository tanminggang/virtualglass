
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include "primitives.h"

using std::vector;
using std::string;

// Mesh stuff
class Vertex
{
	public:
		Vertex(Point3D const &_position, Point3D const &_normal);
		Point3D position;
		Point3D normal;
};

class Triangle
{
	public:
		Triangle(uint32_t _v1, uint32_t _v2, uint32_t _v3);
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;
};


//groups are a (hack-y) way of tracking triangles that belong to specific canes
class Group
{
	public:
		Group(uint32_t _triangle_begin, uint32_t _triangle_size, uint32_t _vertex_begin, uint32_t _vertex_size, 
			Color _color);
		uint32_t triangle_begin;
		uint32_t triangle_size;
		uint32_t vertex_begin;
		uint32_t vertex_size;
		Color color;
};

class Geometry
{
	public:
		vector< Vertex > vertices;
		vector< Triangle > triangles;
		vector< Group > groups;
		void clear();
		bool valid() const;
		void compute_normals_from_triangles();
		void save_obj_file(string const &filename) const;
		void save_ply_file(string const &filename) const;
		void save_raw_file(string const &filename) const;
};

#endif

