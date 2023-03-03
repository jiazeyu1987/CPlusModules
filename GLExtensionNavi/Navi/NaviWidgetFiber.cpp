#include "NaviWidgetFiber.h"


#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkParametricEllipsoid.h>
#include "vtkCallbackCommand.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "qSlicerApplication.h"
#include "qSlicerIOManager.h"
#include "vtkMatrix4x4.h"
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkTrivialProducer.h>
#include <vtkUnstructuredGrid.h>
#include <vtksys/SystemTools.hxx>
#include <vtksys/RegularExpression.hxx>
#include <vtkAbstractTransform.h>
#include <vtkGeneralTransform.h>
#include <QtCore/qitemselectionmodel.h>
#include <QtWidgets/qabstractitemview.h>
#include <QtWidgets/qcompleter.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkEventBroker.h>
#include <QtWidgets/qlistwidget.h>
#include <vtkMRMLMarkupsLineNode.h>
#include <vtkMRMLMarkupsDisplayNode.h>
#include <vtkCylinderSource.h>
#include <vtkImplicitModeller.h>
#include "qGLMessageDialog.h"
#include "vtkContourFilter.h"
#include "vtkMRMLSegmentationDisplayNode.h"
#include "ctkDoubleSlider.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "QMessageBox.h"
#include <QtWidgets/qboxlayout.h>
FiberBaseInfo info1(1, 175, 4, "FD04C01A", "FD04C01A", 5, 6, 6, 7.5, 2.7, 3.7);
FiberBaseInfo info2(2, 175, 4, "FD04C01B", "FD04C01B", 5, 6, 6, 7.5, 2.7, 3.7);
FiberBaseInfo info3(3, 175, 10, "FD10C01A", "FD10C01A", 6.5, 8.3, 7, 7.5, 9.2, 11.7);
FiberBaseInfo info4(4, 175, 10, "FD10C01B", "FD10C01B", 8.5, 9, 7.5, 9, 7.2, 7.2);
FiberBaseInfo info5(5, 175, 15, "FD15C01A", "FD15C01A", 7.5, 7.5, 7, 7, 7.2, 7.2);
FiberBaseInfo info6(6, 175, 15, "FD15C01B", "FD15C01B", 7.5, 7.5, 7, 7, 7.2, 7.2);
QVector<FiberBaseInfo> NaviWidgetFiber::m_BaseInfoVec = { info1, info2, info3, info4, info5, info6 };

const std::string NaviWidgetFiber::input_node_id = "input_node_id";
const std::string NaviWidgetFiber::output_node_id = "output_node_id";
const std::string NaviWidgetFiber::stl_head = "stl_head";
const std::string NaviWidgetFiber::fiber_name = "fiber_name";
const std::string NaviWidgetFiber::fiber_type = "fiber_type";
const std::string NaviWidgetFiber::ablation_node_id = "ablation_node_id";
const std::string NaviWidgetFiber::fiber_save_depth = "fiber_save_depth";
const std::string NaviWidgetFiber::fiber_ablation_visible = "fiber_ablation_visible";
const std::string NaviWidgetFiber::fiber_visible = "fiber_visible";
const std::string NaviWidgetFiber::color_red = "color_red";
const std::string NaviWidgetFiber::color_blue = "color_blue";
const std::string NaviWidgetFiber::color_green = "color_green";
const std::string NaviWidgetFiber::red_projection = "red_projection";
const std::string NaviWidgetFiber::green_projection = "green_projection";
const std::string NaviWidgetFiber::yellow_projection = "yellow_projection";
const std::string NaviWidgetFiber::stl_head_node = "stl_head_node";
const std::string NaviWidgetFiber::r_d_direction = "r_d_direction";
const std::string NaviWidgetFiber::r_d_distance = "r_d_distance";
const std::string NaviWidgetFiber::leksell_x = "leksell_x";
const std::string NaviWidgetFiber::leksell_y = "leksell_y";
const std::string NaviWidgetFiber::leksell_z = "leksell_z";
const std::string NaviWidgetFiber::leksell_arc = "leksell_arc";
const std::string NaviWidgetFiber::leksell_ran = "leksell_ran";
const std::string NaviWidgetFiber::protect_area_id = "protect_area_id";
const std::string NaviWidgetFiber::protect_area_visible = "protect_area_visible";
const std::string NaviWidgetFiber::protect_area_diameter = "protect_area_diameter";
const std::string NaviWidgetFiber::is_on_edit = "is_on_edit";

NaviWidgetFiber::NaviWidgetFiber(FiberData* data, bool is_shuqian,QWidget *parent, vtkMRMLScene* scene,vtkSlicerMeasureLogicAbstract* logic)
	: QWidget(parent), m_FiberData(nullptr)
{
	ui.setupUi(this);
	m_FiberData = data;
	m_FiberData->m_Logic = logic;
	m_FiberData->m_Scene = scene;
	m_FiberData->m_IsShuqian = is_shuqian;
	setWindowFlag(Qt::FramelessWindowHint); //隐藏标题框
	//this->setAttribute(Qt::WA_StyledBackground);
	ui.cmbType->clear(); //清除列表
	for (int i = 0; i < m_BaseInfoVec.count(); i++) {
		auto info = m_BaseInfoVec.at(i);
		ui.cmbType->addItem(info.name);
	}
	
	ctrl_ablation = CreateSwitch(ui.horizontalLayout);
	ui.widget->setStyleSheet("background-color: transparent;");
	ctrl_protection = CreateSwitch(ui.horizontalLayout_2);
	ctrl_vessel = CreateSwitch(ui.horizontalLayout_3);

	ui.label->setPixmap(QPixmap(":/Resources/fiber.png").scaled(QSize(60,60)));
	ui.label->setAlignment(Qt::AlignHCenter);

	/*ui.cmbType->addItem(QStringLiteral("未选择"));
	ui.cmbType->addItem(QStringLiteral("FD4C01A-4mm"));
	ui.cmbType->addItem(QStringLiteral("FD4C01B-4mm"));
	ui.cmbType->addItem(QStringLiteral("FD10C01A-10mm"));
	ui.cmbType->addItem(QStringLiteral("FD10C01B-10mm"));
	ui.cmbType->addItem(QStringLiteral("FD15C01A-15mm"));
	ui.cmbType->addItem(QStringLiteral("FD15C01B-15mm"));*/
	ui.cmbType->setCurrentIndex(0);
	m_BaseInfo = m_BaseInfoVec[ui.cmbType->currentIndex()];
	connect(ui.cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnFiberSTLChanged(int)));
	connect(ui.btnPalette, SIGNAL(clicked()), this, SLOT(OnOpenColorPad()));
	//connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(OnChangeVisible()));
	connect(ui.btnVisible, SIGNAL(toggled(bool)), this, SLOT(OnChangeVisible(bool)));
	

	connect(ui.btnAddTarget, SIGNAL(toggled(bool)), this, SLOT(OnClickBtnFiberPoint1(bool)));
	connect(ui.btnAddEntry, SIGNAL(toggled(bool)), this, SLOT(OnClickBtnFiberPoint2(bool)));
	//connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(OnDepthChanged(int)));
	//connect(ui.btnDelete, SIGNAL(clicked()), this, SLOT(OnDeleteAblationArea()));//删除消融区
	//connect(ui.btn_add, SIGNAL(clicked()), this, SLOT(OnCreateFiber()));
	//connect(ui.btn_add_3, SIGNAL(clicked()), this, SLOT(OnAddAblationArea()));
	//connect(ui.btn_add_4, SIGNAL(clicked()), this, SLOT(OnEstimate()));
	connect(ui.btn_delete, SIGNAL(clicked()), this, SLOT(OnDelete()));
	connect(ui.btn_edit, SIGNAL(toggled(bool)), this, SLOT(OnEdit(bool)));
	//connect(ui.cb_ablation, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAblationChanged(int)));
	connect(ui.lineEdit, SIGNAL(textChanged(QString)), this, SLOT(OnFiberNameChanged(QString)));
	connect(ui.button_target, SIGNAL(clicked()), this, SLOT(OnClickTargetPoint()));
	connect(ui.button_input, SIGNAL(clicked()), this, SLOT(OnClickInputPoint()));

	connect(ui.button_target_2, SIGNAL(clicked()), this, SLOT(OnAlpha0()));
	connect(ui.button_target_3, SIGNAL(clicked()), this, SLOT(OnAlpha100()));

	connect(ui.horizontalSlider_4, SIGNAL(valueChanged(double)), this, SLOT(OnProtectionDiameterChanged(double)));
	//connect(ui.btn_add_2, SIGNAL(clicked()), this, SLOT(OnFiberViewDirection()));
	connect(ui.horizontalSlider_2, SIGNAL(valueChanged(int)),	this, SLOT(OnOpacityChanged(int)));
	connect(ctrl_vessel, SIGNAL(toggled(bool)), this, SLOT(OnEstimate(bool)));
	connect(ctrl_ablation, SIGNAL(toggled(bool)), this, SLOT(OnChangeAblactionVisible(bool)));
	connect(ctrl_protection, SIGNAL(toggled(bool)), this, SLOT(OnChangeProtectVisible(bool)));
	connect(ui.cmbDir, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onChangeViewDirection(int)));

	m_FiberData->m_ObserverManager = vtkObserverManager::New();
	m_FiberData->m_ObserverManager->GetCallbackCommand()->SetClientData(this);
	m_FiberData->m_ObserverManager->GetCallbackCommand()->SetCallback(
		NaviWidgetFiber::ProcessMRMLEvents);
	m_CallbackCommand->SetClientData(reinterpret_cast<void*>(this));
	m_CallbackCommand->SetCallback(NaviWidgetFiber::processEvents);
	m_FiberData->m_Logic->AddObserver(9999, this->m_CallbackCommand);
	m_FiberData->m_Logic->AddObserver(9998, this->m_CallbackCommand);
	

	CreatPicTextButton(ui.btnAddTarget, ":/Resources/fiberTargetIcon.png", QStringLiteral("添加靶点"));
	CreatPicTextButton(ui.btnAddEntry, ":/Resources/fiberEntryIcon.png", QStringLiteral("添加入点"));
	//ui.lblPerspective->hide();       //光纤视角深度
	//ui.widgetSliderBkg->hide();		 //光纤视角深度滑块

	//ui.btn_add_3->setEnabled(false); // 添加消融区
	//ui.btn_add->setEnabled(false);   //添加光纤
	//ui.btn_add_2->setEnabled(false); // 针道视角

	//ui.btnDelete->hide();

	//ui.btn_add_3->hide();
	//ui.cb_ablation->hide();
	//ui.btn_add_2->hide();
	ui.lblMoveDistance->hide();
	ui.lblMoveDistance->hide();
	OnChangeVisible(true);
	OnChangeAblactionVisible(true);

	m_FiberData->m_TargetFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(m_FiberData->m_Scene->AddNewNodeByClass("vtkMRMLMarkupsFiducialNode", "target_node"));
	qvtkReconnect(m_FiberData->m_TargetFiducialNode, m_FiberData->m_TargetFiducialNode, vtkMRMLMarkupsNode::PointPositionDefinedEvent,
		this, SLOT(onTargetNodeAdded()));

	m_FiberData->m_InputFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(m_FiberData->m_Scene->AddNewNodeByClass("vtkMRMLMarkupsFiducialNode", "Input_node"));
	qvtkReconnect(m_FiberData->m_InputFiducialNode, m_FiberData->m_InputFiducialNode, vtkMRMLMarkupsNode::PointPositionDefinedEvent,
		this, SLOT(onInputNodeAdded()));
	

	SetFiberEnabled(false);
}


