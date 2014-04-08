/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include <stdio.h>

#include "utils/gettime.h"
#include "utils/logoutput.h"
#include "optimizedModel.h"

#define MELD_DIST MM2INT(0.03)
OptimizedVolume::OptimizedVolume(SimpleVolume* volume, OptimizedModel* model)
: model(model)
{
    points.reserve(volume->faces.size() * 3);
    faces.reserve(volume->faces.size());

    std::map<uint32_t, std::vector<uint32_t> > indexMap;
    
    for(SimpleFace& face : volume->faces)
    {
        OptimizedFace f;
        for(uint32_t j=0; j<3; j++)
        {
            Point3 p = face.v[j];
            int hash = ((p.x + MELD_DIST/2) / MELD_DIST) ^ (((p.y + MELD_DIST/2) / MELD_DIST) << 10) ^ (((p.z + MELD_DIST/2) / MELD_DIST) << 20);
            uint32_t idx;
            bool add = true;
            for(unsigned int n = 0; n < indexMap[hash].size(); n++)
            {
                if ((points[indexMap[hash][n]].p - p).testLength(MELD_DIST))
                {
                    idx = indexMap[hash][n];
                    add = false;
                    break;
                }
            }
            if (add)
            {
                indexMap[hash].push_back(points.size());
                idx = points.size();
                points.push_back(p);
            }
            f.index[j] = idx;
        }
        if (f.index[0] != f.index[1] && f.index[0] != f.index[2] && f.index[1] != f.index[2])
        {
            //Check if there is a face with the same points
            bool duplicate = false;
            for(unsigned int _idx0 = 0; _idx0 < points[f.index[0]].faceIndexList.size(); _idx0++)
            {
                for(unsigned int _idx1 = 0; _idx1 < points[f.index[1]].faceIndexList.size(); _idx1++)
                {
                    for(unsigned int _idx2 = 0; _idx2 < points[f.index[2]].faceIndexList.size(); _idx2++)
                    {
                        if (points[f.index[0]].faceIndexList[_idx0] == points[f.index[1]].faceIndexList[_idx1] && points[f.index[0]].faceIndexList[_idx0] == points[f.index[2]].faceIndexList[_idx2])
                            duplicate = true;
                    }
                }
            }
            if (!duplicate)
            {
                for(int x=0; x<3; x++)
                    points[f.index[x]].faceIndexList.push_back(faces.size());
                faces.push_back(f);
            }
        }
    }
    //fprintf(stdout, "\rAll faces are optimized in %5.1fs.\n",timeElapsed(t));

    int openFacesCount = 0;
    int i = 0;
    for(OptimizedFace& f : faces)
    {
        f.touching[0] = getFaceIdxWithPoints(f.index[0], f.index[1], i);
        f.touching[1] = getFaceIdxWithPoints(f.index[1], f.index[2], i);
        f.touching[2] = getFaceIdxWithPoints(f.index[2], f.index[0], i);
        for(int x=0; x<3; x++)
            if (f.touching[x] == -1)
                openFacesCount++;

        i++;
    }
    //fprintf(stdout, "  Number of open faces: %i\n", openFacesCount);
}


void OptimizedModel::saveDebugSTL(const char* filename)
{
    char buffer[80] = "Cura_Engine_STL_export";
    uint32_t n;
    uint16_t s;
    float flt;
    OptimizedVolume* vol = &volumes[0];
    FILE* f = fopen(filename, "wb");
    fwrite(buffer, 80, 1, f);
    n = vol->faces.size();
    fwrite(&n, sizeof(n), 1, f);
    for(OptimizedFace& face : vol->faces)
    {
        flt = 0;
        s = 0;
        fwrite(&flt, sizeof(flt), 1, f);
        fwrite(&flt, sizeof(flt), 1, f);
        fwrite(&flt, sizeof(flt), 1, f);

        for(int x=0; x<3; x++)
        {
            auto& point = vol->points[face.index[x]];
            flt = INT2MM(point.p.x); fwrite(&flt, sizeof(flt), 1, f);
            flt = INT2MM(point.p.y); fwrite(&flt, sizeof(flt), 1, f);
            flt = INT2MM(point.p.z); fwrite(&flt, sizeof(flt), 1, f);
        }

        fwrite(&s, sizeof(s), 1, f);
    }
    fclose(f);
}
