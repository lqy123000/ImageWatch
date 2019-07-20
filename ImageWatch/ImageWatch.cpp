#include "ImageWatch.h"
#include <QDir>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>

char* SUFFIX_LIST[] = { ".jpg", ".png", ".gif", ".bmp", ".jpeg", ".jpe", ".jfif", "tif", "tiff"};

ImageWatch::ImageWatch(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.begin, &QPushButton::clicked, this, &ImageWatch::on_begin);
	connect(ui.prev, &QPushButton::clicked, this, &ImageWatch::on_prev);
	connect(ui.next, &QPushButton::clicked, this, &ImageWatch::on_next);
	connect(ui.index, &QSpinBox::editingFinished, this, &ImageWatch::on_changeIndex);

	m_scene = new QGraphicsScene();
	ui.view->setScene(m_scene);  // 自定义的view，可以用QGraphicsView代替

	ui.base_dir->setText(QDir::currentPath());  // 默认当前目录

	int suffix_num = sizeof(SUFFIX_LIST) / sizeof(SUFFIX_LIST[0]);
	for (int i = 0; i < suffix_num; i++)
	{
		m_nameFilters << QString("*%1").arg(SUFFIX_LIST[i]);  // 不区分大小写
	}

	this->showMaximized();
}

ImageWatch::~ImageWatch()
{
	delete m_scene;
}

QStringList getFileList(const QString& dirPath, const QStringList nameFilters, bool recursive, QString prefix = "")
{
	QDir dir(dirPath);
	QStringList fileNames = dir.entryList(nameFilters, QDir::Files | QDir::NoSymLinks);
	for (auto &fileName : fileNames)
	{
		fileName = prefix + fileName;
	}
	if (recursive)
	{
		QStringList subDirNames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (auto &subDirName : subDirNames)
		{
			QString subDirPath = dir.absoluteFilePath(subDirName);
			QStringList names = getFileList(subDirPath, nameFilters, true, prefix + subDirName + '/');
			fileNames += names;
		}
	}
	return fileNames;
}

void ImageWatch::on_begin()
{
	m_baseDir = ui.base_dir->text();
	// TODO: 1.加入多线程避免文件太多导致界面卡住 2.加入取消操作 3. 可配置是否递归搜索
	m_filenames = getFileList(m_baseDir, m_nameFilters, true, "\\");
	m_count = m_filenames.size();
	m_index = 0;
	if (m_count > 0)
	{
		showImage();
	}

	ui.total_count->setText(QString("/%1").arg(m_count));
	ui.index->setMinimum(1);
	ui.index->setMaximum(m_count);
}

void ImageWatch::on_prev()
{
	if (m_count <= 1)
	{
		return;
	}

	if (m_index <= 0)
	{
		m_index = m_count - 1;
	}
	else
	{
		m_index--;
	}
	showImage();
}

void ImageWatch::on_next()
{
	if (m_count <= 1)
	{
		return;
	}

	if (m_index >= m_count - 1)
	{
		m_index = 0;
	}
	else
	{
		m_index++;
	}
	showImage();
}

void ImageWatch::on_changeIndex()
{
	if (m_count <= 1)
	{
		return;
	}

	m_index = ui.index->value() - 1;
	showImage();
}

void ImageWatch::showImage()
{
	if (m_index < 0 || m_index >= m_count)
	{
		QMessageBox::warning(this, QStringLiteral("Image watch"), QStringLiteral("index error"));
		return;
	}

	ui.path->setText(m_filenames[m_index]);
	ui.index->setValue(m_index+1);
	m_scene->clear();

	QString imagePath = m_baseDir + m_filenames[m_index];
	QFile file(imagePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, QStringLiteral("Image watch"), QStringLiteral("file open error"));
		return;
	}
	QPixmap pixmap;
	pixmap.loadFromData(file.readAll());  // 直接给路径QT会根据后缀名推导导致后缀不一致的失败，因此只好先加载到内存
	m_scene->addPixmap(pixmap);
	m_scene->setSceneRect(pixmap.rect());
}