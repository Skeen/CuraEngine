/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "infill.h"

#include <algorithm>

void generateConcentricInfill(Polygons outline, Polygons& result, int offsets[], int offsetsSize)
{
    int step = 0;
    while(1)
    {
        for(unsigned int polygonNr=0; polygonNr<outline.size(); polygonNr++)
            result.add(outline[polygonNr]);
        outline = outline.offset(-offsets[step]);
        if (outline.size() < 1)
            break;
        step = (step + 1) % offsetsSize;
    }
}

void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation)
{
    Polygons outline = in_outline.offset(extrusionWidth * infillOverlap / 100);
    PointMatrix matrix(rotation);
    
    outline.applyMatrix(matrix);
    
    AABB boundary(outline);
    
    boundary.min.X = ((boundary.min.X / lineSpacing) - 1) * lineSpacing;
    int lineCount = (boundary.max.X - boundary.min.X + (lineSpacing - 1)) / lineSpacing;
    vector<vector<int64_t> > cutList;
    for(int n=0; n<lineCount; n++)
        cutList.emplace_back();

    for(PolygonRef r : outline)
    {
        Point p1 = r[r.size() - 1];

        for(Point& p0 : r)
        {
            int idx0 = (p0.X - boundary.min.X) / lineSpacing;
            int idx1 = (p1.X - boundary.min.X) / lineSpacing;

            int64_t xMin = std::min(p0.X, p1.X);
            int64_t xMax = std::max(p0.X, p1.X);

            int idxMin = std::min(idx0, idx1);
            int idxMax = std::max(idx0, idx1);

            for (int idx = idxMin; idx <= idxMax; idx++)
            {
                int x = (idx * lineSpacing) + boundary.min.X + lineSpacing / 2;
                if ((x < xMin) || (x >= xMax))
                    continue;
                int y = p0.Y + (p1.Y - p0.Y) * (x - p0.X) / (p1.X - p0.X);
                cutList[idx].push_back(y);
            }
            p1 = p0;
        }
    }

    // Our starting point on the x-axis
    int64_t x = (boundary.min.X + lineSpacing / 2);
    for (auto& list : cutList)
    {
        // Breaking condition, is if we're outside the x-axis
        if (x > boundary.max.X)
            break;
    
        std::sort(list.begin(), list.end());
       for(unsigned int i = 0; i + 1 < list.size(); i+=2)
        {
            if (list[i+1] - list[i] < extrusionWidth / 5)
                continue;
            PolygonRef p = result.newPoly();
            p.add(matrix.unapply(Point(x, list[i])));
            p.add(matrix.unapply(Point(x, list[i+1])));
        }
        // At each iteration, step a line spacing up the x-axis
        x += lineSpacing;
    }
}

