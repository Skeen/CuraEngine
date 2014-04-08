#include <cctype>
#include <fstream>
#include <stdio.h>
#include "utils/logoutput.h"
#include "utils/string.h"

#include "settings.h"

#define LTRIM_STRING(s) do { while(((s).length() > 0) && isspace((s)[0])) { (s).erase(0, 1); } } while(0)
#define RTRIM_STRING(s) do { while(((s).length() > 0) && isspace((s)[(s).length() - 1])) { (s).erase((s).length() - 1); } } while(0)
#define TRIM_STRING(s) do { LTRIM_STRING(s); RTRIM_STRING(s); } while(0)
#define STRINGIFY(_s) #_s
#define SETTING(name) do { _index.push_back(_ConfigSettingIndex(STRINGIFY(name), &name)); } while(0)
#define SETTING_VAL(name, default) do { _index.push_back(_ConfigSettingIndex(STRINGIFY(name), &name)); name = default; } while(0)
#define SETTING2(name, altname) do { _index.push_back(_ConfigSettingIndex(STRINGIFY(name), &name)); _index.push_back(_ConfigSettingIndex(STRINGIFY(altname), &name)); } while(0)

ConfigSettings::ConfigSettings()
{
    SETTING(layerThickness);
    SETTING(initialLayerThickness);
    SETTING(filamentDiameter);
    SETTING(filamentFlow);
    SETTING(layer0extrusionWidth);
    SETTING(extrusionWidth);
    SETTING(insetCount);
    SETTING(downSkinCount);
    SETTING(upSkinCount);
    SETTING(sparseInfillLineDistance);
    SETTING(infillOverlap);
    SETTING(skirtDistance);
    SETTING(skirtLineCount);
    SETTING(skirtMinLength);

    SETTING(retractionAmount);
    SETTING(retractionAmountPrime);
    SETTING(retractionAmountExtruderSwitch);
    SETTING(retractionSpeed);
    SETTING(retractionMinimalDistance);
    SETTING(minimalExtrusionBeforeRetraction);
    SETTING(retractionZHop);

    SETTING(enableCombing);
    SETTING(enableOozeShield);
    SETTING(wipeTowerSize);
    SETTING(multiVolumeOverlap);
    
    SETTING(initialSpeedupLayers);
    SETTING(initialLayerSpeed);
    SETTING(printSpeed);
    SETTING(infillSpeed);
    SETTING(inset0Speed);
    SETTING(insetXSpeed);
    SETTING(moveSpeed);
    SETTING(fanFullOnLayerNr);

    SETTING(supportType);
    SETTING(supportAngle);
    SETTING(supportEverywhere);
    SETTING(supportLineDistance);
    SETTING(supportXYDistance);
    SETTING(supportZDistance);
    SETTING(supportExtruder);

    SETTING(minimalLayerTime);
    SETTING(minimalFeedrate);
    SETTING(coolHeadLift);
    SETTING(fanSpeedMin);
    SETTING(fanSpeedMax);

    SETTING(raftMargin);
    SETTING(raftLineSpacing);
    SETTING(raftBaseThickness);
    SETTING(raftBaseLinewidth);
    SETTING(raftBaseSpeed);
    SETTING(raftInterfaceThickness);
    SETTING(raftInterfaceLinewidth);
    SETTING(raftInterfaceLineSpacing);
    SETTING(raftFanSpeed);
    SETTING(raftSurfaceThickness);
    SETTING(raftSurfaceLinewidth);
    SETTING(raftSurfaceLineSpacing);
    SETTING(raftSurfaceLayers);
    SETTING(raftSurfaceSpeed);
    SETTING(raftAirGap);

    SETTING2(objectPosition.X, posx);
    SETTING2(objectPosition.Y, posy);
    SETTING(objectSink);

    SETTING(fixHorrible);
    SETTING(spiralizeMode);
    SETTING(gcodeFlavor);

    memset(extruderOffset, 0, sizeof(extruderOffset));
    SETTING_VAL(extruderOffset[1].X, 0);
    SETTING_VAL(extruderOffset[1].Y, 0);
    SETTING_VAL(extruderOffset[2].X, 0);
    SETTING_VAL(extruderOffset[2].Y, 0);
    SETTING_VAL(extruderOffset[3].X, 0);
    SETTING_VAL(extruderOffset[3].Y, 0);

    startCode =
        "M109 S210     ;Heatup to 210C\n"
        "G21           ;metric values\n"
        "G90           ;absolute positioning\n"
        "G28           ;Home\n"
        "G1 Z15.0 F300 ;move the platform down 15mm\n"
        "G92 E0        ;zero the extruded length\n"
        "G1 F200 E5    ;extrude 5mm of feed stock\n"
        "G92 E0        ;zero the extruded length again\n";
    endCode =
        "M104 S0                     ;extruder heater off\n"
        "M140 S0                     ;heated bed heater off (if you have it)\n"
        "G91                            ;relative positioning\n"
        "G1 E-1 F300                    ;retract the filament a bit before lifting the nozzle, to release some of the pressure\n"
        "G1 Z+0.5 E-5 X-20 Y-20 F9000   ;move Z up a bit and retract filament even more\n"
        "G28 X0 Y0                      ;move X/Y to min endstops, so the head is out of the way\n"
        "M84                         ;steppers off\n"
        "G90                         ;absolute positioning\n";
}

