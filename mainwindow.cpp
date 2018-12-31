#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"

#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegExp>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCoordinatesAreOkay(false),
    mFileListIsOkay(false),
    mcOkayStyle("{background-color: white;}"),
    mcErrorStyle("{background-color: rgb(254, 220, 220);}")
{
    ui->setupUi(this);
    ui->picturesFolder->setText(mSettings.getFolder());
    ui->pathToExiftool->setText(mSettings.getExifTool());
    ui->coordinateWebsite->setText(mSettings.getWebsite());
    ui->checkOverwrite->setCheckState(mSettings.getOverwriteOriginal() ? Qt::Checked : Qt::Unchecked);
    ui->gpsCoodinates->setStyleSheet("QWidget " + mcErrorStyle);
    ui->pictures->setStyleSheet("QWidget " + mcErrorStyle);
    mFileList = new QStringList;
    setWindowTitle("Google Map GPS Coordinates to Pictures");
}

MainWindow::~MainWindow()
{
    mSettings.saveSettings(false);
    delete ui;
}

void MainWindow::on_pathToExiftool_textChanged(const QString &text)
{
    mSettings.updateExifTool(text);
}

void MainWindow::on_pushBtnFiles_clicked()
{
     QFileDialog dialog(this, tr("Open Images"), mSettings.getFolder());
     dialog.setFileMode(QFileDialog::ExistingFiles);
     dialog.setLabelText(QFileDialog::Accept, "Select");
     dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg *.PNG *.JPG *.XPM);;All (*)"));
     if (dialog.exec()!=QDialog::Accepted)
      return;
     delete mFileList;
     mFileList = new QStringList(dialog.selectedFiles());
     QStringList filenameList;
     QString relativeFilePath;
     QDir dir(ui->picturesFolder->text());
     for (QStringList::iterator it = mFileList->begin(); it != mFileList->end(); it++){
         QFileInfo fi(*it);
         relativeFilePath = dir.relativeFilePath(fi.absoluteFilePath());
         filenameList.append(relativeFilePath);
     }
     ui->pictures->setText(filenameList.join(QChar::LineFeed));
}

void MainWindow::on_pushBtnFolder_clicked()
{
    QFileDialog dialog(this, tr("Select Folder"), mSettings.getFolder());
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setLabelText(QFileDialog::Accept, "Select");
    if (dialog.exec()!=QDialog::Accepted)
     return;
    ui->picturesFolder->setText(dialog.selectedFiles()[0]);
}

void MainWindow::on_picturesFolder_textChanged(const QString &text)
{
    mSettings.updateFolder(text);
}

static bool capturedQRegsNotEmpty(const QRegExp& regExp)
{
    const QStringList& list = regExp.capturedTexts();
    int i;
    for(i=0; i<list.length() - 1; i++ ){
        if(list[i].isEmpty()) {
            return false;
        }
    }
    return true;
}

