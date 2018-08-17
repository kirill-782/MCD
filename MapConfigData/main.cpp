#include "mapconfigdata.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MapConfigData w;
	w.show();
	return a.exec();
}
