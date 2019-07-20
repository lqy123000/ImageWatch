#ifndef IMAGEWATCH_H
#define IMAGEWATCH_H

#include <QtWidgets/QWidget>
#include "ui_ImageWatch.h"
#include <QStringList>
#include <QGraphicsScene>

class ImageWatch : public QWidget
{
	Q_OBJECT

public:
	ImageWatch(QWidget *parent = 0);
	~ImageWatch();

private slots:
	void on_begin();
	void on_prev();
	void on_next();
	void on_changeIndex();

private:
	void showImage();

private:
	Ui::ImageWatchClass ui;
	QGraphicsScene *m_scene;		//³¡¾°£¨»­²¼£©
	QString m_baseDir;
	QStringList m_filenames;
	QStringList m_nameFilters;
	int m_index;
	int m_count;
};

#endif // IMAGEWATCH_H