void MainWindow::on_gpsCoodinates_textChanged(const QString &text)
{
    // a number (latitude) a number (longitude), and an optional number (altitude)
    QRegExp rxDecimal("^[^0-9.-]*(-?\\d*(?:\\.\\d*)?)[^0-9.-]+(-?\\d*(?:\\.\\d*)?)(?:[^0-9.-]+(-?\\d*(?:\\.\\d*)?))?\\D*$");
    QRegExp rxSexagesimal("\\D*(\\d+) *° *(\\d+) *' *(\\d*(?:\\.\\d*)?) *\" *([nNsS])"
                          "\\D*(\\d+) *° *(\\d+) *' *(\\d*(?:\\.\\d*)?) *\" *([eEwW])"
                          "(?:[^0-9.-]+(-?\\d*(?:\\.\\d*)?))?\\D*$");
    mLatitude = mLongitude = mAltitude = 1e99;
    if (rxSexagesimal.indexIn(text)>-1 /*&& capturedQRegsNotEmpty(rxSexagesimal)*/) {
        mLatitude = rxSexagesimal.cap(1).toDouble();
        mLatitude += rxSexagesimal.cap(2).toDouble() / 60.0;
        mLatitude += rxSexagesimal.cap(3).toDouble() / 3600.0;
        if(QString("sS").indexOf(rxSexagesimal.cap(4)[0])>-1)
         mLatitude = -mLatitude;
        mLongitude = rxSexagesimal.cap(5).toDouble();
        mLongitude += rxSexagesimal.cap(6).toDouble() / 60.0;
        mLongitude += rxSexagesimal.cap(7).toDouble() / 3600.0;
        if(QString("wW").indexOf(rxSexagesimal.cap(8)[0])>-1)
         mLongitude = -mLongitude;
        if(!rxSexagesimal.cap(9).isEmpty()) {
            mAltitude = rxSexagesimal.cap(9).toDouble();
         }
       }
    else if (rxDecimal.indexIn(text)>-1 && capturedQRegsNotEmpty(rxDecimal)) {
      mLatitude = rxDecimal.cap(1).toDouble();
      mLongitude = rxDecimal.cap(2).toDouble();
      if(!rxDecimal.cap(3).isEmpty()) {
          mAltitude = rxDecimal.cap(3).toDouble();
      }
     }
    if(coordinatesAreOkay()) {
        ui->gpsCoodinates->setStyleSheet("QLineEdit " + mcOkayStyle);
        QString test = "" +  QString("%1").arg(mLatitude) + " ~ " + QString("%1").arg(mLongitude);
        if(altitudeIsOkay()) {
            test += QString(" %1m").arg(mAltitude);
        }
        qDebug() << "on_gpsCoodinates_textChanged: coordinatesAreOkay=" << test;
        mCoordinatesAreOkay = true;
    } else {
        ui->gpsCoodinates->setStyleSheet("QWidget " + mcErrorStyle);
        mCoordinatesAreOkay = false;
        qDebug() << "on_gpsCoodinates_textChanged: coordinatesAreOkay=false";
    }
    manageGoPushButton();
}

bool MainWindow::coordinatesAreOkay() const
{
    return(mLatitude >= -360.0 && mLatitude<=360.0 &&
           mLongitude >= -360.0 && mLongitude<=360.0);
}

bool MainWindow::altitudeIsOkay() const
{
    return(mAltitude>-9999.0 && mAltitude<9999);
}

void MainWindow::on_pictures_textChanged()
{
 mFileListIsOkay = !ui->pictures->toPlainText().isEmpty();
 ui->pictures->setStyleSheet("QWidget " + (mFileListIsOkay ? mcOkayStyle:mcErrorStyle));
 manageGoPushButton();
}

bool MainWindow::manageGoPushButton()
{
    bool enabled=mCoordinatesAreOkay && mFileListIsOkay;
    ui->pushBtnGo->setEnabled(enabled);
    return(enabled);
}

void MainWindow::on_pushBtnGo_clicked()
{
    QString app = ui->pathToExiftool->text();
    QString command = QString("\"%1\" -m -GPSLatitude=%2 -GPSLatitudeRef=%3 -GPSLongitude=%4 -GPSLongitudeRef=%5")
            .arg(quoteQString(app))
            .arg(abs(mLatitude))
            .arg(mLatitude>=0 ? "N" : "S")
            .arg(abs(mLongitude))
            .arg(mLongitude>=0 ? "E" : "W");
    if(altitudeIsOkay()) {
       command += QString(" -GPSAltitude=%1 -GPSAltitudeRef=%2")
               .arg(abs(mAltitude)).arg(mAltitude>=0 ? "above" : "below");
    }
    if(ui->checkOverwrite->isChecked()) {
       command += " -overwrite_original";
    }
    for (QStringList::iterator it = mFileList->begin(); it != mFileList->end(); it++){
        *it=(*it).trimmed();
        if((*it).length()>0) {
            command += " " + quoteQString(*it);
        }
    }
    QMessageBox msgBox;
    msgBox.setText("Patch coordinates in files?");
    msgBox.setDetailedText(command);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Yes:
          qDebug() << "on_pushBtnGo_clicked launches command: " << command;
          QProcess::execute(command);
          break;
      case QMessageBox::No:
          qDebug() << "on_pushBtnGo_clicked with no";
          break;
    }
}

