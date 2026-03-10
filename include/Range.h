#ifndef RANGE_H
#define RANGE_H
#include "KDpoint.h"

struct Range
{
    int xmin,xmax;
    int ymin,ymax;
    bool contains( KDpoint* p);
};
#endif
