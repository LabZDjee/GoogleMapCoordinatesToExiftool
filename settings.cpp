#include "settings.h"

// in $HOME.config/<org>/<app>.conf
Settings::Settings() :
    mSettings("LabZ", "GPS-Coordinates-to-Pictures")
{
    loadSettings();
}

bool Settings::areSettingsDirty() const
{
    return(mFolder!=mFolder0 || mExifTool!=mExiftool0 || mWebsite!=mWebsite0 || mOverwriteOriginal!=mOverwriteOriginal0);
}
bool Settings::saveSettings(bool bForced)
{
 bool doIt = bForced || areSettingsDirty();
 if(doIt) {
     mSettings.beginGroup("main");
     mSettings.setValue("exiftool", mExifTool);
     mSettings.setValue("folder", mFolder);
     mSettings.setValue("website", mWebsite);
     mSettings.setValue("overwriteOriginal", mOverwriteOriginal);
     mSettings.endGroup();
 }
 return(doIt);
}
bool Settings::loadSettings()
{
    mSettings.beginGroup("main");
    mFolder = mSettings.value("folder", ".").toString();
    mExifTool = mSettings.value("exiftool", "exiftool").toString();
    mWebsite = mSettings.value("website", "https://www.google.com/maps").toString();
    mOverwriteOriginal = mSettings.value("overwriteOriginal", "true").toBool();
    mSettings.endGroup();
    mFolder0=mFolder;
    mExiftool0=mExifTool;
    mWebsite0 = mWebsite;
    mOverwriteOriginal0 = mOverwriteOriginal;
    return(true);
}
