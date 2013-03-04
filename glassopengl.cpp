
#include "glew.h"
#ifdef _WIN32
#  include <windows.h>
#endif
#include <iostream>
#include "geometry.h"
#include "glassopengl.h"
#include "globalbackgroundcolor.h"
#include "constants.h"

namespace GlassOpenGL
{

void errors(string const &where)
{
	GLuint err;
	while ((err = glGetError()) != GL_NO_ERROR) 
	{
		std::cerr << "(in " << where << ") OpenGL error #" << err
			<< ": " << gluErrorString(err) << std::endl;
	}
}

void initialize()
{
	// For shadow/lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
}

void renderWithoutDepthPeeling(const Geometry& geometry)
{
	// This method has a lot in common with PeelRenderer::render()
	// It's effectively a weak skeleton of that one, with a new trick thrown in.
	// The goal here is to render without using real transparency testing
	// while still looking ok; in particular no funny M.C. Escher-like artifacts.
	//
	// The trick we use is to map all the actual alphas into two groups:
	// opaque (a = 1.0) and nearly transparent (a <= 0.1). Everything with 
	// alpha more than 0.1 is made opaque and everything else keeps its originally alpha.
	// We then render all opaque things first using regular occlusion, 
	// i.e. no blending and CULL_FACE and DEPTH_TEST on. Then we render
	// all transparent things (in arbitrary/uncontrollable) order with BLEND on.
	// 
	// The "trick" is meant to fix the lack of good z-order of transparent things.
	// Intuitively, the alpha mapping enforces the following rule:
	// "Transparent things are so transparent that the user can't
	// tell if they're rendered in the wrong order." 

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Check that Vertex and Triangle have proper size:
	assert(sizeof(Vertex) == sizeof(GLfloat) * (3 + 3));
	assert(sizeof(Triangle) == sizeof(GLuint) * 3);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].position));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &(geometry.vertices[0].normal));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	// make a pass on opaque things
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c = g->color;
		if (c.a < 0.1)
			continue;
		glColor4f(c.r * 0.8f, c.g * 0.8f, c.b * 0.8f, 0.8f);
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}

	// make a pass on mandatory transparent things
	// this part assumes that there's only one mandatory transparent thing, 
	// and anything inside the cane/piece has a single face of the mandatory transparent
	// stuff in front of it.
	// moreover, we set the alpha of this mandatory transparent stuff to 0.1 and its 
	// color to the generic white (gray) colors

	// We cull back faces and fake the combined front and back faces with just front faces
	// to avoid blending and z-ordering issues associated with drawing multiple faces 
	// covering the same pixel.
	glEnable(GL_CULL_FACE); 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (std::vector< Group >::const_iterator g = geometry.groups.begin(); g != geometry.groups.end(); ++g)
	{
		Color c = g->color;
		if (c.a >= 0.1)
			continue;
		if (!g->ensureVisible)
			continue;
		c.r = c.g = c.b = 1.0;
		c.a = 0.1;

		// the 1.2 ratio is meant to adjust for the default GL_AMBIENT
		// coefficient of 0.2 and its contribution via the (undrawn) back face
		glColor4f(c.r * 1.2 * c.a, c.g * 1.2 * c.a, c.b * 1.2 * c.a, 2.0 * c.a);
		glDrawElements(GL_TRIANGLES, g->triangle_size * 3,
			GL_UNSIGNED_INT, &(geometry.triangles[g->triangle_begin].v1));
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

        //final pass -- render background color behind everything else.
        glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glBegin(GL_QUADS);
        glColor3f(GlobalBackgroundColor::color.r,
                GlobalBackgroundColor::color.g,
                GlobalBackgroundColor::color.b);
        glVertex2f(-1.1f,-1.1f);
        glVertex2f( 1.1f,-1.1f);
        glVertex2f( 1.1f, 1.1f);
        glVertex2f(-1.1f, 1.1f);
        glEnd();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

}


