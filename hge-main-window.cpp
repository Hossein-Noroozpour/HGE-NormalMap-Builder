#include "hge-main-window.hpp"
#include "ui_hge-main-window.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtDebug>
#include "hge-normal-calculation-algorithm.hpp"
#define NUMBEROFTHREADS 4
MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow),
	hasNotLoadedImage(true)
{
	ui->setupUi(this);
	this->setWindowTitle("Hulixerian Game Engine: Normal Map Creator.");
	connect(ui->loadPB, SIGNAL(clicked()), this, SLOT(onLoadClicked()));
	connect(ui->openPB, SIGNAL(clicked()), this, SLOT(onOpenClicked()));
	connect(ui->savePB, SIGNAL(clicked()), this, SLOT(onSaveClicked()));
	connect(ui->generateNormalPB, SIGNAL(clicked()), this, SLOT(onGenerateClicked()));
}
MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::onLoadClicked()
{
	QString fileName = ui->loadFileLE->text();
	if(fileName.isEmpty())
	{
		QMessageBox::information(this, tr("Image file"), tr("Please specify an image file first."));
		return;
	}
	QImage img(fileName);
	if(img.isNull())
	{
		QMessageBox::information(this, tr("Image Viewer"), tr("Cannot load %1.").arg(fileName));
		return;
	}
	ui->imageArea->setPixmap(QPixmap::fromImage(img));
	hasNotLoadedImage = false;
}
void MainWindow::onOpenClicked()
{
	QFileDialog dialog(this, "Choose image file", ui->loadFileLE->text());
	ui->loadFileLE->setText(dialog.getOpenFileName());
}
void MainWindow::onSaveClicked()
{
	QFileDialog dialog(this, "Save image file", ui->saveFileLE->text());
	QImage img = ui->imageArea->pixmap()->toImage();
	img.save(dialog.getSaveFileName(), "JPG", 100);
}
void MainWindow::onGenerateClicked()
{
	if(hasNotLoadedImage)
	{
		QMessageBox::information(this, tr("There is no image"),
				tr("Please specify an image file first and load it and then click generate."));
		return;
	}
	QImage img = ui->imageArea->pixmap()->toImage();
	int width = img.width();
	int height = img.height();
	int stride = img.bytesPerLine();
	int pixelBytes = stride / width;
	unsigned char *pixels = img.bits();
	//qDebug() << stride << " " << width << " " << height;
	qDebug() << pixelBytes << " ";
	if(pixelBytes * width != stride)
	{
		QMessageBox::information(this, tr("Error in image"),
				tr("Your image has a wrong stride."));
		return;
	}
	for(int w = width; w != 0; w >>= 1)
	{
		if((w & 1) && (w > 1))
		{
			QMessageBox::information(this, tr("Error in image"),
					tr("Your image has a wrong width, it must be power of two."));
			return;
		}
	}
	for(int h = height; h != 0; h >>= 1)
	{
		if((h & 1) && (h > 1))
		{
			QMessageBox::information(this, tr("Error in image"),
					tr("Your image has a wrong height, it must be power of two."));
			return;
		}
	}
	unsigned char *normals = new unsigned char[height * width * 3];
	NormalCalculationAlgorithm** threads = new NormalCalculationAlgorithm*[NUMBEROFTHREADS];
	for(int i = 0; i < NUMBEROFTHREADS; i++)
	{
		threads[i] = new NormalCalculationAlgorithm(
					0, pixels, width, height, pixelBytes, i, NUMBEROFTHREADS, normals);
		threads[i]->start();
	}
	QThread::yieldCurrentThread();
	for(int i = 0; i < NUMBEROFTHREADS; i++)
	{
		threads[i]->wait();
		delete threads[i];
	}
	delete [] threads;
	ui->imageArea->setPixmap(QPixmap::fromImage(QImage(normals, width, height, QImage::Format_RGB888)));
	delete [] normals;
}