void NaviWidgetFiber::OnEdit(bool b) {
	emit(on_edit_fiber(this,b));
	if(b){
		SetAttribute(NaviWidgetFiber::is_on_edit.c_str(), "true");
	}
	else {
		SetAttribute(NaviWidgetFiber::is_on_edit.c_str(), "false");
	}
}

void NaviWidgetFiber::onChangeViewDirection(int i) {
	emit(view_direction(this, i));
	ui.cmbDir->setCurrentIndex(0);
}

void NaviWidgetFiber::CreateCylinderProtectionArea() {
	if (!m_FiberData->cyl)
		m_FiberData->cyl = vtkCylinderSource::New();
	else
	return;
	m_FiberData->cyl->SetRadius(ui.horizontalSlider_4->value());

	

	m_FiberData->cyl->SetHeight(m_BaseInfoVec[ui.cmbType->currentIndex()].length);
	m_FiberData->cyl->SetResolution(60);
}

void NaviWidgetFiber::OnChangeProtectVisible(bool b) {
	if (!m_FiberData->m_ModelNode) {
		return;
	}
	if (b) {
		if (!m_FiberData->m_ProtectionNode) {

			CreateCylinderProtectionArea();
			m_FiberData->cyl->Update();

			double center[3];
			center[0] = 0;
			center[1] = 0;
			center[2] = m_BaseInfoVec[ui.cmbType->currentIndex()].length / 2;

			vtkNew<vtkMatrix4x4> rotateMatrix;
			rotateMatrix->Identity();
			rotateMatrix->SetElement(1,1,0);
			rotateMatrix->SetElement(1, 2, -1);
			rotateMatrix->SetElement(2,1,1);
			rotateMatrix->SetElement(2, 2, 0);

			vtkNew<vtkTransform> handleToWorldTransform;
			handleToWorldTransform->Identity();
			handleToWorldTransform->PostMultiply();
			handleToWorldTransform->SetMatrix(rotateMatrix);
			handleToWorldTransform->Translate(center);

			auto transformNode = vtkMRMLTransformNode::SafeDownCast(m_FiberData->m_Scene->AddNewNodeByClass("vtkMRMLTransformNode"));
			m_FiberData->m_ProtectionNode = vtkMRMLSegmentationNode::SafeDownCast(m_FiberData->m_Scene->AddNewNodeByClass("vtkMRMLSegmentationNode"));
			m_FiberData->m_ProtectionNode->CreateDefaultDisplayNodes();
			auto displayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(m_FiberData->m_ProtectionNode->GetDisplayNode());


			m_FiberData->m_ProtectionNode->SetAndObserveTransformNodeID(transformNode->GetID());
			transformNode->SetAndObserveTransformNodeID(m_FiberData->m_ModelNode->GetTransformNodeID());
			transformNode->SetMatrixTransformToParent(handleToWorldTransform->GetMatrix());
			
			displayNode->SetOpacity2DFill(0.1);
			

			ui.horizontalSlider_4->setValue(10.0);
			OnProtectionDiameterChanged(ui.horizontalSlider_4->value());
			SetAttribute(NaviWidgetFiber::protect_area_id.c_str(), m_FiberData->m_ProtectionNode->GetID());
		}
		auto displayNode = m_FiberData->m_ProtectionNode->GetDisplayNode();
		displayNode->SetVisibility(true);
		SetAttribute(NaviWidgetFiber::protect_area_visible.c_str(), "true");
		
	}
	else {
		auto displayNode = m_FiberData->m_ProtectionNode->GetDisplayNode();
		displayNode->SetVisibility(false);
		SetAttribute(NaviWidgetFiber::protect_area_visible.c_str(), "false");
	}
}

void NaviWidgetFiber::SetFrameInfo(QString x, QString y, QString z, QString arc, QString ran) {
	SetAttribute(leksell_x.c_str(), x.toStdString().c_str());
	SetAttribute(leksell_y.c_str(), y.toStdString().c_str());
	SetAttribute(leksell_z.c_str(), z.toStdString().c_str());
	SetAttribute(leksell_arc.c_str(), arc.toStdString().c_str());
	SetAttribute(leksell_ran.c_str(), ran.toStdString().c_str());
}

void NaviWidgetFiber::changeRelativePositionDirection(int index) {
	m_FiberData->m_RelativePositionDirection = index;
	SetAttribute(r_d_direction.c_str(), QString("%1").arg(m_FiberData->m_RelativePositionDirection).toStdString().c_str());
}

void NaviWidgetFiber::ModifyRelativePositionDistance(double dis) {
	m_FiberData->m_RelativePositionDistance += dis;
	if (m_FiberData->m_RelativePositionDistance < 0 ) {
		m_FiberData->m_RelativePositionDistance = 0;
	}
	SetAttribute(r_d_distance.c_str(), QString("%1").arg(m_FiberData->m_RelativePositionDistance).toStdString().c_str());
}

void NaviWidgetFiber::OnClickInputPoint() {
	emit(center_input());
}

void NaviWidgetFiber::ChangeWave() {
	if (!m_FiberData->m_SegmentationNode) {
		return;
	}
	vtkSegmentation* segmentation = m_FiberData->m_SegmentationNode->GetSegmentation();
	int number = segmentation->GetNumberOfSegments();
	for (int i = 0; i < number; i++) {
		onRemoveAblationArea();
	}
	for (int i = 0; i < number; i++) {
		OnAddAblationArea();
	}
}

