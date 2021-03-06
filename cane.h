

#ifndef CANE_H
#define CANE_H

#include <vector>
#include <stack>

#include <QObject>

#include "canetemplate.h"
#include "shape.h"
#include "primitives.h"
#include "subcanetemplate.h"
#include "casing.h"
#include "templateparameter.h"

using std::vector;
using std::stack;

class Cane;
class GlassColor;

class Cane : public QObject
{
	Q_OBJECT

	public:
		Cane(enum CaneTemplate::Type t);

		void setTemplateType(enum CaneTemplate::Type t);
		enum CaneTemplate::Type templateType() const;

		void setCount(unsigned int count);
		unsigned int count() const;

		void setCasingThickness(float t, unsigned int index);
		float casingThickness(unsigned int index) const;

		void setOutermostCasingShape(enum GeometricShape s);
		enum GeometricShape casingShape(unsigned int index) const;
		enum GeometricShape outermostCasingShape() const;

		void setCasingColor(GlassColor* gc, unsigned int index);
		void setOutermostCasingColor(GlassColor* gc);
		const GlassColor* casingColor(unsigned int index) const;
		const GlassColor* outermostCasingColor() const;

		float twist() const;
		void setTwist(float t);

		void addCasing(enum GeometricShape s);
		void removeCasing();
		unsigned int casingCount() const; 
		bool hasMinimumCasingCount() const;

		Cane* copy() const;
		void set(Cane* c);

		SubcaneTemplate subcaneTemplate(unsigned int index) const;
		void setSubcaneTemplate(SubcaneTemplate t, unsigned int index);	
		void addSubcaneTemplate(SubcaneTemplate t);
		void removeSubcaneTemplate(unsigned int index);
		unsigned int subcaneCount() const;	

		bool hasDependencyOn(GlassColor* color) const;
		bool hasDependencyOn(Cane* cane) const;

	signals:
		void modified();

	private:
		void initializeTemplate();
		void resetSubcanes(bool hardReset);
		void pushNewSubcane(bool hardReset, vector<SubcaneTemplate>* newSubcanes,
			enum GeometricShape s, Point2D location, float diameter);

		unsigned int casingDependencyOccurrances(GlassColor* glassColor);
		void addCasingDependency(GlassColor* gc);
		void removeCasingDependency(GlassColor* gc);

		unsigned int subcaneDependencyOccurrances(Cane* cane);
		void addSubcaneDependency(Cane* cane);
		void removeSubcaneDependency(Cane* cane);

		enum CaneTemplate::Type type_;
		vector<Casing> casings_;
		unsigned int count_;
		float twist_;
		vector<SubcaneTemplate> subcanes_;

	private slots:
		void dependencyModified();
};

Cane * deep_copy(const Cane *cane);
void deep_delete(Cane *cane);

#endif

