#include "mainwindow.h"
#include <QApplication>

/*
 * exiftool -GPSLongitude="1.848951"  -GPSLatitude="42.764256" Caussou-2016-08-06.jpg
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