void NaviWidgetFiber::SetSliderValue(int value) {
	//ui.horizontalSlider->setValue(value);
}

void NaviWidgetFiber::OnClickTargetPoint() {
	emit(center_target());
}

void NaviWidgetFiber::CreateSegmentationNode() {
	if (m_FiberData->m_SegmentationNode) {
		return;
	}
	m_FiberData->m_SegmentationNode = vtkMRMLSegmentationNode::SafeDownCast(m_FiberData->m_Scene->CreateNodeByClass("vtkMRMLSegmentationNode"));
	
	m_FiberData->m_SegmentationNode->CreateDefaultStorageNode();
	m_FiberData->m_SegmentationNode->SetMasterRepresentationToClosedSurface();
	m_FiberData->m_Scene->AddNode(m_FiberData->m_SegmentationNode);
	m_FiberData->m_SegmentationNode->CreateDefaultDisplayNodes();
	vtkSegmentation* segmentation = m_FiberData->m_SegmentationNode->GetSegmentation();
	m_FiberData->m_SegmentationNode->GetDisplayNode()->SetOpacity(0.5);
	SetAttribute(NaviWidgetFiber::ablation_node_id.c_str(), m_FiberData->m_SegmentationNode->GetID());
	//segmentation->SetMasterRepresentationName(vtkSegmentationConverter::GetClosedSurfaceRepresentationName());
}


void NaviWidgetFiber::DoUpdateAblationNumber() {
	int number = 0;
	if (m_FiberData->m_SegmentationNode) {
		number = m_FiberData->m_SegmentationNode->GetSegmentation()->GetNumberOfSegments();
	}
	ui.lblCount->setText(QString::number(number));
}

bool NaviWidgetFiber::disable_use() {
	/*if (m_FiberData->m_PointTarget[0] == 0 && m_FiberData->m_PointTarget[1] == 0 && m_FiberData->m_PointTarget[2] == 0 &&
		m_FiberData->m_PointInput[0] == 0 && m_FiberData->m_PointInput[1] == 0 && m_FiberData->m_PointInput[2] == 0) {
		return false;
	}
	else
		return true;*/
	if (m_FiberData->m_TargetFiducialNode->GetNumberOfControlPoints() == 0 &&
		m_FiberData->m_InputFiducialNode->GetNumberOfControlPoints() == 0) {
		return false;
	}
	else {
		return true;
	}
}

void NaviWidgetFiber::processEvents(vtkObject* caller, unsigned long eid, void* clientData, void* callData) {
	NaviWidgetFiber* self = reinterpret_cast<NaviWidgetFiber*>(clientData); 
	if (!self->m_FiberData) {
		return;
	}
	if (!self->m_FiberData->m_Active) {
		return;
	}
	if(eid==9999){
	
	vtkMRMLMarkupsFiducialNode* item = reinterpret_cast<vtkMRMLMarkupsFiducialNode*>(callData);
	if (!item) {
		return;
	}
	self->m_FiberData->m_FiducialNode = item;
	self->m_FiberData->m_FiducialNode->CreateDefaultDisplayNodes();
	vtkMRMLMarkupsDisplayNode* displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(item->GetDisplayNode());
	displayNode->PropertiesLabelVisibilityOff();
	displayNode->PointLabelsVisibilityOff();
	vtkNew<vtkIntArray> events;
	events->InsertNextValue(9998);
	self->m_FiberData->m_ObserverManager->AddObjectEvents(displayNode, events);
	}
	if (eid == 9998) {
		/*if (!self->m_FiberData->m_LoadingModelFlag) {
			return;
		}
		self->m_FiberData->m_LoadingModelFlag = false;
		vtkMRMLModelNode* model = reinterpret_cast<vtkMRMLModelNode*>(callData);
		self->m_FiberData->m_ModelNode = model;
		self->SetAttribute(NaviWidgetFiber::fiber_name.c_str(), self->GetName().toStdString().c_str());
		self->SetAttribute(NaviWidgetFiber::fiber_type.c_str(), self->GetType().toStdString().c_str());
		self->SetAttribute(NaviWidgetFiber::input_node_id.c_str(), self->m_FiberData->m_InputFiducialNode->GetID());
		self->SetAttribute(NaviWidgetFiber::output_node_id.c_str(), self->m_FiberData->m_TargetFiducialNode->GetID());
		
		self->DoUpdateFiberPosition();*/
		
		

	}
}

void NaviWidgetFiber::mouseReleaseEvent(QMouseEvent* event)
{
	//emit clicked_fiber(this);
	emit clicked(this);
}
QString NaviWidgetFiber::GetName() {
	return ui.lineEdit->text();
}

void NaviWidgetFiber::SetFiberName(QString str)
{
	ui.lineEdit->setText(str);
	SetAttribute(NaviWidgetFiber::fiber_name.c_str(), str.toStdString().c_str());
}

QString NaviWidgetFiber::GetType() {
	return ui.cmbType->currentText();
}

void NaviWidgetFiber::CreatPicTextButton(QPushButton* btn, QString picPath, QString text)
{
	QLabel* labelPic = new QLabel;
	QLabel* labelText = new QLabel;
	labelPic->setPixmap(QPixmap(picPath));
	labelText->setText(text);
	labelText->setStyleSheet("font: 20px 'Source Han Sans CN-Regular, Source Han Sans CN';");
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addSpacing(2);
	layout->addWidget(labelPic);
	layout->addSpacing(10);
	layout->addWidget(labelText);
	layout->addStretch();
	btn->setLayout(layout);
}

void NaviWidgetFiber::DoUpdateFiberPosition() {
	double m_PointInput[3];
	m_FiberData->m_InputFiducialNode->GetNthControlPointPositionWorld(0, m_PointInput);
	double m_PointTarget[3];
	m_FiberData->m_TargetFiducialNode->GetNthControlPointPositionWorld(0, m_PointTarget);

	if (!m_FiberData->m_InputFiducialNode) {
		return;
	}
	if (!m_FiberData->m_TargetFiducialNode) {
		return;
	}
	if (!m_FiberData->m_TransformNode) {
		vtkMRMLTransformNode* transform_node = vtkMRMLTransformNode::SafeDownCast(m_FiberData->m_Scene->CreateNodeByClass("vtkMRMLLinearTransformNode"));
		m_FiberData->m_Scene->AddNode(transform_node);
		m_FiberData->m_TransformNode = transform_node;
	}
	vtkMRMLMarkupsNode::ControlPoint* cp1 = m_FiberData->m_InputFiducialNode->GetNthControlPoint(0);
	vtkMRMLMarkupsNode::ControlPoint* cp2 = m_FiberData->m_TargetFiducialNode->GetNthControlPoint(0);
	m_PointTarget[0] = cp2->Position[0];
	m_PointTarget[1] = cp2->Position[1];
	m_PointTarget[2] = cp2->Position[2];
	m_PointInput[0] = cp1->Position[0];
	m_PointInput[1] = cp1->Position[1];
	m_PointInput[2] = cp1->Position[2];

	vtkMRMLTransformNode* transform_node = m_FiberData->m_TransformNode;
	vtkNew<vtkMatrix4x4> transformToParentMatrix;
	transformToParentMatrix->Identity();
	transformToParentMatrix->SetElement(0, 3, m_PointTarget[0]);
	transformToParentMatrix->SetElement(1, 3, m_PointTarget[1]);
	transformToParentMatrix->SetElement(2, 3, m_PointTarget[2]);

	double rotationVector_Local[3] = { 0.0 };

	double vector0[3];
	vector0[0] = 0;
	vector0[1] = 0;
	vector0[2] = 1;
	double vector1[3];
	vector1[0] = m_PointInput[0] - m_PointTarget[0];
	vector1[1] = m_PointInput[1] - m_PointTarget[1];
	vector1[2] = m_PointInput[2] - m_PointTarget[2];
	double angle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(vector0, vector1));
	vtkMath::Cross(vector0, vector1, rotationVector_Local);

	vtkNew<vtkTransform> handleToParentMatrix;
	handleToParentMatrix->PostMultiply();
	handleToParentMatrix->Concatenate(transformToParentMatrix);
	handleToParentMatrix->Translate(-m_PointTarget[0], -m_PointTarget[1], -m_PointTarget[2]);
	handleToParentMatrix->RotateWXYZ(angle, rotationVector_Local);
	handleToParentMatrix->Translate(m_PointTarget);
	transformToParentMatrix->DeepCopy(handleToParentMatrix->GetMatrix());

	transform_node->SetMatrixTransformToParent(transformToParentMatrix.GetPointer());
	vtkNew<vtkMatrix4x4>rasToIJK;
	auto cc = transform_node->GetMatrixTransformFromWorld(rasToIJK);
	RotateToFiber(m_PointTarget, m_PointInput);

	if (m_FiberData->m_ModelNode2) {
		if (!m_FiberData->m_ModelNode2->GetDisplayNode()) {
			m_FiberData->m_ModelNode2->CreateDefaultDisplayNodes();
		}
		m_FiberData->m_ModelNode2->GetDisplayNode()->SetVisibility2D(true);
		m_FiberData->m_ModelNode2->SetAndObserveTransformNodeID(transform_node->GetID());

	}
	if (m_FiberData->m_ModelNode) {
		if (!m_FiberData->m_ModelNode->GetDisplayNode()) {
			m_FiberData->m_ModelNode->CreateDefaultDisplayNodes();
		}
		m_FiberData->m_ModelNode->GetDisplayNode()->SetVisibility2D(true);
		m_FiberData->m_ModelNode->SetAndObserveTransformNodeID(transform_node->GetID());
	}
	
	/*if (m_FiberData->m_ProtectionNode) {
		auto dia = ui.horizontalSlider_4->value();
		DoSafeRemove(m_FiberData->m_ProtectionNode);
		m_FiberData->m_ProtectionNode = nullptr;
		m_FiberData->m_ProtectionNodeVisibility = false;
		OnChangeProtectVisible(false);
		ui.horizontalSlider_4->setValue(dia);
	}*/
	//transform_node->HardenTransform();
	
	

	
	
	
}

