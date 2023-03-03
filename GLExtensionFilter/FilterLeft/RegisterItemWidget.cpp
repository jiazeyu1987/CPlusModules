#include "RegisterItemWidget.h"
#include <QDebug>
#include <QMessageBox>
 
RegisterItemWidget::RegisterItemWidget(QWidget *parent)
	: QWidget(parent)
{
    ui.setupUi(this);

	SetState(0);
	// 连接信号槽
	connect(ui.zlabel, SIGNAL(clicked()), this, SLOT(dosomething()));
	//connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(dosomething()));
}

void RegisterItemWidget::SetState(int state1) {
	state = state1;
}

void RegisterItemWidget::SetName(QString name1) {
	m_name = name1;
	ui.lblText->setText(name1);

}

void RegisterItemWidget::SetImage(QString path) {
	image_path = path;
	QImage *img = new QImage; //新建一个image对象
	ui.zlabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui.zlabel->setScaledContents(true);
	img->load(path); //将图像资源载入对象img，注意路径，可点进图片右键复制路径
	ui.zlabel->setPixmap(QPixmap::fromImage(*img)); //将图片放入label，使用setPixmap,注意指针*img

}

void RegisterItemWidget::SetFrontImage(QString path) {
	QImage *img = new QImage; //新建一个image对象
	ui.zlabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui.zlabel->setScaledContents(true);
	img->load(path); //将图像资源载入对象img，注意路径，可点进图片右键复制路径
	ui.zlabel->setPixmap(QPixmap::fromImage(*img)); //将图片放入label，使用setPixmap,注意指针*img

}

void RegisterItemWidget::SetNode(vtkMRMLScalarVolumeNode* in_node) {
	m_Node = in_node;
	char* name = m_Node->GetName();
	ui.lblText->setText(QString::fromStdString(std::string(name)));
}





void RegisterItemWidget::mouseReleaseEvent(QMouseEvent *event) {
}


void RegisterItemWidget::onToggled(bool bChecked)
{
}

void RegisterItemWidget::dosomething()
{
}
