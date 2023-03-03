#include "BuildPhaseStructrueView.h"
#include <QDebug>
#include <QMessageBox>



BuildPhaseStructrueView::BuildPhaseStructrueView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlag(Qt::FramelessWindowHint); //Òþ²Ø±êÌâ¿ò
	this->setStyleSheet("QWidget {background-color: #22305b;}");
}

void BuildPhaseStructrueView::SetName(QString name1) {
	m_Name = name1;
	ui.lblName->setText(name1);
}

void BuildPhaseStructrueView::SetPath(QString path1, QString alias1) {
	m_Path = path1;

	QImage img;
	//QString qpath = QString::fromStdString(show_path + "\\cover.png");
	img.load(path1 + alias1 + ".png");
	QPixmap pixmap = QPixmap::fromImage(img);
	int w = 96;
	int h = 96;
	QPixmap fitpixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	ui.lblImage->setPixmap(fitpixmap);
}