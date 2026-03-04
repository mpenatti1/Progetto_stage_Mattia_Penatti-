#include "Range.h"

bool Range::contains( KDpoint* p){
    return p->getX() >= xmin && p->getX() <= xmax &&
           p->getY() >= ymin && p->getY() <= ymax;
}
