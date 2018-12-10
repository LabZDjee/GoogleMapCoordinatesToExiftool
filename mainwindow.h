
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pathToExiftool_textChanged(const QString &arg1);

    void on_pushBtnFiles_clicked();

    void on_pushBtnFolder_clicked();

    void on_picturesFolder_textChanged(const QString &arg1);

    void on_gpsCoodinates_textChanged(const QString &arg1);

    bool coordinatesAreOkay() const;

    bool altitudeIsOkay() const;

    void on_pictures_textChanged();

    void on_pushBtnGo_clicked();

    void on_pushClearFiles_clicked();

    void on_pushButtonQuit_clicked();

    void on_pushGotoWebsite_clicked();

    void on_coordinateWebsite_textChanged(const QString &arg1);

    void on_checkOverwrite_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    Settings mSettings;
    double mLattitude;
    double mLongitude;
    double mAltitude;
    bool mCoordinatesAreOkay;
    bool mOverwriteOriginal;
    bool mFileListIsOkay;
    const QString mcOkayStyle;
    const QString mcErrorStyle;
    QStringList* mFileList;

    bool manageGoPushButton();

    static QString& quoteQString(QString&);

};

#endif // MAINWINDOW_H
