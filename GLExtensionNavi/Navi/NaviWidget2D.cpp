#include "NaviWidget2D.h"
#include "vtkMRMLColorTableNode.h"
#include <vtkMRMLDiffusionWeightedVolumeNode.h>
#include "vtkMRMLMarkupsROINode.h"
#include "itksys/SystemTools.hxx"
#include "QMessageBox.h"
#include "vtkMRMLSegmentationNode.h"
#include "QFileInfo.h"
#include "qGLMessageDialog.h"
const std::string NaviWidget2D::visible_2d = "visible_2d";
const std::string NaviWidget2D::visible_3d = "visible_3d";
const std::string NaviWidget2D::visible_roi = "visible_roi";
const std::string NaviWidget2D::node_roi = "node_roi";
const std::string NaviWidget2D::color_node_id = "color_node_id";
const std::string NaviWidget2D::preset_index = "preset_index";
const std::string NaviWidget2D::preset_shift = "preset_shift";

NaviWidget2D::NaviWidget2D(QWidget*parent)
	: NaviBase2D(parent)
{
	
    ui.setupUi(this);
	setWindowFlag(Qt::FramelessWindowHint); //隐藏标题框
	m_Data = new WidgetData();
	/*connect(ui.checkBox,SIGNAL(stateChanged(int)),
		this, SLOT(onVisibleChanged(int)));*/
	connect(ui.btn2D, SIGNAL(toggled(bool)),
		this, SLOT(onVisibleChanged2D(bool)));
	connect(ui.btn3D, SIGNAL(toggled(bool)),
		this, SLOT(onVisibleChanged3D(bool)));
	connect(ui.btnROI, SIGNAL(toggled(bool)),
		this, SLOT(onVisibleChangedROI(bool)));
	/*connect(ui.chkVolume, SIGNAL(toggled(bool)),
		this, SLOT(OncbxChanged(bool)));*/
	/*connect(ui.checkBox_3, SIGNAL(toggled(bool)),
		this, SLOT(onVisibleChangedROI(bool)));*/
	QObject::connect(ui.stripskull, SIGNAL(clicked()),
		this, SLOT(OnStripSkull()));
	connect(ui.btnModel, SIGNAL(clicked()), this, SLOT(onCreateModel()));
	ui.VolumePropertyNodeWidget->setVisible(true);
	ui.VolumePropertyNodeWidget->setEnabled(true);
	ui.disable_mask->hide();
	//ui.PresetComboBox->setStyleSheet("border:2px solid red");
	//connect(ui.btnVolume, SIGNAL(toggled(bool)), this, SLOT(ControlPanelVisable(bool)));

	pSwitchControl = new SwitchControl(this);
	pSwitchControl->setAttribute(Qt::WA_StyledBackground);
	pSwitchControl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	pSwitchControl->setMinimumWidth(40);
	ui.horizontalLayout->insertWidget(3, pSwitchControl, 0, Qt::Alignment());

	// 设置状态、样式
	pSwitchControl->setToggle(false);
	pSwitchControl->setCheckedColor(QColor(0, 160, 230));
	connect(pSwitchControl, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
	ui.btnControl->setVisible(false);
	//this->setAttribute(Qt::WA_StyledBackground);
	ui.widget->setStyleSheet("background-color: transparent;");
}

void NaviWidget2D::onCreateModel() {
	auto alias_name = m_Node->GetAttribute("alias_name");
	int r = 255;
	int g = 200;
	int b = 200;
	QString volume_name;
	QString segment_name;
	if (strcmp(alias_name, "vessel") == 0) {
		volume_name = "VesselnessFiltered";
		segment_name = "vessel_segmentationnode";
		r = 255;
		b = 0;
		g = 0;
	}
	else if (strcmp(alias_name, "[S]vessel") == 0) {
		volume_name = "strippedbrain_vessel";
		segment_name = "stripped_vessel_segmentationnode";
		r = 255;
		b = 0;
		g = 0;
	}
	else if (strcmp(alias_name, "[S]basic") == 0) {
		volume_name = "strippedbrain_basic";
		segment_name = "stripped_brain_segmentationnode";
		r = 255;
		b = 200;
		g = 200;
	}
	else {
		auto segment_node = m_Scene->AddNewNodeByClass("vtkMRMLSegmentationNode");
		r = 255;
		g = 255;
		b = 255;
		PythonQtObjectPtr context = PythonQt::self()->getMainModule();
		context.evalScript(
			QString(
				"import slicer.util as util; \
        SlicerSkinExtractorLogic = util.getModuleLogic('SlicerSkinExtractor');\
        SlicerSkinExtractorLogic.process_id('%1','%2');\
        ")
			.arg(m_Node->GetID()).arg(segment_node->GetID())
		);
		segment_node->SetAttribute("structure_name", m_Node->GetAttribute("alias_name"));
		segment_node->SetAttribute("color_red", QString("%1").arg(r).toStdString().c_str());
		segment_node->SetAttribute("color_blue", QString("%1").arg(g).toStdString().c_str());
		segment_node->SetAttribute("color_green", QString("%1").arg(b).toStdString().c_str());
		segment_node->SetAttribute("extra_image_path", m_Node->GetAttribute("extra_image_path"));

		vtkMRMLColorNode* tnode = vtkMRMLColorNode::New();
		tnode->SetAttribute("Help_Node", "true");
		tnode->SetAttribute("Help_Name", "fresh_3d");
		tnode->SetAttribute("Help_Info_0", "1");
		m_Scene->AddNode(tnode);
		return;
	}
	PythonQtObjectPtr context = PythonQt::self()->getMainModule();
	context.evalScript(
		QString(
		"import slicer.util as util; \
        BrainSegmentLogic = util.getModuleLogic('HDBrainExtractionTool');\
        BrainSegmentLogic.CreateModelFromVolume('%1','%2',%3,%4,%5);\
        ")
		.arg(volume_name).arg(segment_name).arg(r).arg(g).arg(b)
	);
	auto node = m_Scene->GetFirstNodeByName(segment_name.toStdString().c_str());
	if (!node) {
		return;
	}

	
	node->SetAttribute("structure_name", m_Node->GetAttribute("alias_name"));
	node->SetAttribute("color_red", QString("%1").arg(r).toStdString().c_str());
	node->SetAttribute("color_blue", QString("%1").arg(g).toStdString().c_str());
	node->SetAttribute("color_green", QString("%1").arg(b).toStdString().c_str());
	node->SetAttribute("extra_image_path", m_Node->GetAttribute("extra_image_path"));
	
	vtkMRMLColorNode* tnode = vtkMRMLColorNode::New();
	tnode->SetAttribute("Help_Node", "true");
	tnode->SetAttribute("Help_Name", "fresh_3d");
	tnode->SetAttribute("Help_Info_0", "1");
	m_Scene->AddNode(tnode);
}

void NaviWidget2D::OnStripSkull() {
	auto name = m_Node->GetAttribute("alias_name");
	auto is_stripped_skull = m_Node->GetAttribute("is_stripped_skull");
	if (is_stripped_skull) {
		qGLMessageDialog::ShowWarnning(QStringLiteral("当前图像的已经是去除颅骨图像"));
		return;
	}
	auto stripped_brain_segment = vtkMRMLSegmentationNode::SafeDownCast(m_Scene->GetFirstNodeByName("stripped_brain_segment"));
	if (!stripped_brain_segment) {
		qGLMessageDialog::ShowWarnning(QStringLiteral("您需要先去除颅骨再执行该操作"));
		return;
	}
	if (strcmp(name, "basic") == 0) {
		qGLMessageDialog::ShowWarnning(QStringLiteral("Basic序列已经有了去除颅骨后的序列"));
		return;
	}
	auto new_name = QString("%1_%2").arg("strippedbrain").arg(name).toStdString().c_str();
	
	PythonQtObjectPtr context = PythonQt::self()->getMainModule();
	QString t1 = QString("import slicer.util as util; BrainSegmentLogic = util.getModuleLogic('HDBrainExtractionTool'); BrainSegmentLogic.segment_image_from_skull('%1','%2')").arg(m_Node->GetName()).arg(new_name);
	context.evalScript(t1);
	qCritical() << "end stripped skull segment script";
	emit(fresh_2d());
	qCritical() << "end fresh 2d";
	
	auto node = m_Scene->GetFirstNodeByName(new_name);
	if (node) {
		const char* alias_name_new = node->GetAttribute("alias_name");
		emit(show_node(alias_name_new));
	}
	
	qCritical() << "end show node";
	
}

void NaviWidget2D::onToggled(bool bChecked)
{
	onRongjiChanged(bChecked);
	emit showBlendPro(this);
}

void NaviWidget2D::showMask(NaviWidget2D* Item2D, bool isShow)
{
	if (isShow)
	{
		ui.disable_mask->show();
	}
	else
	{
		ui.disable_mask->hide();
	}
}

//void NaviWidget2D::ControlPanelVisable(bool bChecked)
//{
//	hiddenPanelVisible = bChecked;
//	emit displayHiddenPanel(this);
//}
void NaviWidget2D::SetDropdownBtnStatus(bool state, bool isEmitEvent)
{
	/*if (!isEmitEvent)
		disconnect(ui.btnVolume, SIGNAL(toggled(bool)), this, SLOT(ControlPanelVisable(bool)));
	ui.btnVolume->setChecked(state);
	if (!isEmitEvent)
		connect(ui.btnVolume, SIGNAL(toggled(bool)), this, SLOT(ControlPanelVisable(bool)));*/
}
// --------------------------------------------------------------------------
void NaviWidget2D::setColorNode(vtkMRMLNode* colorNode)
{
	vtkMRMLVolumeNode* volumeNode = m_Node;
	if (!volumeNode) {
		return;
	}
	vtkMRMLScalarVolumeDisplayNode* displayNode = volumeNode ? vtkMRMLScalarVolumeDisplayNode::SafeDownCast(
		volumeNode->GetDisplayNode()) : nullptr;

	if (!displayNode || !colorNode)
	{
		return;
	}
	Q_ASSERT(vtkMRMLColorNode::SafeDownCast(colorNode));
	displayNode->SetAndObserveColorNodeID(colorNode->GetID());
	SetAttribute(NaviWidget2D::color_node_id.c_str(), colorNode->GetID());
}

void NaviWidget2D::onRongjiChanged(int state) {
	if (state == 1) // "选中"
	{
		m_Rongji = 1;
	}
	else {
		m_Rongji = 0;
	}
	
	emit(onRongjiChanged());
}

void NaviWidget2D::SetAttribute(const char* key, const char* val) {
		if (m_Node) {
			m_Node->SetAttribute(key, val);
		}
}

void NaviWidget2D::OnOpacityValueChanged(int value) {
	m_Data->m_opacity = value;
	emit(onOpacityChanged(value));
}

void NaviWidget2D::ChangeVisibleROIState(bool b) {
	onVisibleChangedROI(b);
}

void NaviWidget2D::ChangeVisible2DState(bool b) {
	onVisibleChanged2D(b);
}

void NaviWidget2D::ChangeVisible3DState(bool b) {
	onVisibleChanged3D(b);
}

void NaviWidget2D::onVisibleChanged2D(bool b) {
	if (b) {
		SetAttribute(NaviWidget2D::visible_2d.c_str(), "true");
	}
	else {
		SetAttribute(NaviWidget2D::visible_2d.c_str(), "false");
	}
	emit(visible_changed(this));
}

void NaviWidget2D::onVisibleChanged3D(bool b) {
	if (b) // "选中"
	{
		vtkMRMLVolumePropertyNode* volumePropertyNode = (m_VolumeRenderingDisplayNode ? m_VolumeRenderingDisplayNode->GetVolumePropertyNode() : nullptr);
		ui.PresetComboBox->blockSignals(true);
		ui.PresetComboBox->setCurrentNode(volumePropertyNode);
		ui.PresetComboBox->setMRMLVolumePropertyNode(volumePropertyNode);
		ui.PresetComboBox->updatePresetSliderRange();
		ui.VolumePropertyNodeWidget->setMRMLVolumePropertyNode(volumePropertyNode);
		ui.PresetComboBox->blockSignals(false);
		if (m_Node) {
			m_Node->SetDisplayVisibility(1);
		}
		vrlLogic->setVolumeRendering(m_Node, true);
		auto presetIndex = m_Node->GetAttribute("preset_index");
		SetAttribute(NaviWidget2D::visible_3d.c_str(), "true");
		ui.PresetComboBox->setCurrentNodeID(presetIndex);
		emit(visible_changed(this));
	}
	else {
		if (m_Node) {
			vrlLogic->setVolumeRendering(m_Node, false);
		}
		SetAttribute(NaviWidget2D::visible_3d.c_str(), "false");
	}
}

//void NaviWidget2D::onVisibleChanged(int state) {
//	if (state == Qt::Checked) // "选中"
//	{
//		if(ui.chkVolume->isChecked()){
//			m_Node->SetDisplayVisibility(1);
//		}
//		m_Visibility = true;
//		SetAttribute(NaviWidget2D::visible_2d.c_str(), "true");
//	}
//	else {
//		m_Node->SetDisplayVisibility(0);
//		ui.checkBox_3->setChecked(false);
//		ui.chkVolume->setChecked(false);
//		m_Visibility = false;
//		SetAttribute(NaviWidget2D::visible_2d.c_str(), "false");
//	}
//	emit(visible_changed(this));
//}

vtkMRMLScalarVolumeDisplayNode* NaviWidget2D::GetDisplayNode() {
	auto displayNode = m_Node->GetDisplayNode();
	auto scalarDisplayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(displayNode);
	
	return scalarDisplayNode;
}

void NaviWidget2D::onOpenColorPad()
{
	m_Data->m_pColorDialog = new QColorDialog;
	connect(m_Data->m_pColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(onGetColor(QColor)));
	m_Data->m_pColorDialog->exec();
}

void NaviWidget2D::onGetColor(QColor color)
{
	m_Data->m_red = color.red();
	m_Data->m_green = color.green();
	m_Data->m_blue = color.blue();
	freshColor();
}

void NaviWidget2D::freshColor() {
	QString colorStr = QString("background-color:rgb(%1,%2,%3);").arg(m_Data->m_red).arg(m_Data->m_green).arg(m_Data->m_blue);
	double Color[4];
	Color[0] = m_Data->m_red / 255.0;
	Color[1] = m_Data->m_green / 255.0;
	Color[2] = m_Data->m_blue / 255.0;
	Color[3] = 0.5;
	vtkMRMLColorNode* colorNode = GetDisplayNode()->GetColorNode();
	vtkMRMLColorTableNode* tableColorNode = vtkMRMLColorTableNode::SafeDownCast(colorNode);
	//tableColorNode->SetSimpleColorHue(Color[0]*0.33+ Color[1]*0.33+ Color[2]*0.33);
}

void NaviWidget2D::SetWidget2DVisible(bool checked)
{
	ChangeVisible2DState(checked);
	ChangeVisible3DState(checked);
}


//void NaviWidget2D::OncbxChanged(bool b) {
//	if (ui.chkVolume->isChecked())
//	{
//		if (!b_visible_3d) {
//			ui.chkVolume->setChecked(false);
//			return;
//		}
//		
//		
//		emit(visible_changed(this));
//	}
//	else
//	{
//		
//	}
//}

void NaviWidget2D::DoSetColorTableNode(vtkMRMLNode* node) {
	//ui.ColorTableComboBox->setCurrentNodeIndex(2);
	ui.ColorTableComboBox->setCurrentNode(node);
}

void NaviWidget2D::SetName(QString name1) {
	ui.lblName->setText(name1);
}

QPixmap* NaviWidget2D::GetPirctureFromScalarVolumeNode(vtkMRMLScalarVolumeNode* node) {
	return nullptr;
}

void NaviWidget2D::SetNode(vtkMRMLScene* scene,vtkMRMLScene* m_PresetScene, vtkMRMLScalarVolumeNode* in_node, vtkMRMLVolumeRenderingDisplayNode* displayNode) {
	m_Scene = scene; 
	m_Node = in_node;
	char* name = m_Node->GetName();
	const char* origin = m_Node->GetAttribute("origin_node");
	const char* alias_name = m_Node->GetAttribute("alias_name");
	if (origin) {
		auto n1 = m_Scene->GetNodeByID(origin);
		if (n1) {
			name = n1->GetName();
		}
	}
	const char* filename = m_Node->GetAttribute("filename");
	bool isFile = false;
	if (filename) {
		auto foldpath = itksys::SystemTools::GetParentDirectory(filename);
		auto picpath = foldpath + "/cover.png";
		QFileInfo fileInfo(QString::fromStdString(picpath));
		if (fileInfo.isFile()) {
			QImage img;
			img.load(QString::fromStdString(picpath));
			QPixmap pixmap = QPixmap::fromImage(img);
			int w = 80;
			int h = 80;
			QPixmap fitpixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			ui.lblImage->setPixmap(fitpixmap);
			if(alias_name)
				ui.lblName->setText(QString::fromStdString(std::string(alias_name)));
			else
				ui.lblName->setText("    ");
			isFile = true;
		}
	}
	ui.PresetComboBox->setMRMLScene(m_PresetScene);
	ui.ColorTableComboBox->setMRMLScene(scene);
	
	m_VolumeRenderingDisplayNode = displayNode;
	//ui.PresetComboBox->setEnabled(true);
	//ui.checkBox->setChecked(true);
	vrlLogic->setVolumeRendering(m_Node, false);
	QObject::connect(ui.ColorTableComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
		this, SLOT(setColorNode(vtkMRMLNode*)));
	QObject::connect(ui.PresetComboBox, SIGNAL(presetOffsetChanged(double, double, bool)),
		ui.VolumePropertyNodeWidget, SLOT(moveAllPoints(double, double, bool)));
	QObject::connect(ui.PresetComboBox, SIGNAL(currentNodeIDChanged(const QString)),
		this, SLOT(SetPresettNodeID(const QString)));
	
}

void NaviWidget2D::SetPresettNodeID(const QString id1) {
	SetAttribute(NaviWidget2D::preset_index.c_str(), id1.toStdString().c_str());
}

void NaviWidget2D::SetExtraImage(QString path) {
	auto applicationPath = QApplication::applicationDirPath();
	auto picpath = applicationPath + path;
	auto p1 = picpath;
	qInfo() << "SetExtraImage:" << path;
	QImage img;
	img.load(p1);
	QPixmap pixmap = QPixmap::fromImage(img);
	int w = 80;
	int h = 80;
	QPixmap fitpixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	ui.lblImage->setPixmap(fitpixmap);
}

void NaviWidget2D::onVisibleChangedROI(bool b)
{
	vtkMRMLVolumeRenderingDisplayNode* displayNode = m_VolumeRenderingDisplayNode;
	if (!displayNode)
	{
		return;
	}
	// When the display box is visible, it should probably activate the
	// cropping (to follow the "what you see is what you get" pattern).
	

	vtkMRMLDisplayableNode* roiNode = m_VolumeRenderingDisplayNode->GetROINode();
	if (!roiNode) {
		vtkMRMLMarkupsROINode* node = vtkMRMLMarkupsROINode::SafeDownCast(m_Scene->AddNewNodeByClass("vtkMRMLAnnotationROINode"));
		m_VolumeRenderingDisplayNode->SetAndObserveROINodeID(node->GetID());
		roiNode = m_VolumeRenderingDisplayNode->GetROINode();
		
	}
	SetAttribute(NaviWidget2D::node_roi.c_str(), roiNode->GetID());
	int numberOfDisplayNodes = roiNode->GetNumberOfDisplayNodes();

	std::vector<int> wasModifying(numberOfDisplayNodes);

	for (int index = 0; index < numberOfDisplayNodes; index++)
	{
		wasModifying[index] = roiNode->GetNthDisplayNode(index)->StartModify();
	}

	if (b)
	{
		displayNode->SetCroppingEnabled(b);
		//ui.btnROI->setStyleSheet("background-color: rgb(255, 66, 69);");
	}
	else {
		//ui.btnROI->setStyleSheet("background-color: rgb(36, 46, 66);");
	}
	roiNode->SetDisplayVisibility(b);
	SetAttribute(NaviWidget2D::visible_roi.c_str(), QStringLiteral("%1").arg(b).toStdString().c_str());
	for (int index = 0; index < numberOfDisplayNodes; index++)
	{
		roiNode->GetNthDisplayNode(index)->EndModify(wasModifying[index]);
	}
}