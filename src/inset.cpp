/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "inset.h"
#include "polygonOptimizer.h"

#include <algorithm>

void generateInsets(SliceLayerPart* part, int offset, int insetCount)
{
    part->combBoundery = part->outline.offset(-offset);
    if (insetCount == 0)
    {
        part->insets.push_back(part->outline);
        return;
    }
    
    for(int i=0; i<insetCount; i++)
    {
        part->insets.push_back(Polygons());
        part->insets[i] = part->outline.offset(-offset * i - offset/2);
        optimizePolygons(part->insets[i]);
        if (part->insets[i].size() < 1)
        {
            part->insets.pop_back();
            break;
        }
    }
}

void generateInsets(SliceLayer* layer, int offset, int insetCount)
{
    for(SliceLayerPart& part : layer->parts)
    {
        generateInsets(&part, offset, insetCount);
    }
    
    //Remove the parts which did not generate an inset. As these parts are too small to print,
    // and later code can now assume that there is always minimal 1 inset line.
    layer->parts.erase(std::remove_if(std::begin(layer->parts), std::end(layer->parts),
                [](const SliceLayerPart& part)
                {
                return (part.insets.size() < 1);
                }), layer->parts.end());
}

