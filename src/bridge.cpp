/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "bridge.h"
#include "utils/polygondebug.h"
#include "utils/remove_utils.h"

#include <algorithm>

namespace cura {

double area_cmp(PolygonRef r1, PolygonRef r2)
{
    return fabs(r1.area()) > fabs(r2.area());
}

bool remove_orientation(PolygonRef r)
{
    return  !r.orientation();
}

int bridgeAngle(Polygons outline, SliceLayer* prevLayer)
{
    AABB boundaryBox(outline);
    //To detect if we have a bridge, first calculate the intersection of the current layer with the previous layer.
    // This gives us the islands that the layer rests on.
    Polygons islands;
    for (auto prevLayerPart : prevLayer->parts)
    {
        if (!boundaryBox.hit(prevLayerPart.boundaryBox))
            continue;

        islands.add(outline.intersection(prevLayerPart.outline));
    }
    if (islands.size() > 5 || islands.size() < 1)
        return -1;

    auto iter = std::max_element(std::begin(islands), std::end(islands), area_cmp);
    PolygonRef idx1 = *iter;
    // Remove that from the list
    islands.erase(iter);
    // Find the second one
    iter = std::max_element(std::begin(islands), std::end(islands), area_cmp);
    PolygonRef idx2 = *iter;

    Point center1 = idx1.centerOfMass();
    Point center2 = idx2.centerOfMass();

    return angle(center2 - center1);
}
}//namespace cura
