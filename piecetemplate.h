
#ifndef PIECETEMPLATE_H
#define PIECETEMPLATE_H

#include <vector>
#include "primitives.h"
#include "constants.h"

#define WAVY_ONE_TEMPLATE 1
#define WAVY_TWO_TEMPLATE 2
#define WAVY_THREE_TEMPLATE 3
#define WAVY_FOUR_TEMPLATE 4
#define ROLLUP_TEMPLATE 5

using std::vector;

class PieceTemplate
{
	public:
		PieceTemplate(int t);
		int type;
};

#endif

