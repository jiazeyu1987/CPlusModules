#include "NaviWidgetPhase.h"
#include <QDebug>
#include <QMessageBox>
#include "QFileInfo.h"

NaviWidgetPhase::NaviWidgetPhase(QWidget *parent)
    : QWidget(parent)
{ 
	ui.setupUi(this);

	setWindowFlag(Qt::FramelessWindowHint); //Òþ²Ø±êÌâ¿ò

}

void NaviWidgetPhase::SetMyNode(vtkMRMLScalarVolumeNode* in_node) {
	m_Node = in_node;
	const char* alias_name = in_node->GetAttribute("alias_name");
	if (alias_name) {
		ui.lblName->setText(QString::fromStdString(std::string(alias_name)));
	}else
	ui.lblName->setText(QString::fromStdString(std::string(m_Node->GetName())));
}

vtkMRMLScalarVolumeNode* NaviWidgetPhase::GetMyNode() {
	return m_Node;
}

void NaviWidgetPhase::SetImage(const char* imagePath, int width, int height ,const char* extra_image_path)
{
	QFileInfo fileInfo(QString::fromLatin1(imagePath));
	if (fileInfo.isFile()) {
		QImage* img = new QImage();
		img->load(imagePath);
		QPixmap pixmap = QPixmap::fromImage(*img);
		QPixmap fitpixmap = pixmap.scaled(ui.lblImage->size());
		ui.lblImage->setScaledContents(true);
		ui.lblImage->setPixmap(pixmap);
	}
	else if(extra_image_path){
		auto applicationPath = QApplication::applicationDirPath();
		auto picpath = applicationPath + extra_image_path;
		QImage* img = new QImage();
		img->load(picpath);
		QPixmap pixmap = QPixmap::fromImage(*img);
		QPixmap fitpixmap = pixmap.scaled(ui.lblImage->size());
		ui.lblImage->setScaledContents(true);
		ui.lblImage->setPixmap(pixmap);
	}
}

void NaviWidgetPhase::SelectedStyle(bool selected)
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