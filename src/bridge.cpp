/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "bridge.h"
#include "utils/polygondebug.h"
#include "utils/remove_utils.h"

#include <algorithm>

int bridgeAngle(SliceLayerPart* part, SliceLayer* prevLayer)
{
    //To detect if we have a bridge, first calculate the intersection of the current layer with the previous layer.
    // This gives us the islands that the layer rests on.
    Polygons islands;
    for(SliceLayerPart& par : prevLayer->parts)
    {
        if (!part->boundaryBox.hit(par.boundaryBox)) continue;
        
        islands.add(part->outline.intersection(par.outline));
    }

    // TODO: Why is this check in place?
    if (islands.size() > 5)
        return -1;

    //Skip internal holes
    remove_if(islands, [](PolygonRef r)
    { return !r.orientation(); });

    // We need at least 2 islands to be existant at this point
    if (islands.size() < 2)
        return -1;
    
    // Lambda to compare area
    const auto area_cmp = [](PolygonRef r1, PolygonRef r2)
    { return fabs(r1.area()) > fabs(r2.area()); };

    // Next find the 2 largest islands that we rest on.
    // Find the first one
    auto iter = std::max_element(std::begin(islands), std::end(islands), area_cmp);
    PolygonRef idx1 = *iter;
    // Remove that from the list
    islands.erase(iter);
    // Find the second one
    iter = std::max_element(std::begin(islands), std::end(islands), area_cmp);
    PolygonRef idx2 = *iter;

    Point center1 = idx1.centerOfMass();
    Point center2 = idx2.centerOfMass();
    
    double angle = atan2(center2.X - center1.X, center2.Y - center1.Y) / M_PI * 180.0;
    if (angle < 0) angle += 360;
    return angle;
}
