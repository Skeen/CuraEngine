#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>

#include "utils/floatpoint.h"

#ifndef VERSION
#define VERSION "DEV"
#endif

#ifndef DEFAULT_CONFIG_PATH
#define DEFAULT_CONFIG_PATH "default.cfg"
#endif

#define CONFIG_MULTILINE_SEPARATOR "\"\"\""

// TODO: Use multiple bools instead
enum Fix_Horrible
{
    FIX_HORRIBLE_UNION_ALL_TYPE_A = 0x01,
    FIX_HORRIBLE_UNION_ALL_TYPE_B = 0x02,
    FIX_HORRIBLE_EXTENSIVE_STITCHING = 0x04,
    FIX_HORRIBLE_UNION_ALL_TYPE_C = 0x08,
    FIX_HORRIBLE_KEEP_NONE_CLOSED = 0x10
};

/**
 * Type of support material.
 * Grid is a X/Y grid with an outline, which is very strong, provides good support. But in some cases is hard to remove.
 * Lines give a row of lines which break off one at a time, making them easier to remove, but they do not support as good as the grid support.
 */
// TODO: Use strongly typed enum
enum Support_Type
{
    SUPPORT_TYPE_GRID = 0,
    SUPPORT_TYPE_LINES = 1
};

// TODO: Use strongly typed enum
enum GCode_Flavor
{
/**
 * RepRap flavored GCode is Marlin/Sprinter/Repetier based GCode.
 *  This is the most commonly used GCode set.
 *  G0 for moves, G1 for extrusion.
 *  E values give mm of filament extrusion.
 *  Retraction is done on E values with G1. Start/end code is added.
 *  M106 Sxxx and M107 are used to turn the fan on/off.
 **/
GCODE_FLAVOR_REPRAP = 0,
/**
 * UltiGCode flavored is Marlin based GCode.
 *  UltiGCode uses less settings on the slicer and puts more settings in the firmware. This makes for more hardware/material independed GCode.
 *  G0 for moves, G1 for extrusion.
 *  E values give mm^3 of filament extrusion. Ignores the filament diameter setting.
 *  Retraction is done with G10 and G11. Retraction settings are ignored. G10 S1 is used for multi-extruder switch retraction.
 *  Start/end code is not added.
 *  M106 Sxxx and M107 are used to turn the fan on/off.
 **/
GCODE_FLAVOR_ULTIGCODE = 1,
/**
 * Makerbot flavored GCode.
 *  Looks a lot like RepRap GCode with a few changes. Requires MakerWare to convert to X3G files.
 *   Heating needs to be done with M104 Sxxx T0
 *   No G21 or G90
 *   Fan ON is M126 T0 (No fan strength control?)
 *   Fan OFF is M127 T0
 *   Homing is done with G162 X Y F2000
 **/
GCODE_FLAVOR_MAKERBOT = 2,
/**
 * Bits From Bytes GCode.
 *  BFB machines use RPM instead of E. Which is coupled to the F instead of independed. (M108 S[deciRPM])
 *  Need X,Y,Z,F on every line.
 *  Needs extruder ON/OFF (M101, M103), has auto-retrection (M227 S[2560*mm] P[2560*mm])
 **/
GCODE_FLAVOR_BFB = 3,
/**
 * MACH3 GCode
 *  MACH3 is CNC control software, which expects A/B/C/D for extruders, instead of E.
 **/
GCODE_FLAVOR_MACH3 = 4
};

constexpr unsigned int MAX_EXTRUDERS = 16;

class _ConfigSettingIndex
{
public:
    const char* key;
    int* ptr;

    _ConfigSettingIndex(const char* key, int* ptr) : key(key), ptr(ptr) {}
};

