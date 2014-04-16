/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "inset.h"
#include "polygonOptimizer.h"

#include "utils/remove_utils.h"

#include <algorithm>
namespace cura {

#include "utils/remove_utils.h"

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

bool remove_part(const SliceLayerPart& part)
{
    return (part.insets.size() < 1);
}

void generateInsets(SliceLayer* layer, int offset, int insetCount)
{
    for(SliceLayerPart& part : layer->parts)
    {
        generateInsets(&part, offset, insetCount);
    }
    
    //Remove the parts which did not generate an inset. As these parts are too small to print,
    // and later code can now assume that there is always minimal 1 inset line.
    remove_if(layer->parts, remove_part);
}

}//namespace cura
