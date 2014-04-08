#ifndef MULTIVOLUMES_H
#define MULTIVOLUMES_H

#include <vector>

#include "sliceDataStorage.h"

/* This file contains code to help fixing up and changing layers that are build from multiple volumes. */

void carveMultipleVolumes(std::vector<SliceVolumeStorage> &volumes);

//Expand each layer a bit and then keep the extra overlapping parts that overlap with other volumes.
//This generates some overlap in dual extrusion, for better bonding in touching parts.
void generateMultipleVolumesOverlap(std::vector<SliceVolumeStorage> &volumes, int overlap);

#endif//MULTIVOLUMES_H

