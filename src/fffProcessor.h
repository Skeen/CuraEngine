#ifndef FFF_PROCESSOR_H
#define FFF_PROCESSOR_H

#include <algorithm>
#include "utils/socket.h"
#include "utils/gettime.h"
#include "gcodeExport.h"
#include "sliceDataStorage.h"


#define GUI_CMD_REQUEST_MESH 0x01
#define GUI_CMD_SEND_POLYGONS 0x02
#define GUI_CMD_FINISH_OBJECT 0x03

//FusedFilamentFabrication processor.
class fffProcessor
{
private:
    int maxObjectHeight = 0;
    int fileNr = 1;
    GCodeExport gcode;
    ConfigSettings& config;
    TimeKeeper timeKeeper;
    ClientSocket guiSocket;

    GCodePathConfig skirtConfig;
    GCodePathConfig inset0Config;
    GCodePathConfig insetXConfig;
    GCodePathConfig fillConfig;
    GCodePathConfig supportConfig;
public:
    fffProcessor(ConfigSettings& config);

    void guiConnect(int portNr);

    void sendPolygonsToGui(const char* name, int layerNr, int32_t z, Polygons& polygons);

    bool setTargetFile(const char* filename);

    bool processFile(const char* input_filename);

    void finalize();

private:
    void preSetup();

    bool prepareModel(SliceDataStorage& storage, const char* input_filename);

    void processSliceData(SliceDataStorage& storage);
    
    void writeGCode(SliceDataStorage& storage);

    //Add a single layer from a single mesh-volume to the GCode
    void addVolumeLayerToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int volumeIdx, int layerNr);

    void addSupportToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr);

    void addWipeTower(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr, int prevExtruder);
};

#endif//FFF_PROCESSOR_H
