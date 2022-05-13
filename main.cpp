#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    a.setApplicationName("TaskBook");
    w.setWindowTitle("TaskBook");
    a.setWindowIcon(QIcon("://icon.ico"));
    w.show();
    return a.exec();
}
