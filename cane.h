

#ifndef CANE_H
#define CANE_H

#include <QMetaType>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"
#include "constants.h"
#include "yaml-cpp/yaml.h"
#include <QColor>

class Cane
{
public:
	Cane();
	Cane(int type);
	int height();
	void reset();
	void shallowCopy(Cane* dest);
	void pullIntuitive(float twistFactor, float stretchFactor);
	void pullIntuitive(int subcane, float twistFactor, float stretchFactor);
	void pullLinear(float twistFactor, float stretchFactor);
	void flatten(float rectangle_ratio, float rectangle_theta, float flatness);
	void flatten(int subcane, float rectangle_ratio, float rectangle_theta, float flatness);
        void createFlatten(int subcane);
        void createFlatten();
        void createPull(int subcane);
        void createPull();
	void createBundle();
	void add(Cane* addl);
	void deleteCane(int subcane);
	void moveCane(int subcane, float delta_x, float delta_y, float delta_z);
	int leafNodes();
	Cane* deepCopy();
	void setColor(Color color);
	void setColor(QColor color);
	std::string yamlRepresentation();
	QColor qcolor();
	QString typeName();
	QString typeAmt(int type, int index);

	int type;
	std::vector<Point> vertices;
	float amts[MAX_AMT_TYPES];
	int subcaneCount;
	Point subcaneLocations[MAX_SUBCANE_COUNT]; // z is center displacement
	Cane* subcanes[MAX_SUBCANE_COUNT];
	Color color;
	int libraryIndex;

public slots:
	void changeLibraryIndex(int index);

private:
	Cane* getTopBundleNode();

};

Q_DECLARE_METATYPE(Cane)
Q_DECLARE_METATYPE(Cane*)

#endif