void NaviWidgetFiber::SetSwitchVisible(SwitchControl* sc, bool b) {
	QTimer::singleShot(1200, [=]() {
		sc->setToggle(b);
		});
	
}

void NaviWidgetFiber::OnFiberViewDirection() {
	double m_PointInput[3];
	m_FiberData->m_InputFiducialNode->GetNthControlPointPositionWorld(0, m_PointInput);
	double m_PointTarget[3];
	m_FiberData->m_TargetFiducialNode->GetNthControlPointPositionWorld(0, m_PointTarget);
	if(disable_use())
	RotateToFiber(m_PointTarget, m_PointInput);
}

void NaviWidgetFiber::OnAlpha0() { 
	ui.horizontalSlider_2->setValue(0);
}
void NaviWidgetFiber::OnAlpha100() { ui.horizontalSlider_2->setValue(100); }

void NaviWidgetFiber::OnOpacityChanged(int value) {
	double opacity = value / 100.0;
	auto dis = vtkMRMLModelDisplayNode::SafeDownCast(m_FiberData->m_ModelNode->GetDisplayNode());
	dis->SetOpacity(opacity);
	dis->SetSliceIntersectionOpacity(opacity);

	auto dis2 = vtkMRMLModelDisplayNode::SafeDownCast(m_FiberData->m_ModelNode2->GetDisplayNode());
	dis2->SetOpacity(opacity);
	dis2->SetSliceIntersectionOpacity(opacity);

	if (m_FiberData->m_ProtectionNode) {
		auto dis3 = vtkMRMLSegmentationDisplayNode::SafeDownCast(m_FiberData->m_ProtectionNode->GetDisplayNode());
		dis3->SetOpacity(opacity);
	}

	if (m_FiberData->m_TargetFiducialNode) {
		auto disp = m_FiberData->m_TargetFiducialNode->GetDisplayNode();
		if (!disp) {
			m_FiberData->m_TargetFiducialNode->CreateDefaultDisplayNodes();
			disp = m_FiberData->m_TargetFiducialNode->GetDisplayNode();
		}
		disp->SetOpacity(opacity);
	}

	if (m_FiberData->m_InputFiducialNode) {
		auto disp = m_FiberData->m_InputFiducialNode->GetDisplayNode();
		if (!disp) {
			m_FiberData->m_InputFiducialNode->CreateDefaultDisplayNodes();
			disp = m_FiberData->m_InputFiducialNode->GetDisplayNode();
		}
		disp->SetOpacity(opacity);
	}
}

void NaviWidgetFiber::SetToggleActive(bool b) {
	if (m_FiberData->m_TargetFiducialNode) {
		auto disp = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_TargetFiducialNode->GetDisplayNode());
		if (!disp) {
			m_FiberData->m_TargetFiducialNode->CreateDefaultDisplayNodes();
			disp = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_TargetFiducialNode->GetDisplayNode());
		}
		if(b){
			m_FiberData->m_TargetFiducialNode->SetLocked(false);
			disp->SetGlyphScale(1.5);
			disp->SetSelectedColor(0, 1, 0);
			disp->SetHandlesInteractive(true);
		}
		else {
			disp->SetGlyphScale(1);
			disp->SetSelectedColor(1, 1, 0);
			disp->SetHandlesInteractive(false);
			m_FiberData->m_TargetFiducialNode->SetLocked(true);
		}
	}

	if (m_FiberData->m_InputFiducialNode) {
		auto disp = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_InputFiducialNode->GetDisplayNode());
		if (!disp) {
			m_FiberData->m_InputFiducialNode->CreateDefaultDisplayNodes();
			disp = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_InputFiducialNode->GetDisplayNode());
		}
		if(b){
			m_FiberData->m_InputFiducialNode->SetLocked(false);
			disp->SetGlyphScale(1.5);
			disp->SetSelectedColor(0, 1, 0);
			disp->SetHandlesInteractive(true);
		}
		else {
			m_FiberData->m_InputFiducialNode->SetLocked(true);
			disp->SetGlyphScale(1);
			disp->SetSelectedColor(1, 1, 0);
			disp->SetHandlesInteractive(false);
		}
	}
}

void NaviWidgetFiber::OnEstimate(bool b) {
	
	if (!b) {
		m_FiberData->m_Scene->RemoveNode(m_FiberData->m_FiberVesselNode);
		m_FiberData->m_FiberVesselNode = nullptr;
	}
	else {
		
		emit estimate(this);
		ui.label_9->setText(QStringLiteral("血管干涉"));
	}
}

void NaviWidgetFiber::onAdjustFiberPoint() {
	DoUpdateFiberPosition();
}

void NaviWidgetFiber::OnUpdateTargetPointInfo() {
	DoUpdateFiberPosition();
}

void NaviWidgetFiber::update_ablation_position(int i) {
	if (!disable_use())
		return;
	if (!m_FiberData->m_SegmentationNode) {
		return;
	}
	vtkSegmentation* segmentation = m_FiberData->m_SegmentationNode->GetSegmentation();
	int number = segmentation->GetNumberOfSegments();
	auto segment = segmentation->GetNthSegment(i);
	DoAddAblationDetail(m_FiberData->start_pos[i], QString("%1").arg(i+1).toStdString());
}

void NaviWidgetFiber::onRemoveAblationArea() {
	if (!disable_use())
		return;
	if (!m_FiberData->m_SegmentationNode) {
		return;
	}
	m_FiberData->m_AblationNum-=1;
	std::cout << "Remove Ablation:" << m_FiberData->m_AblationNum << std::endl;
	if (m_FiberData->m_AblationNum < 0) {
		m_FiberData->m_AblationNum = 0;
		return;
	}
	vtkSegmentation* segmentation = m_FiberData->m_SegmentationNode->GetSegmentation();
	int number = segmentation->GetNumberOfSegments();
	auto segment = segmentation->GetNthSegment(number - 1);
	segmentation->RemoveSegment(segment);
	DoUpdateAblationNumber();
	if (number == 0) {
	}
	emit ablation_number_changed();
}

void NaviWidgetFiber::OnAddAblationArea() {
	if (m_FiberData->m_AblationNum >= 5)
	{
		//(QStringLiteral("添加模拟消融区"), QStringLiteral("最多可以添加5个模拟消融区, 添加失败"));
		return;
	}
	++m_FiberData->m_AblationNum;
	std::cout << "Add Ablation:" << m_FiberData->m_AblationNum << std::endl;
	CreateSegmentationNode();
	auto startpos = m_BaseInfo.startPos[FiberData::fiber_type] + m_FiberData->m_AblationNum * m_BaseInfo.spacing;
	m_FiberData->start_pos[m_FiberData->m_AblationNum-1] = m_FiberData->m_AblationNum * m_BaseInfo.spacing;
	DoAddAblationDetail(startpos,QString("%1").arg(m_FiberData->m_AblationNum).toStdString());
	DoUpdateAblationNumber();
	emit ablation_number_changed();
}