class ConfigSettings
{
private:
    std::vector<_ConfigSettingIndex> _index;
public:
    int layerThickness          = 100;
    int initialLayerThickness   = 300;
    int filamentDiameter        = 2890;
    int filamentFlow            = 100;
    int layer0extrusionWidth    = 600;
    int extrusionWidth          = 400;
    int insetCount              = 2;
    int downSkinCount           = 6;
    int upSkinCount             = 6;
    int sparseInfillLineDistance = 100 * extrusionWidth / 20;
    int infillOverlap           = 15;
    int skirtDistance           = 6000;
    int skirtLineCount          = 1;
    int skirtMinLength          = 0;

    //Retraction settings
    int retractionAmount        = 4500;
    int retractionAmountPrime   = 0;
    int retractionAmountExtruderSwitch = 14500;
    int retractionSpeed         = 45;
    int retractionMinimalDistance = 1500;
    int minimalExtrusionBeforeRetraction = 100;
    int retractionZHop          = 0;

    int enableCombing           = 1;
    int enableOozeShield        = 0;
    int wipeTowerSize           = 0;
    int multiVolumeOverlap      = 0;

    int initialSpeedupLayers    = 4;
    int initialLayerSpeed       = 20;
    int printSpeed              = 50;
    int infillSpeed             = 50;
    int inset0Speed             = 50;
    int insetXSpeed             = 50;
    int moveSpeed               = 150;
    int fanFullOnLayerNr        = 2;

    //Support material
    int supportType             = SUPPORT_TYPE_GRID;
    int supportAngle            = -1;
    int supportEverywhere       = 0;
    int supportLineDistance     = sparseInfillLineDistance;
    int supportXYDistance       = 700;
    int supportZDistance        = 150;
    int supportExtruder         = -1;

    //Cool settings
    int minimalLayerTime        = 5;
    int minimalFeedrate         = 10;
    int coolHeadLift            = 0;
    int fanSpeedMin             = 100;
    int fanSpeedMax             = 100;

    //Raft settings
    int raftMargin              = 5000;
    int raftLineSpacing         = 1000;
    int raftBaseThickness       = 0;
    int raftBaseLinewidth       = 0;
    int raftBaseSpeed           = 0;
    int raftInterfaceThickness  = 0;
    int raftInterfaceLinewidth  = 0;
    int raftInterfaceLineSpacing = 250;
    int raftFanSpeed            = 0;
    int raftSurfaceThickness    = 0;
    int raftSurfaceLinewidth    = 0;
    int raftSurfaceLineSpacing  = 0;
    int raftSurfaceLayers       = 0;
    int raftSurfaceSpeed        = 0;
    int raftAirGap              = 0;

    FMatrix3x3 matrix;
    IntPoint objectPosition     = IntPoint(102500, 102500);
    int objectSink              = 0;

    int fixHorrible             = 0;
    int spiralizeMode           = 0;
    int gcodeFlavor             = GCODE_FLAVOR_REPRAP;

    IntPoint extruderOffset[MAX_EXTRUDERS];
    std::string startCode = R"(
M109 S210     ;Heatup to 210C
G21           ;metric values
G90           ;absolute positioning
G28           ;Home
G1 Z15.0 F300 ;move the platform down 15mm
G92 E0        ;zero the extruded length
G1 F200 E5    ;extrude 5mm of feed stock
G92 E0        ;zero the extruded length again
)";

    std::string endCode = R"(
M104 S0                     ;extruder heater off
M140 S0                     ;heated bed heater off (if you have it)
G91                            ;relative positioning
G1 E-1 F300                    ;retract the filament a bit before lifting the nozzle, to release some of the pressure
G1 Z+0.5 E-5 X-20 Y-20 F9000   ;move Z up a bit and retract filament even more
G28 X0 Y0                      ;move X/Y to min endstops, so the head is out of the way
M84                         ;steppers off
G90                         ;absolute positioning
)";

    ConfigSettings();
    bool setSetting(const char* key, const char* value);
    bool readSettings(void);
    bool readSettings(const char* path);
};

#endif//SETTINGS_H
