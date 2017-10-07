#include "settings.h"

Settings::Settings() :
    mSettings("LabZ", "GPS-Coordinates-to-Pictures")
{
    loadSettings();
}

bool Settings::areSettingsDirty() const
{
    return(mFolder!=mFolder0 || mExifTool!=mExiftool0);
}
bool Settings::saveSettings(bool bForced)
{
 bool canDoIt = bForced || areSettingsDirty();
 if(canDoIt) {
     mSettings.beginGroup("main");
     mSettings.setValue("exiftool", mExifTool);
     mSettings.setValue("folder", mFolder);
     mSettings.endGroup();
 }
 return(canDoIt);
}
bool Settings::loadSettings()
{
    mSettings.beginGroup("main");
    mFolder = mSettings.value("folder", ".").toString();
    mExifTool = mSettings.value("exiftool", "exiftool").toString();
    mSettings.endGroup();
    mFolder0=mFolder;
    mExiftool0=mExifTool;
    return(true);
}