void NaviWidgetFiber::DoAddAblationDetail(int startpos, std::string segment_id) {
	double m_PointInput[3];
	m_FiberData->m_InputFiducialNode->GetNthControlPointPositionWorld(0, m_PointInput);
	double m_PointTarget[3];
	m_FiberData->m_TargetFiducialNode->GetNthControlPointPositionWorld(0, m_PointTarget);

	vtkSegmentation* segmentation = m_FiberData->m_SegmentationNode->GetSegmentation();
	auto seg = segmentation->GetSegment(segment_id);
	if (seg) {
		segmentation->RemoveSegment(seg);
	}
	vtkSmartPointer<vtkSegment> currentSegment = vtkSmartPointer<vtkSegment>::New();

	auto matrix = vtkMatrix4x4::New();
	m_FiberData->m_ModelNode->GetParentTransformNode()->GetMatrixTransformToWorld(matrix);

	double target[4];
	target[0] = 0;
	target[1] = 0;
	target[2] = 0;
	target[3] = 1;
	double target_out[4];
	matrix->MultiplyPoint(target, target_out);


	vtkEllipsoidSource* sphereSource = vtkEllipsoidSource::New();
	sphereSource->SetCenter(0, 0, 0);
	sphereSource->SetXRadius(m_BaseInfo.ablRadius[FiberData::fiber_type]);
	sphereSource->SetYRadius(m_BaseInfo.ablRadius[FiberData::fiber_type]);
	sphereSource->SetZRadius(m_BaseInfo.ablZRadius[FiberData::fiber_type]);
	sphereSource->SetPhiResolution(100);
	sphereSource->SetThetaResolution(100);
	sphereSource->Update();
	vtkPolyData* polyData = vtkPolyData::SafeDownCast(sphereSource->GetOutput());

	vtkSmartPointer<vtkMatrix4x4> matirx1 = m_FiberData->m_ModelNode->GetParentTransformNode()->GetMatrixTransformFromParent();
	double vector1[3];
	vector1[0] = m_PointInput[0] - m_PointTarget[0];
	vector1[1] = m_PointInput[1] - m_PointTarget[1];
	vector1[2] = m_PointInput[2] - m_PointTarget[2];
	double newp[3];
	auto len1 = sqrt(1.0 * vector1[0] * vector1[0] + vector1[1] * vector1[1] + vector1[2] * vector1[2]);
	newp[0] = m_PointTarget[0] + vector1[0] * startpos / len1;
	newp[1] = m_PointTarget[1] + vector1[1] * startpos / len1;
	newp[2] = m_PointTarget[2] + vector1[2] * startpos / len1;
	matirx1->SetElement(0, 3, newp[0]);
	matirx1->SetElement(1, 3, newp[1]);
	matirx1->SetElement(2, 3, newp[2]);
	matirx1->SetElement(0, 2, -matirx1->GetElement(0, 2));
	matirx1->SetElement(1, 2, -matirx1->GetElement(1, 2));
	matirx1->SetElement(2, 0, -matirx1->GetElement(2, 0));
	matirx1->SetElement(2, 1, -matirx1->GetElement(2, 1));
	vtkTransform* transform1 = vtkTransform::New();
	transform1->SetMatrix(matirx1);

	vtkSmartPointer<vtkGeneralTransform> trasform = vtkSmartPointer<vtkGeneralTransform>::New();
	m_FiberData->m_ModelNode->GetParentTransformNode()->GetTransformToWorld(trasform);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(polyData);
	transformFilter->SetTransform(dynamic_cast<vtkAbstractTransform*>(transform1));
	transformFilter->Update();

	auto currentPolyData = transformFilter->GetOutput();
	currentSegment->AddRepresentation(vtkSegmentationConverter::GetSegmentationClosedSurfaceRepresentationName(), currentPolyData);
	currentSegment->SetName("AblationArea");
	double Color[4];
	Color[0] = 1;
	Color[1] = 1;
	Color[2] = 1;
	Color[3] = 0.1;

	currentSegment->SetColor(Color);
	segmentation->AddSegment(currentSegment, segment_id);
}

//void OnAddAblationArea1() {
//	
//
//	if (!disable_use())
//		return;
//
//	if (!m_FiberData->m_TransformNode) {
//		return;
//	}
//	CreateSegmentationNode();
//	vtkSegmentation* segmentation = m_FiberData->m_SegmentationNode->GetSegmentation();
//	vtkSmartPointer<vtkSegment> currentSegment = vtkSmartPointer<vtkSegment>::New();
//
//	auto matrix = vtkMatrix4x4::New();
//	m_FiberData->m_ModelNode->GetParentTransformNode()->GetMatrixTransformToWorld(matrix);
//	
//	double target[4];
//	target[0] = 0;
//	target[1] = 0;
//	target[2] = 0;
//	target[3] = 1;
//	double target_out[4];
//	matrix->MultiplyPoint(target, target_out);
//
//
//	vtkEllipsoidSource* sphereSource = vtkEllipsoidSource::New();
//	sphereSource->SetCenter(0, 0,0);
//	sphereSource->SetXRadius(m_BaseInfo.ablRadius[FiberData::fiber_type]);
//	sphereSource->SetYRadius(m_BaseInfo.ablRadius[FiberData::fiber_type]);
//	sphereSource->SetZRadius(m_BaseInfo.ablZRadius[FiberData::fiber_type]);
//	sphereSource->SetPhiResolution(100);
//	sphereSource->SetThetaResolution(100);
//	sphereSource->Update();
//	vtkPolyData* polyData = vtkPolyData::SafeDownCast(sphereSource->GetOutput());
//
//	vtkSmartPointer<vtkGeneralTransform> trasform = vtkSmartPointer<vtkGeneralTransform>::New();
//	m_FiberData->m_ModelNode->GetParentTransformNode()->GetTransformToWorld(trasform);
//	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
//	transformFilter->SetInputData(polyData);
//	transformFilter->SetTransform(dynamic_cast<vtkAbstractTransform*>(trasform.GetPointer()));
//	transformFilter->Update();
//
//	auto currentPolyData = transformFilter->GetOutput();
//	currentSegment->AddRepresentation(segmentation->GetMasterRepresentationName(), currentPolyData);
//	currentSegment->SetName("AblationArea");
//
//	segmentation->AddSegment(currentSegment);
//	DoUpdateAblationNumber();
//}

void NaviWidgetFiber::RotateToFiber(double* m_PointTarget, double* m_PointInput) {
	emit(rotate_to_vector(m_PointInput, m_PointTarget)); 
	emit(center_target());
};

//void NaviWidgetFiber::onOpacityChanged1(int i) {
//	auto dis2 = m_FiberData->m_ModelNode2->GetDisplayNode();
//	dis2->SetOpacity(i / 100.0);
//	auto dis = m_FiberData->m_ModelNode->GetDisplayNode();
//	dis->SetOpacity(i / 100.0);
//}

void NaviWidgetFiber::SetFiberFrameInfo(QString x, QString y, QString z, QString arc, QString ring) {
	this->SetAttribute("leksell_frame_position_x", x.toStdString().c_str());
	this->SetAttribute("leksell_frame_position_y", y.toStdString().c_str());
	this->SetAttribute("leksell_frame_position_z", z.toStdString().c_str());
	this->SetAttribute("leksell_frame_position_arc", arc.toStdString().c_str());
	this->SetAttribute("leksell_frame_position_ring", ring.toStdString().c_str());
}

