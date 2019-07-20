#include "ImageWatch.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ImageWatch w;
	w.show();
	return a.exec();
}
