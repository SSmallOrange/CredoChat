#include "mainwindow.h"

#include <QFile>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    QFile qss(":/style/stylesheet.qss");
    if (qss.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qDebug("%s", style.toStdString().c_str());
        qss.close();
    } else {
        qDebug("qss error");
    }

    w.show();
    return a.exec();
}
