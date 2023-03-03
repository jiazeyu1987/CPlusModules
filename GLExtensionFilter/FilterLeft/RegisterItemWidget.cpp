#include "RegisterItemWidget.h"
#include <QDebug>
#include <QMessageBox>
 
RegisterItemWidget::RegisterItemWidget(QWidget *parent)
	: QWidget(parent)
{
    ui.setupUi(this);

	SetState(0);
	// �����źŲ�
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
	QImage *img = new QImage; //�½�һ��image����
	ui.zlabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui.zlabel->setScaledContents(true);
	img->load(path); //��ͼ����Դ�������img��ע��·�����ɵ��ͼƬ�Ҽ�����·��
	ui.zlabel->setPixmap(QPixmap::fromImage(*img)); //��ͼƬ����label��ʹ��setPixmap,ע��ָ��*img

}

void RegisterItemWidget::SetFrontImage(QString path) {
	QImage *img = new QImage; //�½�һ��image����
	ui.zlabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui.zlabel->setScaledContents(true);
	img->load(path); //��ͼ����Դ�������img��ע��·�����ɵ��ͼƬ�Ҽ�����·��
	ui.zlabel->setPixmap(QPixmap::fromImage(*img)); //��ͼƬ����label��ʹ��setPixmap,ע��ָ��*img

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
