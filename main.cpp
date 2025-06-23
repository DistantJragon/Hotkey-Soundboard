#include "hotkeysoundboard.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HotkeySoundboard w;
    w.show();
    return a.exec();
}
