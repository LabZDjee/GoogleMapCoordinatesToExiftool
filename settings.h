#ifndef SETTING_H
#define SETTING_H

#include <QSettings>

class Settings
{
public:
    Settings();
    void updateFolder(const QString& newValue) {mFolder=newValue;}
    void updateExifTool(const QString& newValue) {mExifTool=newValue;}
    void updateWebsite(const QString& newValue) {mWebsite=newValue;}
    void updateOverwriteOriginal(bool newValue) {mOverwriteOriginal=newValue;}
    const QString& getFolder() const { return(mFolder);}
    const QString& getExifTool() const { return(mExifTool);}
    const QString& getWebsite() const { return(mWebsite);}
    bool getOverwriteOriginal() const { return(mOverwriteOriginal);}
    bool areSettingsDirty() const;
    bool saveSettings(bool bForced=false);
    bool loadSettings();
private:
    QSettings mSettings;
    QString mFolder, mFolder0;
    QString mExifTool, mExiftool0;
    QString mWebsite, mWebsite0;
    bool mOverwriteOriginal, mOverwriteOriginal0;
 };

#endif // SETTING_H