#undef STRINGIFY
#undef SETTING

bool ConfigSettings::setSetting(const char* key, const char* value)
{
    for(auto& config : _index)
    {
        if (stringcasecompare(key, config.key) == 0)
        {
            *config.ptr = atoi(value);
            return true;
        }
    }
    if (stringcasecompare(key, "startCode") == 0)
    {
        this->startCode = value;
        return true;
    }
    if (stringcasecompare(key, "endCode") == 0)
    {
        this->endCode = value;
        return true;
    }
    return false;
}

bool ConfigSettings::readSettings(void) {
    return readSettings(DEFAULT_CONFIG_PATH);
}

bool ConfigSettings::readSettings(const char* path) {
    std::ifstream config(path);
    std::string line;
    size_t line_number = 0;

    if(!config.good()) return false;

    while(config.good()) {
        size_t pos = std::string::npos;
        std::getline(config, line);
        line_number += 1;

        // De-comment and trim, skipping anything that shows up empty
        pos = line.find_first_of('#');
        if(pos != std::string::npos) line.erase(pos);
        TRIM_STRING(line);
        if(line.length() == 0) continue;

        // Split into key = val
        std::string key(""), val("");
        pos = line.find_first_of('=');
        if(pos != std::string::npos && line.length() > (pos + 1)) {
            key = line.substr(0, pos);
            val = line.substr(pos + 1);
            TRIM_STRING(key);
            TRIM_STRING(val);
        }

        // Are we about to read a multiline string?
        if(val == CONFIG_MULTILINE_SEPARATOR) {
            val = "";
            bool done_multiline = false;

            while(config.good() && !done_multiline) {
                std::getline(config, line);
                line_number += 1;

                // We RTRIM the line for two reasons:
                //
                // 1) Make sure that a direct == comparison with '"""' works without
                //    worrying about trailing space.
                // 2) Nobody likes trailing whitespace anyway
                RTRIM_STRING(line);

                // Either accumuliate or terminate
                if(line == CONFIG_MULTILINE_SEPARATOR) {
                    done_multiline = true;
                    // Make sure we don't add an extra trailing newline
                    // to the parsed value
                    RTRIM_STRING(val);
                }
                else {
                    line += "\n";
                    val += line;
                }
            }

            // If we drop out but didn't finish reading, something failed
            if(!done_multiline) {
                cura::logError("Config(%s):L%zd: Failed while reading multiline string.\n", path, line_number);
                return false;
            }

        }

        // Fail if we don't get a key and val
        if(key.length() == 0 || val.length() == 0) {
            cura::logError("Config(%s): Line %zd: No key value pair found\n", path, line_number);
            return false;
        }

        // Set a config setting for the current K=V
        if(!setSetting(key.c_str(), val.c_str())) {
            cura::logError("Config(%s):L%zd: Failed to set '%s' to '%s'\n", path, line_number, key.c_str(), val.c_str());
            return false;
        }
    }

    return true;
}