void NaviWidgetFiber::ProcessMRMLEvents(vtkObject* caller, unsigned long eid, void* clientData, void* callData)
{
	return;
	/*NaviWidgetFiber* self = reinterpret_cast<NaviWidgetFiber*>(clientData);
	vtkMRMLMarkupsDisplayNode* displayNode = reinterpret_cast<vtkMRMLMarkupsDisplayNode*>(caller);
	std::vector<std::string> dd = displayNode->GetActiveComponentInteractionContexts();
	if (dd.size()>0) {
		std::string id1 = dd.at(0);
		vtkMRMLMarkupsNode::ControlPoint* cp = self->m_FiberData->m_FiducialNode->GetNthControlPoint(0);
		self->m_FiberData->m_ObserverManager->RemoveObjectEvents(displayNode);
		if (self->m_FiberData->m_State == NaviWidgetFiber::WaitForTargetPoint) {
			self->m_FiberData->m_State = NaviWidgetFiber::Normal;
			self->ui.btnAddTarget->setChecked(false);
			self->m_FiberData->m_PointTarget[0] = cp->Position[0];
			self->m_FiberData->m_PointTarget[1] = cp->Position[1];
			self->m_FiberData->m_PointTarget[2] = cp->Position[2];
			self->CheckFiberAddActive();
		}
		if (self->m_FiberData->m_State == NaviWidgetFiber::WaitForInputPoint) {
			self->m_FiberData->m_State = NaviWidgetFiber::Normal;
			self->ui.btnAddEntry->setChecked(false);
			self->m_FiberData->m_PointInput[0] = cp->Position[0];
			self->m_FiberData->m_PointInput[1] = cp->Position[1];
			self->m_FiberData->m_PointInput[2] = cp->Position[2];
			self->CheckFiberAddActive();
		}
	}*/
}

void NaviWidgetFiber::CheckFiberAddActive() {

	if (!m_FiberData->m_TargetFiducialNode || !m_FiberData->m_InputFiducialNode) {
		//ui.btn_add->setEnabled(false);
	}
	if (m_FiberData->m_TargetFiducialNode->GetNumberOfControlPoints() > 0 &&
		m_FiberData->m_InputFiducialNode->GetNumberOfControlPoints() > 0) {
		//ui.btn_add->setEnabled(true);
		OnCreateFiber();
	}
	else {
		//ui.btn_add->setEnabled(false);
	}

}
	

void NaviWidgetFiber::OnProtectionDiameterChanged(double val) {
	if (!m_FiberData->m_ProtectionNode) {
		return;
	}
	m_FiberData->m_ProtectionNode->GetSegmentation()->RemoveAllSegments();
	CreateCylinderProtectionArea();
	m_FiberData->cyl->SetRadius(val);
	m_FiberData->cyl->Update();
	m_FiberData->m_ProtectionNode->AddSegmentFromClosedSurfaceRepresentation(m_FiberData->cyl->GetOutput(), "Cylinder");
	auto segment = m_FiberData->m_ProtectionNode->GetSegmentation()->GetNthSegment(0);
	if (segment)
		segment->SetColor(255 / 255, 157 / 255.0, 0);
	//emit(estimate(this));
	SetAttribute(NaviWidgetFiber::protect_area_diameter.c_str(), QString::number(val).toStdString().c_str());
}

void NaviWidgetFiber::OnFiberNameChanged(QString) {
	SetAttribute(NaviWidgetFiber::fiber_name.c_str(), ui.lineEdit->text().toStdString().c_str());
}

void NaviWidgetFiber::OnDelete() {
	emit(delete_fiber(this));
}

void NaviWidgetFiber::OnAblationChanged(int index) {
}

void NaviWidgetFiber::OnDeleteAblationArea() {
}

void NaviWidgetFiber::DoFreshAblationList() {
}
void NaviWidgetFiber::setPictureText(QString str)
{
	QStringList strList = str.split('-');
	ui.label_10->setText(strList.at(1));
	ui.label_5->setText(strList.at(2));
}
void NaviWidgetFiber::OnFiberSTLChanged(int  index) {
	QString t1 = ui.cmbType->currentText();
	setPictureText(t1);
	SetAttribute(NaviWidgetFiber::fiber_type.c_str(), t1.toStdString().c_str());

	if (lastSelectType == index) {
		return;
	}
	lastSelectType = index;
	m_BaseInfo = m_BaseInfoVec[lastSelectType];
	m_FiberData->m_FiberType = lastSelectType;
	if (!m_FiberData->m_ModelNode) {
		return;
	}
	qSlicerApplication::application()->processEvents();
	OnCreateFiber();
	qSlicerApplication::application()->processEvents();
	ChangeWave();
	qSlicerApplication::application()->processEvents();
	freshColor();
	qSlicerApplication::application()->processEvents();
}



void NaviWidgetFiber::SetFixedFiberType(QString  index) {
	ui.cmbType->setCurrentText(index);
}


void NaviWidgetFiber::OnDepthChanged(int i) {
	if(disable_use())
	{
		emit fiber_depth(this, i);
		SetAttribute(NaviWidgetFiber::fiber_save_depth.c_str(), QString("%1").arg(i).toStdString().c_str());
	}
	
}

void NaviWidgetFiber::OnNodeAdd(const itk::EventObject &)
{
}


void NaviWidgetFiber::onTargetNodeAdded() {
	int n = m_FiberData->m_TargetFiducialNode->GetNumberOfControlPoints();
	if (n == 1) {
		ui.btnAddTarget->setChecked(false);
	}
	CheckFiberAddActive();
}

void NaviWidgetFiber::onInputNodeAdded() {
	int n = m_FiberData->m_InputFiducialNode->GetNumberOfControlPoints();
	if (n == 1) {
		ui.btnAddEntry->setChecked(false);
	}
	CheckFiberAddActive();
}

void NaviWidgetFiber::OnClickBtnFiberPoint1(bool b) {
	if (b) {
		if (m_FiberData->m_ModelNode) {
			qGLMessageDialog::ShowWarnning(QStringLiteral("已创建光纤,如果要修正靶点请使用[路径微调]功能"));
			ui.btnAddTarget->setChecked(false);
			return;
		}
		auto displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_TargetFiducialNode->GetDisplayNode());
		if (!displayNode) {
			m_FiberData->m_TargetFiducialNode->CreateDefaultDisplayNodes();
			displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_TargetFiducialNode->GetDisplayNode());
		}
		displayNode->SetGlyphTypeFromString("Cross2D");
		displayNode->SetGlyphScale(3.0);
		m_FiberData->m_TargetFiducialNode->RemoveAllControlPoints();
		SetAttribute(NaviWidgetFiber::output_node_id.c_str(), m_FiberData->m_TargetFiducialNode->GetID());
		m_FiberData->m_Logic->AddNewDisplayNodeForMarkupsNode(m_FiberData->m_TargetFiducialNode);
		m_FiberData->m_Logic->SetActiveListID(m_FiberData->m_TargetFiducialNode);
		m_FiberData->m_Logic->SetPlaceMode();
		m_FiberData->m_TargetFiducialNode->SetLocked(true);
		m_FiberData->m_TargetFiducialNode->GetDisplayNode()->SetSelectedColor(m_FiberData->unSelectedColor);
		m_FiberData->m_State = NaviWidgetFiber::WaitForTargetPoint;

	}


}


void NaviWidgetFiber::OnClickBtnFiberPoint2(bool b) {
	if (b) {
		if (m_FiberData->m_ModelNode) {
			qGLMessageDialog::ShowWarnning(QStringLiteral("已创建光纤,如果要修正入点请使用[路径微调]功能"));
			ui.btnAddEntry->setChecked(false);
			return;
		}
		auto displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_InputFiducialNode->GetDisplayNode());
		if (!displayNode) {
			m_FiberData->m_InputFiducialNode->CreateDefaultDisplayNodes();
			displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(m_FiberData->m_InputFiducialNode->GetDisplayNode());
		}
		displayNode->SetGlyphTypeFromString("Diamond2D");
		displayNode->SetGlyphScale(3.0);
		SetAttribute(NaviWidgetFiber::input_node_id.c_str(), m_FiberData->m_InputFiducialNode->GetID());
		m_FiberData->m_Logic->AddNewDisplayNodeForMarkupsNode(m_FiberData->m_InputFiducialNode);
		m_FiberData->m_Logic->SetActiveListID(m_FiberData->m_InputFiducialNode);
		m_FiberData->m_Logic->SetPlaceMode();
		m_FiberData->m_InputFiducialNode->SetLocked(true);
		m_FiberData->m_InputFiducialNode->GetDisplayNode()->SetSelectedColor(m_FiberData->unSelectedColor);
		m_FiberData->m_State = NaviWidgetFiber::WaitForInputPoint;
	}
}

void NaviWidgetFiber::onGetColor(QColor color) {
	m_FiberData->m_red = color.red();
	m_FiberData->m_green = color.green();
	m_FiberData->m_blue = color.blue();
	SetAttribute(NaviWidgetFiber::color_red.c_str(), QStringLiteral("%1").arg(m_FiberData->m_red).toStdString().c_str());
	SetAttribute(NaviWidgetFiber::color_green.c_str(), QStringLiteral("%1").arg(m_FiberData->m_green).toStdString().c_str());
	SetAttribute(NaviWidgetFiber::color_blue.c_str(), QStringLiteral("%1").arg(m_FiberData->m_blue).toStdString().c_str());
	freshColor();
}

