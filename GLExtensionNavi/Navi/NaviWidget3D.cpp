#include "NaviWidget3D.h"

#include <QDebug>
#include <QMessageBox>
#include <qstyle.h>
#include "vtkMRMLSegmentationNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "qGLMessageDialog.h"
const std::string NaviWidget3D::visible_2d = "visible_2d";
const std::string NaviWidget3D::visible_3d = "visible_3d";
const std::string NaviWidget3D::opacity_all = "opacity_all";
const std::string NaviWidget3D::opacity_2D = "opacity_2D";
const std::string NaviWidget3D::opacity_3D = "opacity_3D";
const std::string NaviWidget3D::color_red = "color_red";
const std::string NaviWidget3D::color_blue = "color_blue";
const std::string NaviWidget3D::color_green = "color_green";
const std::string NaviWidget3D::structure_name = "structure_name";
const std::string NaviWidget3D::master_node_id = "master_node_id";
NaviWidget3D::NaviWidget3D(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	init();
	
}



NaviWidget3D::NaviWidget3D(Widget3DData* m_data,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init();
	
}

void NaviWidget3D::init() {
	m_Data = new Widget3DData();
	//m_Data->m_opacity = ui.horizontalSlider->maximum();
	m_Data->m_opacity2D = ui.horizontalSlider2D->maximum();
	m_Data->m_opacity3D = ui.horizontalSlider3D->maximum();
	//ui.horizontalSlider->setValue(ui.horizontalSlider->maximum());
	//connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(OnOpacityValueChanged(int)));
	connect(ui.horizontalSlider2D, SIGNAL(valueChanged(int)), this, SLOT(OnOpacityValueChanged2D(int)));
	connect(ui.horizontalSlider3D, SIGNAL(valueChanged(int)), this, SLOT(OnOpacityValueChanged3D(int)));
	ui.horizontalSlider2D->setValue(100);
	ui.horizontalSlider3D->setValue(100);
	connect(ui.btn2D, SIGNAL(toggled(bool)), this, SLOT(onVisibleChanged(bool)));
	connect(ui.btn3D, SIGNAL(toggled(bool)), this, SLOT(on3DVisibleChanged(bool)));
	connect(ui.btnDelete, SIGNAL(clicked()), this, SLOT(onDelete3DItem()));
	connect(ui.btnModify, SIGNAL(clicked()), this, SLOT(onModify3DItem()));
	connect(ui.btnPalette, SIGNAL(clicked()), this, SLOT(onOpenColorPad()));
	
}

void NaviWidget3D::ChangeVisible2DState(bool b) {
	onVisibleChanged(b);
}

void NaviWidget3D::ChangeVisible3DState(bool b) {
	on3DVisibleChanged(b);
}

void NaviWidget3D::onModify3DItem() {
	
	if (ui.lblName->text() == QStringLiteral("大脑")) {
		QString tips = QStringLiteral("修改大脑模型将导致[去除颅骨]结果发生变化,是否进行?");
		QStringList tmpList = { QStringLiteral("确定"), QStringLiteral("取消") };
		MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("删除提示"), tips, MessageButtonType::BUTTON_OK_AND_CANCEL, tmpList, true);
		if (res == MessageResult::CANCEL)
		{
			return;
		}
	}

	if (ui.lblName->text() == QStringLiteral("头架")) {
		QString tips = QStringLiteral("头架模型无法进行修改");
		qGLMessageDialog::ShowWarnning(QStringLiteral("删除提示"), tips,  true);
		return;
	}
	
	emit(modifyWidget3D(this));
}

void NaviWidget3D::SetMasterNode(vtkMRMLScalarVolumeNode* node) {
	m_Data->m_MasterNode = node;
	SetAttribute(NaviWidget3D::master_node_id.c_str(), node->GetID());
}

