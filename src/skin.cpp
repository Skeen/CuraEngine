/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "skin.h"

#include <algorithm>

void generateSkins(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount, int infillOverlap)
{
    SliceLayer* layer = &storage.layers[layerNr];

    for(SliceLayerPart& part : layer->parts)
    {
        Polygons upskin = part.insets[part.insets.size() - 1].offset(-extrusionWidth/2);
        Polygons downskin = upskin;
        
        if (part.insets.size() > 1)
        {
            //Add thin wall filling by taking the area between the insets.
            Polygons thinWalls = part.insets[0].offset(-extrusionWidth / 2 - extrusionWidth * infillOverlap / 100).difference(part.insets[1].offset(extrusionWidth * 6 / 10));
            upskin.add(thinWalls);
            downskin.add(thinWalls);
        }
        if (int(layerNr - downSkinCount) >= 0)
        {
            SliceLayer* layer2 = &storage.layers[layerNr - downSkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                    downskin = downskin.difference(comp.insets[comp.insets.size() - 1]);
            }
        }
        if (int(layerNr + upSkinCount) < (int)storage.layers.size())
        {
            SliceLayer* layer2 = &storage.layers[layerNr + upSkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                    upskin = upskin.difference(comp.insets[comp.insets.size() - 1]);
            }
        }
        
        part.skinOutline = upskin.unionPolygons(downskin);

        double minAreaSize = (2 * M_PI * INT2MM(extrusionWidth) * INT2MM(extrusionWidth)) * 0.3;
        part.skinOutline.erase(std::remove_if(part.skinOutline.begin(), part.skinOutline.end(),
                [minAreaSize](PolygonRef r)
                {
                    // Only create an up/down skin if the area is large enough. So you do not create tiny blobs of "trying to fill"
                    double area = INT2MM(INT2MM(fabs(r.area())));
                    return (area < minAreaSize);
                }), part.skinOutline.end());
    }
}

void generateSparse(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount)
{
    SliceLayer* layer = &storage.layers[layerNr];

    for(SliceLayerPart& part : layer->parts)
    {
        Polygons sparse = part.insets[part.insets.size() - 1].offset(-extrusionWidth/2);
        Polygons downskin = sparse;
        Polygons upskin = sparse;
        
        if (int(layerNr - downSkinCount) >= 0)
        {
            SliceLayer* layer2 = &storage.layers[layerNr - downSkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                {
                    if (comp.insets.size() > 1)
                    {
                        downskin = downskin.difference(comp.insets[comp.insets.size() - 2]);
                    }else{
                        downskin = downskin.difference(comp.insets[comp.insets.size() - 1]);
                    }
                }
            }
        }
        if (int(layerNr + upSkinCount) < (int)storage.layers.size())
        {
            SliceLayer* layer2 = &storage.layers[layerNr + upSkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                {
                    if (comp.insets.size() > 1)
                    {
                        upskin = upskin.difference(comp.insets[comp.insets.size() - 2]);
                    }else{
                        upskin = upskin.difference(comp.insets[comp.insets.size() - 1]);
                    }
                }
            }
        }
        
        Polygons result = upskin.unionPolygons(downskin);

        double minAreaSize = 3.0;//(2 * M_PI * INT2MM(config.extrusionWidth) * INT2MM(config.extrusionWidth)) * 3;
        result.erase(std::remove_if(result.begin(), result.end(),
                [minAreaSize](PolygonRef r)
                {
                    // Only create an up/down skin if the area is large enough. So you do not create tiny blobs of "trying to fill"
                    double area = INT2MM(INT2MM(fabs(r.area())));
                    return (area < minAreaSize);
                }), result.end());

        part.sparseOutline = sparse.difference(result);
    }
}
