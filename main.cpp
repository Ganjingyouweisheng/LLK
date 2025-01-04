#include "main_game_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainGameWindow w;
    w.show();
    w.setFixedSize(w.geometry().size());
    w.show();


    return a.exec();
}
