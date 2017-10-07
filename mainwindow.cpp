#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>

/*
 * QProcess int QProcess::execute(const QString &command) (static)
 * exiftool -GPSLongitude="1.848951"  -GPSLatitude="42.764256" Caussou-2016-08-06.jpg
 *  43°19'47.4"N 0°45'37.6"W
 *  43.329833, -0.760444
 *   (-?\d{1,3}\.\d*)[^-0-9]+(-?\d{1,3}\.\d*)
 *   (\d{1,3})°(\d{1,2})'(\d{1,2}\.?\d*)([nNsS])[^-0-9]+(\d{1,3})°(\d{1,2})'(\d{1,2}\.?\d*)([eEwW])
 * https://doc-snapshots.qt.io/qt5-5.9/qregexp.html
 */

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
    ui->gpsCoodinates->setStyleSheet("QWidget " + mcErrorStyle);
    ui->pictures->setStyleSheet("QWidget " + mcErrorStyle);
    setWindowTitle("Google Map GPS Coordinates to Pictures");
}

MainWindow::~MainWindow()
{
    mSettings.saveSettings(false);
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    close();
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
     dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg);;All (*)"));
     if (dialog.exec()!=QDialog::Accepted)
      return;
     QStringList fileNames = dialog.selectedFiles();
     ui->pictures->setText(fileNames.join(QChar::LineFeed));
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
    QStringList::Iterator it = regExp.capturedTexts().begin();
    for (; it != regExp.capturedTexts().end(); it++) {
      if(it->length()==0)
       return(false);
     }
    return(true);
}

void MainWindow::on_gpsCoodinates_textChanged(const QString &text)
{
    QRegExp rxDecimal("^[^0-9.-]*(-?\\d*(?:\\.\\d*)?)[^0-9.-]+(-?\\d*(?:\\.\\d*)?)\\D*$");
    QRegExp rxSexagesimal("\\D*(\\d+) *° *(\\d+) *' *(\\d*(?:\\.\\d*)?) *\" *([nNsS])"
                          "\\D*(\\d+) *° *(\\d+) *' *(\\d*(?:\\.\\d*)?) *\" *([eEwW])");
    mLattitude = mLongitude = 1e99;
    if (rxSexagesimal.indexIn(text)>-1 /*&& capturedQRegsNotEmpty(rxSexagesimal)*/) {
        mLattitude = rxSexagesimal.cap(1).toDouble();
        mLattitude += rxSexagesimal.cap(2).toDouble() / 60.0;
        mLattitude += rxSexagesimal.cap(3).toDouble() / 3600.0;
        if(QString("sS").indexOf(rxSexagesimal.cap(4)[0])>-1)
         mLattitude = -mLattitude;
        mLongitude = rxSexagesimal.cap(5).toDouble();
        mLongitude += rxSexagesimal.cap(6).toDouble() / 60.0;
        mLongitude += rxSexagesimal.cap(7).toDouble() / 3600.0;
        if(QString("wW").indexOf(rxSexagesimal.cap(8)[0])>-1)
         mLongitude = -mLongitude;
       }
    else if (rxDecimal.indexIn(text)>-1 && capturedQRegsNotEmpty(rxDecimal)) {
      mLattitude = rxDecimal.cap(1).toDouble();
      mLongitude = rxDecimal.cap(2).toDouble();
     }
    if(coordinatesAreOkay()) {
        ui->gpsCoodinates->setStyleSheet("QLineEdit " + mcOkayStyle);
        QString test = "" +  QString("%1").arg(mLattitude) + " ~ " + QString("%1").arg(mLongitude);
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
    return(mLattitude >= -360.0 && mLattitude<=360.0 &&
           mLongitude >= -360.0 && mLongitude<=360.0);
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
    QString command = QString("%1 -GPSLongitude=\"%2\" -GPSLatitude=\"%3\"")
            .arg(quoteQString(app))
            .arg(mLongitude)
            .arg(mLattitude);
    QStringList fileList = ui->pictures->toPlainText().split(QChar::LineFeed);
    for(int i=0; i<fileList.length(); i++) {
        fileList[i]=fileList[i].trimmed();
        if(fileList[i].length()>0) {
            command += " " + quoteQString(fileList[i]);
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
          //int QProcess::execute(command)
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
    ui->pictures->clear();
}