void NaviWidgetFiber::freshColor() {
	QString colorStr = QString("background-color:rgb(%1,%2,%3);").arg(m_FiberData->m_red).arg(m_FiberData->m_green).arg(m_FiberData->m_blue);
	ui.btnPalette->setStyleSheet(colorStr);
	double Color[4];
	Color[0] = m_FiberData->m_red / 255.0;
	Color[1] = m_FiberData->m_green / 255.0;
	Color[2] = m_FiberData->m_blue / 255.0;
	Color[3] = 0.5;
	if (m_FiberData->m_ModelNode) {
		m_FiberData->m_ModelNode->GetDisplayNode()->SetColor(Color);
	}
}

void NaviWidgetFiber::OnDeleteFiber() {
	if (m_FiberData->m_ModelNode) {
		m_FiberData->m_Scene->RemoveNode(m_FiberData->m_ModelNode);
	}
	if (m_FiberData->m_ModelNode2) {
		m_FiberData->m_Scene->RemoveNode(m_FiberData->m_ModelNode2);
	}
	if (m_FiberData->m_RedProjectionNode) {
		m_FiberData->m_Scene->RemoveNode(m_FiberData->m_RedProjectionNode);
	}
	if (m_FiberData->m_GreenProjectionNode) {
		m_FiberData->m_Scene->RemoveNode(m_FiberData->m_GreenProjectionNode);
	}
	if (m_FiberData->m_YellowProjectionNode) {
		m_FiberData->m_Scene->RemoveNode(m_FiberData->m_YellowProjectionNode);
	}
	
}

void NaviWidgetFiber::OnCreateFiber() {
	
	/*if (m_FiberData->m_ModelNode) {
		qGLMessageDialog::ShowWarnning(QStringLiteral("已创建光纤,如果要修正靶点请使用[路径微调]功能"));
		return;
	}*/
	ui.widgetSliderBkg->show();
	//ui.btn_add_2->setEnabled(true);
	//ui.btn_add_3->setEnabled(true);
	if (!m_FiberData->m_InputFiducialNode || !m_FiberData->m_TargetFiducialNode) //防止滚动选择光纤的时候改变了光纤类型
	{
		qGLMessageDialog::ShowWarnning(QStringLiteral("请创建靶点和入点"));
		return;
	}
	m_FiberData->m_LoadingModelFlag = true;
	auto list1 = m_BaseInfo.list;
	auto iomanager = qSlicerApplication::application()->ioManager();

	for (int i = 0; i < list1.length(); i++) {
		auto base_path = QApplication::applicationDirPath();
		QString path = QString("%1/fiber/%2.STL").arg(base_path).arg(list1[i]);
		//iomanager->loadFile(path);
		qSlicerIO::IOProperties properties;
		vtkCollection* loadedNodes = vtkCollection::New();
		properties.insert("fileName", path);
		iomanager->loadNodes("ModelFile", properties, loadedNodes);
		auto nodename = QString("%1.STL").arg(list1[i]);

		vtkMRMLNode* node = nullptr;
		vtkCollectionSimpleIterator it;
		for (loadedNodes->InitTraversal(it);
			(node = (vtkMRMLNode*)loadedNodes->GetNextItemAsObject(it));)
		{
			QString name1 = node->GetName();
			if (name1.startsWith(list1[i])) {
				break;
			}
		}

		//auto node = m_FiberData->m_Scene->GetFirstNodeByName(nodename.toStdString().c_str());
		vtkMRMLModelNode* model = reinterpret_cast<vtkMRMLModelNode*>(node);
		if (i == 0) {
			if(m_FiberData->m_ModelNode){
				/*for (int i = 0; i < m_FiberData->m_ModelNode->GetAttributeNames().size(); i++) {
					auto key = m_FiberData->m_ModelNode->GetAttributeNames().at(i).c_str();
					model->SetAttribute(key, m_FiberData->m_ModelNode->GetAttribute(key));
				}*/
				node->CopyContent(m_FiberData->m_ModelNode);
				model = reinterpret_cast<vtkMRMLModelNode*>(node);
			}
			else {
				
			   
			}
			OnDeleteFiber();

			m_FiberData->m_ModelNode = model;
			SetAttribute(NaviWidgetFiber::fiber_name.c_str(), ui.lineEdit->text().toStdString().c_str());
			SetAttribute(NaviWidgetFiber::fiber_type.c_str(), m_BaseInfo.name.toStdString().c_str(), i);
			SetAttribute(NaviWidgetFiber::input_node_id.c_str(), m_FiberData->m_InputFiducialNode->GetID(), i);
			SetAttribute(NaviWidgetFiber::output_node_id.c_str(), m_FiberData->m_TargetFiducialNode->GetID(), i);
			m_FiberData->m_ModelNode->CreateDefaultDisplayNodes();

			auto red_projection_node_name = QString::fromLatin1(model->GetID()) + "_red_projection";
			auto red_projection_node = vtkMRMLMarkupsLineNode::SafeDownCast(m_FiberData->m_Scene->AddNewNodeByClass("vtkMRMLMarkupsLineNode", red_projection_node_name.toStdString()));
			red_projection_node->SetAttribute("view_name", "red");
			red_projection_node->CreateDefaultDisplayNodes();
			auto red_display_node = vtkMRMLMarkupsDisplayNode::SafeDownCast(red_projection_node->GetDisplayNode());
			red_display_node->SetLineThickness(0.5);
			red_display_node->SetSelectedColor(0.7, 0.7, 0.7);
			red_display_node->SetPropertiesLabelVisibility(false);
			red_display_node->SetOpacity(0.2);
			m_FiberData->m_RedProjectionNode = red_projection_node;
			model->SetAttribute(NaviWidgetFiber::red_projection.c_str(), red_projection_node->GetID());
			SetAttribute(NaviWidgetFiber::stl_head.c_str(), "false");
			auto green_projection_node_name = QString::fromLatin1(model->GetID()) + "_green_projection";
			auto green_projection_node = vtkMRMLMarkupsLineNode::SafeDownCast(m_FiberData->m_Scene->AddNewNodeByClass("vtkMRMLMarkupsLineNode", green_projection_node_name.toStdString()));
			green_projection_node->SetAttribute("view_name", "green");
			green_projection_node->CreateDefaultDisplayNodes();
			auto green_display_node = vtkMRMLMarkupsDisplayNode::SafeDownCast(green_projection_node->GetDisplayNode());
			green_display_node->SetLineThickness(0.5);
			green_display_node->SetSelectedColor(0.7, 0.7, 0.7);
			green_display_node->SetPropertiesLabelVisibility(false);
			green_display_node->SetOpacity(0.2);
			m_FiberData->m_GreenProjectionNode = green_projection_node;
			model->SetAttribute(NaviWidgetFiber::green_projection.c_str(), green_projection_node->GetID());

			auto yellow_projection_node_name = QString::fromLatin1(model->GetID()) + "_yellow_projection";
			auto yellow_projection_node = vtkMRMLMarkupsLineNode::SafeDownCast(m_FiberData->m_Scene->AddNewNodeByClass("vtkMRMLMarkupsLineNode", yellow_projection_node_name.toStdString()));
			yellow_projection_node->SetAttribute("view_name", "yellow");
			yellow_projection_node->CreateDefaultDisplayNodes();
			auto yellow_display_node = vtkMRMLMarkupsDisplayNode::SafeDownCast(yellow_projection_node->GetDisplayNode());
			yellow_display_node->SetLineThickness(0.5);
			yellow_display_node->SetSelectedColor(0.7, 0.7, 0.7);
			yellow_display_node->SetPropertiesLabelVisibility(false);
			yellow_display_node->SetOpacity(0.2);
			m_FiberData->m_YellowProjectionNode = yellow_projection_node;
			model->SetAttribute(NaviWidgetFiber::yellow_projection.c_str(), yellow_projection_node->GetID());
		}
			
		if (i == 1) {
			m_FiberData->m_ModelNode2 = model;
			
			double Color[4];
			Color[0] =1;
			Color[1] = 0;
			Color[2] = 0;
			Color[3] = 0.5;
			if (model) {
				model->CreateDefaultDisplayNodes();
				model->GetDisplayNode()->SetColor(Color);
			}
			SetAttribute(NaviWidgetFiber::stl_head_node.c_str(), model->GetID());
		}
		model->SetName(QString("fiber_stl_%1").arg(i).toStdString().c_str());
		
		

		
	}
	SetFiberEnabled(true);
	DoUpdateFiberPosition();
	DoUpdateDistance();
	freshColor();
	SetAttribute(NaviWidgetFiber::is_on_edit.c_str(), "true");
	emit(update_frames());
}

