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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_pathToExiftool_textChanged(const QString &arg1);

    void on_pushBtnFiles_clicked();

    void on_pushBtnFolder_clicked();

    void on_picturesFolder_textChanged(const QString &arg1);

    void on_gpsCoodinates_textChanged(const QString &arg1);

    bool coordinatesAreOkay() const;

    void on_pictures_textChanged();

    void on_pushBtnGo_clicked();

    void on_pushClearFiles_clicked();

private:
    Ui::MainWindow *ui;
    Settings mSettings;
    double mLattitude;
    double mLongitude;
    bool mCoordinatesAreOkay;
    bool mFileListIsOkay;
    const QString mcOkayStyle;
    const QString mcErrorStyle;

    bool manageGoPushButton();

    static QString& quoteQString(QString&);

};

#endif // MAINWINDOW_H
