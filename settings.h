#ifndef SETTING_H
#define SETTING_H

#include <QSettings>

class Settings
{
public:
    Settings();
    void updateFolder(const QString& newValue) {mFolder=newValue;}
    void updateExifTool(const QString& newValue) {mExifTool=newValue;}
    const QString& getFolder() const { return(mFolder);}
    const QString& getExifTool() const { return(mExifTool);}
    bool areSettingsDirty() const;
    bool saveSettings(bool bForced=false);
    bool loadSettings();
private:
    QSettings mSettings;
    QString mFolder, mFolder0;
    QString mExifTool, mExiftool0;
};

#endif // SETTING_H
