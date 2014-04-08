/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef OPTIMIZED_MODEL_H
#define OPTIMIZED_MODEL_H

#include "modelFile/modelFile.h"

#include <vector>

struct OptimizedFace
{
    int index[3];
    int touching[3];
};

class OptimizedPoint3
{
public:
    Point3 p;
    std::vector<uint32_t> faceIndexList;
    
    OptimizedPoint3() {}
    OptimizedPoint3(Point3 p): p(p) {}
};

class OptimizedModel;
class OptimizedVolume
{
public:
    OptimizedModel* model;
    std::vector<OptimizedPoint3> points;
    std::vector<OptimizedFace> faces;
    
    OptimizedVolume(SimpleVolume* volume, OptimizedModel* model);
    
    int getFaceIdxWithPoints(int idx0, int idx1, int notFaceIdx);
};

class OptimizedModel
{
public:
    std::vector<OptimizedVolume> volumes;
    Point3 vMin;
    Point3 vMax;
    Point3 modelSize;

    OptimizedModel(SimpleModel* model, Point3 center);
    void saveDebugSTL(const char* filename);
};

#endif//OPTIMIZED_MODEL_H
