#include "mainwindow.h"
#include <QApplication>

/*
 * exiftool -GPSLongitudeRef="1.848951"  -GPSLatitudeRef="42.764256" Caussou-2016-08-06.jpg
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
