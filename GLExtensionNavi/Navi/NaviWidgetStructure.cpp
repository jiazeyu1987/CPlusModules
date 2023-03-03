#include "NaviWidgetStructure.h"
#include <QDebug>
#include <QMessageBox>


NaviWidgetStructure::NaviWidgetStructure(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlag(Qt::FramelessWindowHint); //Òþ²Ø±êÌâ¿ò
	//this->setStyleSheet("QWidget {background-color: #22305b;}");
}

void NaviWidgetStructure::SetName(QString name1) {
	m_Name = name1;
	ui.lblName->setText(name1);
}

void NaviWidgetStructure::SetPath(QString path1, QString alias1) {
	m_Path = path1;

	QImage img;
	//QString qpath = QString::fromStdString(show_path + "\\cover.png");
	img.load(path1 + alias1 + ".png");
	QPixmap pixmap = QPixmap::fromImage(img);
	int w = 76;
	int h = 76;
	QPixmap fitpixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	ui.lblImage->setPixmap(fitpixmap);
}

void NaviWidgetStructure::SelectedStyle(bool selected)
{
	if (selected)
	{
		ui.lblName->setStyleSheet("background: #527FD9;");
	}
	else
	{
		ui.lblName->setStyleSheet("background-color: transparent;");
	}
}