QString& MainWindow::quoteQString(QString& str)
{
    QString str0;
    if(str.indexOf(' ')>-1) {
     str0 = str;
     str = QString("\"%1\"").arg(str0);
    }
    return(str);
}

void MainWindow::on_pushClearFiles_clicked()
{
    delete mFileList;
    mFileList = new QStringList;
    ui->pictures->clear();
}

void MainWindow::on_pushButtonQuit_clicked()
{
    close();
}

void MainWindow::on_pushGotoWebsite_clicked()
{
 QString textualUrl = ui->coordinateWebsite->text();
 QDesktopServices::openUrl(QUrl(textualUrl));
}

void MainWindow::on_coordinateWebsite_textChanged(const QString &text)
{
    mSettings.updateWebsite(text);
}

void MainWindow::on_checkOverwrite_stateChanged(int checked)
{
    mOverwriteOriginal =  checked ? true : false;
    mSettings.updateOverwriteOriginal(mOverwriteOriginal);
}

void MainWindow::on_pushBtnGetCoordFromFile_clicked()
{
    QFileDialog dialog(this, tr("Get GPS Coord. from File"), mSettings.getFolder());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setLabelText(QFileDialog::Accept, "Select");
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg *.PNG *.JPG *.XPM);;All (*)"));
    if (dialog.exec()!=QDialog::Accepted || dialog.selectedFiles().length()<1)
     return;
    QString selectedFile = dialog.selectedFiles().first();
    QString app = ui->pathToExiftool->text();
    QString command = QString("%1 %2 -gps:gpslatitude -gps:gpslatituderef -gps:gpslongitude -gps:gpslongituderef -gps:gpsaltitude")
            .arg(quoteQString(app))
            .arg(quoteQString(selectedFile));
    QProcess process;
    process.start(command);
    process.waitForFinished(-1); // will wait forever until finished
    QString stdOut = process.readAllStandardOutput();
    QRegExp rxLatitude("[Ll]atitude *:? *(\\d+) *deg *(\\d+) *' *(\\d*(?:\\.\\d*)?) *\".*((?:North)|(?:South))");
    QString latitude="undefined";
    if (rxLatitude.indexIn(stdOut)>-1) {
        latitude = QString("%1°%2'%3\"%4")
                .arg(rxLatitude.cap(1)).arg(rxLatitude.cap(2))
                .arg(rxLatitude.cap(3)).arg(rxLatitude.cap(4)[0]);
    }
    QRegExp rxLongitude("[Ll]ongitude *:? *(\\d+) *deg *(\\d+) *' *(\\d*(?:\\.\\d*)?) *\".*((?:East)|(?:West))");
    QString longitude="undefined";
    if (rxLongitude.indexIn(stdOut)>-1) {
        longitude = QString("%1°%2'%3\"%4")
                .arg(rxLongitude.cap(1)).arg(rxLongitude.cap(2))
                .arg(rxLongitude.cap(3)).arg(rxLongitude.cap(4)[0]);
    }
    QRegExp rxAltitude("[Aa]ltitude *:? *(\\d*(?:\\.\\d*)?) *m[\r\n]");
    QString altitude = "undefined";
    if (rxAltitude.indexIn(stdOut)>-1) {
        altitude = QString("%1").arg(rxAltitude.cap(1));
    }
    if (latitude=="undefined" || longitude=="undefined") {
        QMessageBox msgBox;
        msgBox.setText(QString("Cannot find any coordinates in '%1'").arg(selectedFile));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        ui->gpsCoodinates->setText("");
        return;
    }
    QString coordinates;
    coordinates = QString("%1, %2").arg(latitude).arg(longitude);
    if(altitude != "undefined") {
        coordinates += QString(", %1").arg(altitude);
    }
    ui->gpsCoodinates->setText(coordinates);
}