void NaviWidget3D::onDelete3DItem() {
	QString tips = QStringLiteral("是否删除该模型?");
	if (ui.lblName->text() == QStringLiteral("大脑")) {
		tips = QStringLiteral("删除大脑模型将导致无法进行[颅骨去除]操作,是否继续删除?");
	}
	QStringList tmpList = { QStringLiteral("确定"), QStringLiteral("取消") };
	MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("删除提示"), tips, MessageButtonType::BUTTON_OK_AND_CANCEL, tmpList, true);
	if (res == MessageResult::CANCEL)
	{
		return;
	}
	emit(removeWidget3D(this));
}

void NaviWidget3D::OnOpacityValueChanged(int value) {
	m_Data->m_opacity = value;
	if(GetDisplayNode())
	GetDisplayNode()->SetOpacity(value/100.0);
	SetAttribute(NaviWidget3D::opacity_all.c_str(), QStringLiteral("%1").arg(value).toStdString().c_str());
}

void NaviWidget3D::OnOpacityValueChanged2D(int value) {
	m_Data->m_opacity2D = value;
	if (GetDisplayNode())
		GetDisplayNode()->SetOpacity2DFill(value / 100.0);
	SetAttribute(NaviWidget3D::opacity_2D.c_str(), QStringLiteral("%1").arg(value).toStdString().c_str());
}

void NaviWidget3D::OnOpacityValueChanged3D(int value) {
	m_Data->m_opacity3D = value;
	if (!GetDisplayNode()) {
		return;
	}
	GetDisplayNode()->SetOpacity3D(value / 100.0);
	SetAttribute(NaviWidget3D::opacity_3D.c_str(), QStringLiteral("%1").arg(value).toStdString().c_str());
}

void NaviWidget3D::SetAttribute(const char* key, const char* val) {
	if (m_Data) {
		if (m_Data->m_SegmentationNode) {
			m_Data->m_SegmentationNode->SetAttribute(key, val);
		}
	}
}

void NaviWidget3D::onVisibleChanged(bool b) {
	if (!GetDisplayNode()) {
		return;
	}
	if (b) // "选中"
	{
		GetDisplayNode()->SetVisibility2D(true);
		SetAttribute(NaviWidget3D::visible_2d.c_str(), "true");
		//ui.btn2D->setStyleSheet("background-color: rgb(255, 66, 69);");
	}
	else {
		GetDisplayNode()->SetVisibility2D(false);
		SetAttribute(NaviWidget3D::visible_2d.c_str(), "false");
		//ui.btn2D->setStyleSheet("background-color: rgb(36, 46, 66);");
	}
}

void NaviWidget3D::OnFresh2DFromDisplayNode() {
	auto vis = GetDisplayNode()->GetVisibility2D()&& GetDisplayNode()->GetVisibility();
	if (vis) {
		ui.btn2D->setChecked(true);
	}
	else {
		ui.btn2D->setChecked(false);
	}
}

void NaviWidget3D::OnFresh3DFromDisplayNode() {
	auto vis = GetDisplayNode()->GetVisibility3D() && GetDisplayNode()->GetVisibility();
	if (vis) {
		ui.btn3D->setChecked(true);
	}
	else {
		ui.btn3D->setChecked(false);
	}
}

vtkMRMLSegmentationDisplayNode* NaviWidget3D::GetDisplayNode() {
	if (!m_Data->m_SegmentationNode) {
		return nullptr;
	}
	auto segmentDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(m_Data->m_SegmentationNode->GetDisplayNode());
	if (segmentDisplayNode) {
		m_Data->m_SegmentationNode->CreateDefaultDisplayNodes();
		segmentDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(m_Data->m_SegmentationNode->GetDisplayNode());
	}
	return segmentDisplayNode;
}

void NaviWidget3D::on3DVisibleChanged(bool b) {
	if (!GetDisplayNode()) {
		return;
	}
	if (b) // "选中"
	{
		GetDisplayNode()->SetVisibility3D(true);
		SetAttribute(NaviWidget3D::visible_3d.c_str(), "true");
		//ui.btn3D->setStyleSheet("background-color: rgb(255, 66, 69);");
	}
	else {
		GetDisplayNode()->SetVisibility3D(false);
		SetAttribute(NaviWidget3D::visible_3d.c_str(), "false");
		//ui.btn3D->setStyleSheet("background-color: rgb(36, 46, 66);");
	}
}



