#include <QtGui/QApplication>
#include "errifiergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ErrifierGUI w;
    w.show();
    
    return a.exec();
}