void NaviWidgetFiber::SetFiberEnabled(bool b) {
	ctrl_vessel->setEnabled(b);
	ctrl_ablation->setEnabled(b);
	ctrl_protection->setEnabled(b);
	ui.horizontalSlider_2->setEnabled(b);
	ui.horizontalSlider_4->setEnabled(b);
	ui.button_target_2->setEnabled(b);
	ui.button_target->setEnabled(b);
	ui.button_input->setEnabled(b);
	ui.button_target_3->setEnabled(b);
	ui.cmbDir->setEnabled(b);
	ctrl_ablation->setToggle(b);
}

void NaviWidgetFiber::DoUpdateDistance() {
	double p1[3];
	m_FiberData->m_InputFiducialNode->GetNthControlPointPositionWorld(0, p1);
	double p2[3];
	m_FiberData->m_TargetFiducialNode->GetNthControlPointPositionWorld(0, p2);

	double distance = sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1]) + (p1[2] - p2[2]) * (p1[2] - p2[2]));
	QString s_distance = QString::number(distance, 'f', 1);
	ui.lblTotalDistance->setText(s_distance);
}

void NaviWidgetFiber::OnOpenColorPad()
{
	m_FiberData->m_pColorDialog = new QColorDialog;
	connect(m_FiberData->m_pColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(onGetColor(QColor)));
	m_FiberData->m_myPalette = ui.btnPalette->palette(); //按钮的调色板
	m_FiberData->m_pColorDialog->exec();
}

void NaviWidgetFiber::OnChangeVisible(bool b) {
		ui.btnVisible->setChecked(b);
		SetAttribute(NaviWidgetFiber::fiber_visible.c_str(), QStringLiteral("%1").arg(b).toStdString().c_str());
		{
			if (m_FiberData->m_ModelNode) {
				vtkMRMLModelDisplayNode*  dis = m_FiberData->m_ModelNode->GetModelDisplayNode();
				if (dis) {
					if (b){
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			if (m_FiberData->m_ModelNode2) {
				vtkMRMLModelDisplayNode* dis = m_FiberData->m_ModelNode2->GetModelDisplayNode();
				if (dis) {
					if (b) {
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			if (m_FiberData->m_FiberVesselNode) {
				m_FiberData->m_Scene->RemoveNode(m_FiberData->m_FiberVesselNode);
				m_FiberData->m_FiberVesselNode = nullptr;
			}
			if (m_FiberData->m_ProtectionNode) {
				auto* dis = m_FiberData->m_ProtectionNode->GetDisplayNode();
				if (dis) {
					if (b) {
						if(m_FiberData->m_ProtectionNodeVisibility)
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			if (m_FiberData->m_RedProjectionNode) {
				auto* dis = m_FiberData->m_RedProjectionNode->GetDisplayNode();
				if (dis) {
					if (b) {
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			if (m_FiberData->m_GreenProjectionNode) {
				auto* dis = m_FiberData->m_GreenProjectionNode->GetDisplayNode();
				if (dis) {
					if (b) {
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			if (m_FiberData->m_YellowProjectionNode) {
				auto* dis = m_FiberData->m_YellowProjectionNode->GetDisplayNode();
				if (dis) {
					if (b) {
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			if (m_FiberData->m_InputFiducialNode) {
				vtkMRMLDisplayNode* dis = m_FiberData->m_InputFiducialNode->GetDisplayNode();
				if (dis) {
					if (b) {
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			if (m_FiberData->m_TargetFiducialNode) {
				vtkMRMLDisplayNode* dis = m_FiberData->m_TargetFiducialNode->GetDisplayNode();
				if (dis) {
					if (b) {
						dis->SetVisibility(true);
					}
					else {
						dis->SetVisibility(false);
					}
				}
			}
			OnChangeAblactionVisible(b);
		}
}

void NaviWidgetFiber::SetAttribute(const char* key, const char* val,int i) {
	if (m_FiberData) {
		if (m_FiberData->m_ModelNode && i==0) {
			m_FiberData->m_ModelNode->SetAttribute(key, val);
		}
		if (m_FiberData->m_ModelNode2 && i == 1) {
			m_FiberData->m_ModelNode2->SetAttribute(key, val);
		}
	}
}


void NaviWidgetFiber::OnChangeAblactionVisible(bool b) {
	if (m_FiberData->m_SegmentationNode) {
		
		vtkMRMLDisplayNode* dis = m_FiberData->m_SegmentationNode->GetDisplayNode();
		if (dis) {
			if (b) {
				SetAttribute(NaviWidgetFiber::fiber_ablation_visible.c_str(), "true");
				dis->SetVisibility(true);
			}
			else {
				SetAttribute(NaviWidgetFiber::fiber_ablation_visible.c_str(), "false");
				dis->SetVisibility(false);
			}
		}
	}
}


void NaviWidgetFiber::SetDeleteable(bool b){
}

void NaviWidgetFiber::SetVisibleEnabled(bool b) {
}


void NaviWidgetFiber::DeleteFromScene() {
	if (!m_FiberData->m_Scene) {
		return;
	}
	if (m_FiberData->m_ModelNode) {
		DoSafeRemove(m_FiberData->m_ModelNode->GetParentTransformNode());
	}
	if (m_FiberData->m_ModelNode) {
		DoSafeRemove(m_FiberData->m_ModelNode);
	}
	if (m_FiberData->m_ModelNode2) {
		DoSafeRemove(m_FiberData->m_ModelNode2);
	}
	if (m_FiberData->m_InputFiducialNode) {
		DoSafeRemove(m_FiberData->m_InputFiducialNode);
	}
	if (m_FiberData->m_TargetFiducialNode) {
		DoSafeRemove(m_FiberData->m_TargetFiducialNode);
	}
	if (m_FiberData->m_ProtectionNode) {
		DoSafeRemove(m_FiberData->m_ProtectionNode);
		m_FiberData->m_ProtectionNode = nullptr;
		m_FiberData->m_ProtectionNodeVisibility = false;
	}
	if (m_FiberData->m_SegmentationNode) {
		DoSafeRemove(m_FiberData->m_SegmentationNode);
		m_FiberData->m_SegmentationNode = nullptr;
	}
	if (m_FiberData->m_FiberVesselNode) {
		DoSafeRemove(m_FiberData->m_FiberVesselNode);
		m_FiberData->m_FiberVesselNode = nullptr;
	}
	if (m_FiberData->m_RedProjectionNode) {
		DoSafeRemove(m_FiberData->m_RedProjectionNode);
	}
	if (m_FiberData->m_GreenProjectionNode) {
		DoSafeRemove(m_FiberData->m_GreenProjectionNode);
	}
	if (m_FiberData->m_YellowProjectionNode) {
		DoSafeRemove(m_FiberData->m_YellowProjectionNode);
	}
	m_FiberData->m_Logic->RemoveObserver(this->m_CallbackCommand);
}


void NaviWidgetFiber::DoSafeRemove(vtkMRMLNode* node) {
	if (node) {
		if (m_FiberData->m_Scene->IsNodePresent(node) == 0) {
			return;
		}
		else {
			m_FiberData->m_Scene->RemoveNode(node);
		}
	}
}


NaviWidgetFiber::~NaviWidgetFiber()
{
	m_FiberData->m_Logic->RemoveObserver(this->m_CallbackCommand);
	m_FiberData = nullptr;

}

SwitchControl* NaviWidgetFiber::CreateSwitch(QHBoxLayout* layout)
{
	SwitchControl* pSwitchControl = new SwitchControl(this);
	pSwitchControl->setAttribute(Qt::WA_StyledBackground);
	pSwitchControl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	pSwitchControl->setMinimumWidth(40);
	// 设置状态、样式
	pSwitchControl->setToggle(false);
	pSwitchControl->setCheckedColor(QColor(0, 160, 230));
	layout->insertWidget(3, pSwitchControl, 0, Qt::Alignment());
	return pSwitchControl;
}