void NaviWidget3D::onOpenColorPad()
{
	m_Data->m_pColorDialog = new QColorDialog;
	
	connect(m_Data->m_pColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(onGetColor(QColor)));
	m_Data->m_myPalette = ui.btnPalette->palette(); //按钮的调色板
	m_Data->m_pColorDialog->exec();
}

void NaviWidget3D::SetSegmentInfo(vtkSegment* in_seg, std::string in_id, vtkMRMLSegmentationNode* snode){
	m_Data->segmentID = in_id;
	m_Data->segment = in_seg;	
	m_Data->m_SegmentationNode = snode;
	GetDisplayNode()->SetVisibility(true);
	SetTitle(m_Data->structure_label);
	auto color = m_Data->segment->GetColor();
	QColor color1;
	color1.setRed(color[0] * 255);
	color1.setBlue(color[2] * 255);
	color1.setGreen(color[1] * 255);
	onGetColor(color1);
	OnOpacityValueChanged(100);
	ChangeVisible2DState(true);
	ChangeVisible3DState(true);
	
}

void NaviWidget3D::onGetColor(QColor color)
{
	m_Data->m_red = color.red();
	m_Data->m_green = color.green();
	m_Data->m_blue = color.blue();
	SetAttribute(NaviWidget3D::color_red.c_str(), QStringLiteral("%1").arg(m_Data->m_red).toStdString().c_str());
	SetAttribute(NaviWidget3D::color_green.c_str(), QStringLiteral("%1").arg(m_Data->m_green).toStdString().c_str());
	SetAttribute(NaviWidget3D::color_blue.c_str(), QStringLiteral("%1").arg(m_Data->m_blue).toStdString().c_str());

	freshColor();
}

void NaviWidget3D::freshColor() {
	QString colorStr = QString("background-color:rgb(%1,%2,%3);").arg(m_Data->m_red).arg(m_Data->m_green).arg(m_Data->m_blue);
	ui.btnPalette->setStyleSheet(colorStr);
	double Color[4];
	Color[0] = m_Data->m_red / 255.0;
	Color[1] = m_Data->m_green / 255.0;
	Color[2] = m_Data->m_blue / 255.0;
	Color[3] = 0.5;
	if(m_Data->segment)
	m_Data->segment->SetColor(Color);
}

void NaviWidget3D::onSetBackColor()
{
	
	
}

NaviWidget3D::~NaviWidget3D() {
	
}

void NaviWidget3D::SetTitle(QString title) {
	m_Data->structure_label = title;
	SetAttribute(NaviWidget3D::structure_name.c_str(), title.toStdString().c_str());
	ui.lblName->setText(m_Data->structure_label);
}


void NaviWidget3D::hideDeleteBtn() {
	ui.btnDelete->setVisible(false);
}

void NaviWidget3D::hideModifyBtn() {
	ui.btnModify->setVisible(false);
}

void NaviWidget3D::SetPicture(QString path, QString alias)
{
 	m_Data->structure_path = path;
	m_Data->structure_alias = alias;

	QImage img;
	img.load(path + alias + ".png");
	QPixmap pixmap = QPixmap::fromImage(img);
	/*int w = 96;
	int h = 96;*/
	int w = 80;
	int h = 80;
	QPixmap fitpixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	ui.lblImage->setPixmap(fitpixmap);
}

void NaviWidget3D::SetData(Widget3DData* data) {
	m_Data = data;
	if (!m_Data) {
		return;
	}
	SetTitle(m_Data->structure_alias);
	SetPicture(m_Data->structure_path, m_Data->structure_alias);
	freshColor();
	ui.horizontalSlider2D->setValue(m_Data->m_opacity2D);
	ui.horizontalSlider3D->setValue(m_Data->m_opacity3D);
}