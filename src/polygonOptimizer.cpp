/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "polygonOptimizer.h"

#include "utils/remove_utils.h"

#include <algorithm>
namespace cura {

void optimizePolygon(PolygonRef poly)
{
    Point p0 = poly[poly.size()-1];
    // TODO: Foreach loop
    for(unsigned int i=0;i<poly.size();i++)
    {
        Point p1 = poly[i];
        if (shorterThen(p0 - p1, MICRON2INT(10)))
        {
            poly.remove(i);
            i --;
        }else if (shorterThen(p0 - p1, MICRON2INT(500)))
        {
            Point p2;
            if (i < poly.size() - 1)
                p2 = poly[i+1];
            else
                p2 = poly[0];
            
            Point diff0 = normal(p1 - p0, 10000000);
            Point diff2 = normal(p1 - p2, 10000000);
            
            int64_t d = dot(diff0, diff2);
            if (d < -99999999999999LL)
            {
                poly.remove(i);
                i --;
            }else{
                p0 = p1;
            }
        }else{
            p0 = p1;
        }
    }
}

bool removePolygon(PolygonRef r)
{
    return (r.size() < 3);
}

void optimizePolygons(Polygons& polys)
{
    // Optimize all polygons
    for(PolygonRef r : polys)
    {
        optimizePolygon(r);
    }
    // Remove all the ones below 3 in size
    remove_if(polys, removePolygon);
}

}//namespace cura
