/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QTimer>
// Slicer includes
#include "qSlicerMeasureModuleWidget.h"
#include "ui_qSlicerMeasureModuleWidget.h"

#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerCLIModuleLogic.h"
#include "ModuleDescription.h"
#include "vtkSlicerCLIModuleLogic.h"
#include "qSlicerCLIModuleWidget.h"
#include "vtkMRMLCommandLineModuleNode.h"
#include "qSlicerModuleManager.h"
#include "qSlicerCLIModule.h"
#include "vtkMRMLCommandLineModuleNode.h"
#include "itkVTKImageToImageFilter.h"
#include "vtkSlicerMeasureLogic.h"
#include "vtkMRMLInteractionNode.h"  
#include "vtkMRMLMarkupsNode.h"
#include <qMRMLSliceControllerWidget.h>
#include "qMRMLSliceWidget.h"
#include "vtkMRMLViewNode.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDViewControllerWidget.h"
#include "vtkMRMLSliceViewInteractorStyle.h"
#include "qMRMLSliceView.h"
#include "qdir.h"
#include "qdatetime.h"
#include "QScreen.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkMRMLCrosshairDisplayableManager.h"
#include "vtkMRMLSliceIntersectionWidget.h"
#include "qSlicerMainWindow.h"
#include "vtkEvent.h"
#include "vtkMRMLMarkupsLineNode.h"
#include <QTimer>
#include "qGLMessageDialog.h"
#include "qtabbar.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMeasureModuleWidgetPrivate : public Ui_qSlicerMeasureModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerMeasureModuleWidget);
protected:
	qSlicerMeasureModuleWidget* const q_ptr;
public:
	int m_Index = 1;
	qSlicerMeasureModuleWidgetPrivate(qSlicerMeasureModuleWidget& object);
	~qSlicerMeasureModuleWidgetPrivate();
	void setSelectionNodeActivePlaceNode(vtkMRMLNode* activePlaceNode);
	void setPlaceModeEnabled(bool placeEnable);
	vtkSlicerMeasureLogic* logic() const;
};





void qSlicerMeasureModuleWidgetPrivate::setPlaceModeEnabled(bool placeEnable)
{
	Q_Q(qSlicerMeasureModuleWidget);
	vtkMRMLInteractionNode* interactionNode = nullptr;
	if (q->mrmlScene())
	{
		interactionNode = vtkMRMLInteractionNode::SafeDownCast(q->mrmlScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
	}
	if (interactionNode == nullptr)
	{
		if (placeEnable)
		{
			qCritical() << Q_FUNC_INFO << " setPlaceModeEnabled failed: invalid interaction node";
		}
		return;
	}

	if (placeEnable)
	{
		interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);
	}
	else
	{
		if (interactionNode->GetCurrentInteractionMode() == vtkMRMLInteractionNode::Place)
		{
			interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);
		}
	}
}

//-----------------------------------------------------------------------------
// qSlicerMeasureModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerMeasureModuleWidgetPrivate::qSlicerMeasureModuleWidgetPrivate(qSlicerMeasureModuleWidget& object) : q_ptr(&object)
{
}
qSlicerMeasureModuleWidgetPrivate::~qSlicerMeasureModuleWidgetPrivate() {}

vtkSlicerMeasureLogic* qSlicerMeasureModuleWidgetPrivate::logic() const {
	Q_Q(const qSlicerMeasureModuleWidget);
	return vtkSlicerMeasureLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
void qSlicerMeasureModuleWidgetPrivate::setSelectionNodeActivePlaceNode(vtkMRMLNode* activePlaceNode)
{
	Q_Q(qSlicerMeasureModuleWidget);
	if (!logic())
	{
		return;
	}
	logic()->SetActiveListID(vtkMRMLMarkupsNode::SafeDownCast(activePlaceNode));
}

//-----------------------------------------------------------------------------
// qSlicerMeasureModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerMeasureModuleWidget::qSlicerMeasureModuleWidget(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSlicerMeasureModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerMeasureModuleWidget::~qSlicerMeasureModuleWidget()
{
	Q_D(qSlicerMeasureModuleWidget);
}

//-----------------------------------------------------------------------------
void qSlicerMeasureModuleWidget::setup()
{
  Q_D(qSlicerMeasureModuleWidget);
  d->setupUi(this);
  d->label_11->hide();
  d->tabWidget->tabBar()->hide();
  measurePanel = new MeasureToolPanel();
  d->verticalLayout->addWidget(measurePanel, 0, 0);
  connect(d->pBtnZoomin, SIGNAL(toggled(bool)), this, SLOT(OnRotate(bool)));
  connect(d->pBtnZoomin_2, SIGNAL(toggled(bool)), this, SLOT(OnBasicPoint(bool)));
  connect(d->pBtnZoomout, SIGNAL(toggled(bool)), this, SLOT(OnZoomOut(bool)));
  connect(d->pBtnMove, SIGNAL(toggled(bool)), this, SLOT(OnTranslate(bool)));
  connect(d->pBtnWhirling, SIGNAL(clicked()), this, SLOT(OnScreenShot()));
  connect(d->pBtnGrayscale, SIGNAL(toggled(bool)), this, SLOT(OnGrayScale(bool)));
  connect(d->pBtnFourView, SIGNAL(clicked()), this, SLOT(OnViewType0())); //四视图
  connect(d->pBtnAllView, SIGNAL(clicked()), this, SLOT(OnViewType1())); //六视图
  connect(d->pBtnThreeView, SIGNAL(clicked()), this, SLOT(OnViewType2())); //三视图
  connect(d->pBtnReset, SIGNAL(clicked()), this, SLOT(OnReset())); //图像复位
  connect(d->pBtnMeasure, SIGNAL(toggled(bool)), this, SLOT(OpenMeasureToolPanel(bool)));
  
  //m_timer = new QTimer(this);
  //m_timer->stop();
  //m_timer->setInterval(1000);
  //connect(m_timer, SIGNAL(timeout()), this, SLOT(Ontimeout()));

  qvtkConnect(logic(), 9997,this, SLOT(OnSignalChangeLayoutTo4()));
  qvtkConnect(logic(), 9998, this, SLOT(OnSignalChangeLayoutTo3()));
  qvtkConnect(logic(), 9999, this, SLOT(OnSignalChangeLayoutTo6()));
  qvtkConnect(logic(), 9995, this, SLOT(OnSignalChangeLayoutTo5()));


  //auto settings = qSlicerApplication::application()->settings;
  //int acpc = settings->value("additional_plugins/acpc").toInt();
 /* if (false) {
	  //d->pBtnACPC->setVisible(false);
	  d->label_10->setVisible(false);
  }
  else {
	  //d->pBtnACPC->setVisible(true);
	  d->label_10->setVisible(true);
	  connect(//d->pBtnACPC, SIGNAL(clicked()), this, SLOT(OnOpenACPCPanel()));
  }*/
 
}

void qSlicerMeasureModuleWidget::onSliceCompositeNodeModified() {
	Q_D(qSlicerMeasureModuleWidget);
	vtkMRMLSliceCompositeNode* compositeNode = nullptr;
	int numberOfCompositeNodes = mrmlScene()->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");
	for (int i = 0; i < numberOfCompositeNodes; i++)
	{
		compositeNode = vtkMRMLSliceCompositeNode::SafeDownCast(mrmlScene()->GetNthNodeByClass(i, "vtkMRMLSliceCompositeNode"));
		if (!compositeNode) {
			continue;
		}
		auto backgroundNodeID = compositeNode->GetBackgroundVolumeID();
		if (backgroundNodeID) {
			auto backgroundNode = mrmlScene()->GetNodeByID(backgroundNodeID);
			if (backgroundNode) {
				d->lwWidget1->setMRMLVolumeNode(backgroundNode);
			}
		}
		break;
	}
}

void qSlicerMeasureModuleWidget::OnOpenACPCPanel() {
	/*Q_D(qSlicerMeasureModuleWidget);
	OnPreOperation("OpenACPCToolPanel", true);
	std::cout << "OnOpenACPCPanel" << std::endl;
	qSlicerMainWindow* mw = dynamic_cast<qSlicerMainWindow*>(qSlicerApplication::application()->mainWindow());
	qSlicerLayoutManager* lm = qSlicerApplication::application()->layoutManager();
	lm->setCurrentPage(10);*/
	
}

void qSlicerMeasureModuleWidget::enter() {
	this->Superclass::enter();
	vtkMRMLSliceCompositeNode* compositeNode = nullptr;
	int numberOfCompositeNodes = mrmlScene()->GetNumberOfNodesByClass("vtkMRMLSliceCompositeNode");
	for (int i = 0; i < numberOfCompositeNodes; i++)
	{
		compositeNode = vtkMRMLSliceCompositeNode::SafeDownCast(mrmlScene()->GetNthNodeByClass(i, "vtkMRMLSliceCompositeNode"));
		qvtkReconnect(compositeNode, vtkCommand::ModifiedEvent, this, SLOT(onSliceCompositeNodeModified()));
		break;
	}

}

void qSlicerMeasureModuleWidget::OpenMeasureToolPanel(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnPreOperation("OpenMeasureToolPanel", checked);

	bool flag = d->pBtnMeasure->isChecked();
	bool flag2 = d->pBtnMeasure->isDown();
	if (checked)
	{
		measurePanel->OnReset();
		d->tabWidget->setCurrentIndex(1);
		connect(measurePanel, SIGNAL(drawLineTriggered()), this, SLOT(OnDrawLineTriggered()));
		connect(measurePanel, SIGNAL(drawAreaTriggered()), this, SLOT(OnDrawPolygonTriggered()));
		connect(measurePanel, SIGNAL(drawAngleTriggered()), this, SLOT(OnDrawAngleTriggered()));
		connect(measurePanel, SIGNAL(drawPointTriggered()), this, SLOT(OnDrawPointTriggered()));
		connect(measurePanel, SIGNAL(drawOpenCurveTriggered()), this, SLOT(OnDrawROITriggered()));
		connect(measurePanel, SIGNAL(drawPlaneTriggered()), this, SLOT(OnDrawPlaneTriggered()));
		connect(measurePanel, SIGNAL(show_off(bool)), this, SLOT(OnMeasureShowOff(bool)));
	}
	else
	{
		d->tabWidget->setCurrentIndex(2);
		d->setPlaceModeEnabled(false);
	}
	OnPostOperation("OpenMeasureToolPanel", checked);
}


void qSlicerMeasureModuleWidget::OnMeasureShowOff(bool b) {
	Q_D(qSlicerMeasureModuleWidget);
	auto dn = mrmlScene()->GetNodesByClass("vtkMRMLMarkupsDisplayNode");
	int numberOfSceneViews = dn->GetNumberOfItems();
	for (int n = 0; n < numberOfSceneViews; ++n)
	{
		vtkMRMLDisplayNode* displayNode =
			vtkMRMLDisplayNode::SafeDownCast(dn->GetItemAsObject(n));
		displayNode->SetVisibility(b);
	}
}

void qSlicerMeasureModuleWidget::OnPreOperation(QString title,bool checked) {
	Q_D(qSlicerMeasureModuleWidget);
	if (title == "OnBasicPoint") {
		if (checked) {
			d->pBtnZoomin_2->setAutoExclusive(false);
			d->pBtnZoomin->setChecked(false);
			d->pBtnZoomout->setChecked(false);
			d->pBtnMove->setChecked(false);
			d->pBtnGrayscale->setChecked(false);
			d->pBtnMeasure->setChecked(false);
			d->pBtnReset->setChecked(false);
		}
		else
		{
			d->pBtnZoomin_2->setAutoExclusive(true);
		}
	}
	if (title == "OnRotate") {
		if (checked) {
			d->pBtnZoomin->setAutoExclusive(false);
			d->pBtnZoomin_2->setChecked(false);
			d->pBtnZoomout->setChecked(false);
			d->pBtnMove->setChecked(false);
			d->pBtnGrayscale->setChecked(false);
			d->pBtnMeasure->setChecked(false);
			d->pBtnReset->setChecked(false);
		}
		else
		{
			d->pBtnZoomin->setAutoExclusive(true);
		}
	}
	if (title == "OnZoomOut") {
		if (checked) {
			d->pBtnZoomout->setAutoExclusive(false);
			d->pBtnZoomin->setChecked(false);
			d->pBtnZoomin_2->setChecked(false);
			d->pBtnMove->setChecked(false);
			d->pBtnGrayscale->setChecked(false);
			d->pBtnMeasure->setChecked(false);
			d->pBtnReset->setChecked(false);
		}
		else
		{
			d->pBtnZoomout->setAutoExclusive(true);
		}
	}
	if (title == "OnTranslate") {
		if (checked) {
			d->pBtnMove->setAutoExclusive(false);
			d->pBtnZoomin->setChecked(false);
			d->pBtnZoomin_2->setChecked(false);
			d->pBtnZoomout->setChecked(false);
			d->pBtnGrayscale->setChecked(false);
			d->pBtnMeasure->setChecked(false);
			d->pBtnReset->setChecked(false);
		}
		else
		{
			d->pBtnMove->setAutoExclusive(true);
		}
	}
	if (title == "OnGrayScale") {
		if (checked) {
			d->pBtnGrayscale->setAutoExclusive(false);
			d->pBtnZoomin->setChecked(false);
			d->pBtnZoomin_2->setChecked(false);
			d->pBtnZoomout->setChecked(false);
			d->pBtnMove->setChecked(false);
			d->pBtnMeasure->setChecked(false);
			d->pBtnReset->setChecked(false);
		}
		else
		{
			d->pBtnGrayscale->setAutoExclusive(true);
		}
	}
	if (title == "OpenMeasureToolPanel") {
		if (checked) {
			d->pBtnMeasure->setAutoExclusive(false);
			d->pBtnZoomin->setChecked(false);
			d->pBtnZoomin_2->setChecked(false);
			d->pBtnZoomout->setChecked(false);
			d->pBtnMove->setChecked(false);
			d->pBtnGrayscale->setChecked(false);
			d->pBtnReset->setChecked(false);
			//d->pBtnACPC->setChecked(false);
		}
		else
		{
			d->pBtnMeasure->setAutoExclusive(true);
		}
	}
	if (title == "OnReset") {
		if (checked) {
			d->pBtnReset->setAutoExclusive(false);
			d->pBtnZoomin->setChecked(false);
			d->pBtnZoomin_2->setChecked(false);
			d->pBtnZoomout->setChecked(false);
			d->pBtnMove->setChecked(false);
			d->pBtnGrayscale->setChecked(false);
			d->pBtnMeasure->setChecked(false);
		}
		else
		{
			d->pBtnReset->setAutoExclusive(true);
		}
	}
	if (title == "OpenACPCToolPanel") {
		if (checked) {
			//d->pBtnACPC->setAutoExclusive(false);
			d->pBtnZoomin->setChecked(false);
			d->pBtnZoomin_2->setChecked(false);
			d->pBtnZoomout->setChecked(false);
			d->pBtnMove->setChecked(false);
			d->pBtnGrayscale->setChecked(false);
			d->pBtnMeasure->setChecked(false);
			d->pBtnReset->setChecked(false);
		}
		else
		{
			//d->pBtnACPC->setAutoExclusive(true);
		}
	}
}

void qSlicerMeasureModuleWidget::OnPostOperation(QString title, bool checked) {
	Q_D(qSlicerMeasureModuleWidget);
}

void qSlicerMeasureModuleWidget::OnBasicPoint(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnPreOperation("OnBasicPoint", checked);
	vtkSmartPointer<vtkCollection> nodes;
	nodes.TakeReference(mrmlScene()->GetNodesByClass("vtkMRMLSliceCompositeNode"));
	if (!nodes.GetPointer())
	{
		return;
	}
	vtkMRMLSliceCompositeNode* node = nullptr;
	vtkCollectionSimpleIterator it;
	for (nodes->InitTraversal(it); (node = static_cast<vtkMRMLSliceCompositeNode*>(
		nodes->GetNextItemAsObject(it)));)
	{
		if (checked) {
			node->SetSliceIntersectionVisibility(true);
			d->label_10->setText(QStringLiteral("基点:通过左键可以设置基点,从而更方便在二维视图上进行查看"));
			d->tabWidget->setCurrentIndex(0);
		}
		else {
			node->SetSliceIntersectionVisibility(false);
			d->tabWidget->setCurrentIndex(2);
		}
	}

	auto layoutManager = qSlicerApplication::application()->layoutManager();
	auto names = layoutManager->sliceViewNames();
	for (int i = 0; i < names.size(); i++) {
		auto name1 = names.at(i);
		auto controller = layoutManager->sliceWidget(name1)->sliceController();
		controller->setSliceVisible(checked);
	}

	OnPostOperation("OnBasicPoint", checked);
	
	/*if (checked)
		qSlicerApplication::application()->layoutManager()->setCurrentPage(1001);
	else
		qSlicerApplication::application()->layoutManager()->setCurrentPage(qSlicerApplication::application()->layoutManager()->getOldPage());*/
}

void qSlicerMeasureModuleWidget::OnRotate(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnPreOperation("OnRotate",checked);
	std::vector<vtkMRMLNode*> sliceWidgets;
	QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
	foreach(const QString & viewName, names)
	{
		vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
			qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
		vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
		if(checked){
			sw->UpdateLeftButtonEventByModifiers(12);
		}
		else {
			sw->UpdateInteractionEventMapping();
		}
	}
	vtkSmartPointer<vtkCollection> nodes;
	nodes.TakeReference(mrmlScene()->GetNodesByClass("vtkMRMLSliceCompositeNode"));
	if (!nodes.GetPointer())
	{
		return;
	}
	vtkMRMLSliceCompositeNode* node = nullptr;
	vtkCollectionSimpleIterator it;
	for (nodes->InitTraversal(it); (node = static_cast<vtkMRMLSliceCompositeNode*>(
		nodes->GetNextItemAsObject(it)));)
	{
		if (checked) {
			node->SetSliceIntersectionVisibility(true);
			d->label_10->setText(QStringLiteral("旋转:使用鼠标拖拽图像上的十字线来进行旋转\n      如果要更改旋转中心点,请使用[基点]工具"));
			d->tabWidget->setCurrentIndex(0);
		}
		else {
			node->SetSliceIntersectionVisibility(false);
			d->tabWidget->setCurrentIndex(2);
		}
	}
	OnPostOperation("OnRotate", checked);
}

void qSlicerMeasureModuleWidget::OnZoomOut(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnPreOperation("OnZoomOut", checked);
	std::vector<vtkMRMLNode*> sliceWidgets;
	QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
	foreach(const QString & viewName, names)
	{
		vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
			qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
		vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
		qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceController()->setSliceLink(true);
		if(checked){
			sw->UpdateLeftButtonEventByModifiers(vtkEvent::AltModifier);
			d->label_10->setText(QStringLiteral("缩放:使用鼠标左键在二维图像上按下并进行拖动"));
			d->tabWidget->setCurrentIndex(0);
		}
		else {
			sw->UpdateInteractionEventMapping();
			d->tabWidget->setCurrentIndex(2);
		}
	}
	OnPostOperation("OnZoomOut", checked);
}

void qSlicerMeasureModuleWidget::OnTranslate(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnPreOperation("OnTranslate", checked);
	std::vector<vtkMRMLNode*> sliceWidgets;
	QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
	foreach(const QString & viewName, names)
	{
		vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
			qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
		vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
		
		if (checked) {
			sw->UpdateLeftButtonEventByModifiers(vtkEvent::ShiftModifier);
			d->label_10->setText(QStringLiteral("平移:使用鼠标左键在二维图像上按下并进行拖动"));
			d->tabWidget->setCurrentIndex(0);
		}
		else {
			d->tabWidget->setCurrentIndex(2);
			sw->UpdateInteractionEventMapping();
		}
	}
	OnPostOperation("OnTranslate", checked);
}

void qSlicerMeasureModuleWidget::OnGrayScale(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnPreOperation("OnGrayScale", checked);
	{
		vtkMRMLInteractionNode* interactionNode = nullptr;
		if (mrmlScene())
		{
			interactionNode = vtkMRMLInteractionNode::SafeDownCast(mrmlScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
		} 
	
		if (interactionNode != nullptr)
		{
			if (checked) {
				interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::AdjustWindowLevel);
				
				d->tabWidget->setCurrentIndex(3);
			}
			
			else {
				interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);
				d->tabWidget->setCurrentIndex(2);
			}
			
		}
	}
	OnPostOperation("OnGrayScale", checked);
}

//重置按钮
void qSlicerMeasureModuleWidget::OnResetButton(QString bn) {
	Q_D(qSlicerMeasureModuleWidget);
	auto interactionNode = vtkMRMLInteractionNode::SafeDownCast(mrmlScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
	if(interactionNode)
		interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);


	vtkSmartPointer<vtkCollection> nodes;
	nodes.TakeReference(mrmlScene()->GetNodesByClass("vtkMRMLSliceCompositeNode"));
	if (!nodes.GetPointer())
	{
		return;
	}
	vtkMRMLSliceCompositeNode* node = nullptr;
	vtkCollectionSimpleIterator it;
	for (nodes->InitTraversal(it); (node = static_cast<vtkMRMLSliceCompositeNode*>(
		nodes->GetNextItemAsObject(it)));)
	{
		node->SetSliceIntersectionVisibility(false);
	}

	if(bn == "reset")
	{
		std::vector<vtkMRMLNode*> sliceWidgets;
		QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
		foreach(const QString & viewName, names)
		{
			vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
				qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
			vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
			sw->UpdateInteractionEventMapping();
		}
	}
}

void qSlicerMeasureModuleWidget::Ontimeout()
{
	Q_D(qSlicerMeasureModuleWidget);
	//m_timer->stop();
	//d->label_11->show();
	//d->pBtnWhirling->setStyleSheet("border-image: url(:/blue/tool_screenshot_disable.png)");
	//qGLMessageDialog::ShowInformation(QStringLiteral("截图完成!"), true);
	//d->pBtnWhirling->setStyleSheet("background-color:transparent");
}
void qSlicerMeasureModuleWidget::OnScreenShot()
{
	Q_D(qSlicerMeasureModuleWidget);
	//d->pBtnWhirling->setStyleSheet("background-color:#cbd8e9");
	//d->pBtnWhirling->setStyleSheet("border-image: url(:/blue/printScreen.png)");
	//m_timer->start();

	QString fileName = QCoreApplication::applicationDirPath();
	QString sName = fileName.append("/ScreenShot/");

	QDir* folder = new QDir;
	bool exist = folder->exists(sName);
	if (!exist)
	{
		bool ok = folder->mkdir(sName);
	}

	exist = folder->exists(sName);
	if (!exist) {
		return;
	}

	QScreen* screen = QGuiApplication::primaryScreen();
	QString filePathNameall = fileName + QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz") + ".jpg";
	screen->grabWindow(0).save(filePathNameall, "jpg");  //抓取屏幕（可以抓取不属于应用程序的窗口）
 	d->label_11->show();
 	QTimer::singleShot(1000,[=]() {
 		d->label_11->hide();
		d->pBtnWhirling->setChecked(false);
 		});
}

void qSlicerMeasureModuleWidget::OnSceenShot(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("OnSceenShot");
	d->pBtnWhirling->setDown(true);
}

void qSlicerMeasureModuleWidget::OnSignalChangeLayoutTo3() {
	Q_D(qSlicerMeasureModuleWidget);
	d->pBtnThreeView->animateClick(100);
}
void qSlicerMeasureModuleWidget::OnSignalChangeLayoutTo4() {
	Q_D(qSlicerMeasureModuleWidget);
	d->pBtnFourView->animateClick(100);
}

void qSlicerMeasureModuleWidget::OnSignalChangeLayoutTo5() {
	Q_D(qSlicerMeasureModuleWidget);
	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
	vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
	if (lnode) {
		lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutOneUpRedSliceView);
	}
	//DoFreshRoomState();
}

void qSlicerMeasureModuleWidget::OnSignalChangeLayoutTo6() {
	Q_D(qSlicerMeasureModuleWidget);
	d->pBtnAllView->animateClick(100);
}

void qSlicerMeasureModuleWidget::OnViewType0() {
	Q_D(qSlicerMeasureModuleWidget);
	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
	vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
	if (lnode) {
		lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutConventionalView);
	}
	//DoFreshRoomState();
}
void qSlicerMeasureModuleWidget::OnViewType1() {
	Q_D(qSlicerMeasureModuleWidget);
	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
	vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
	if (lnode) {
		lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutThreeOverThreeView);
	}
	//DoFreshRoomState();
}
void qSlicerMeasureModuleWidget::OnViewType2() {
	Q_D(qSlicerMeasureModuleWidget);
	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
	vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
	if (lnode) {
		lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutConventionalPlotView);
	}
	//DoFreshRoomState();
}
void qSlicerMeasureModuleWidget::OnReset() {
	Q_D(qSlicerMeasureModuleWidget);
	//OnPreOperation("OnReset", checked);
	OnResetButton("reset");
	Reinit();
	//OnPostOperation("OnReset", checked);
}
void qSlicerMeasureModuleWidget::OnFresh() {
	Q_D(qSlicerMeasureModuleWidget);
}

void qSlicerMeasureModuleWidget::OnDrawPointTriggered() 
{
	Q_D(qSlicerMeasureModuleWidget);
	
	if (measurePanel)
	{
		this->onActiveMarkupMRMLNodeAdded(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsFiducialNode"));
	}
}

void qSlicerMeasureModuleWidget::OnDrawPlaneTriggered()
{
	Q_D(qSlicerMeasureModuleWidget);
	
	if (measurePanel)
	{
		this->onActiveMarkupMRMLNodeAdded(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsPlaneNode"));
	}
}

void qSlicerMeasureModuleWidget::OnDrawROITriggered() {
	Q_D(qSlicerMeasureModuleWidget);
	
	if (measurePanel)
	{
		this->onActiveMarkupMRMLNodeAdded(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsCurveNode"));
	}
}

void qSlicerMeasureModuleWidget::OnDrawLineTriggered()
{
	Q_D(qSlicerMeasureModuleWidget);
	
	if (measurePanel)
	{
		auto lineNodet = qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsLineNode");
		auto lineNode = vtkMRMLMarkupsLineNode::SafeDownCast(lineNodet);
		auto displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(lineNode->GetDisplayNode());
		displayNode->SetPointLabelsVisibility(false);
		this->onActiveMarkupMRMLNodeAdded(lineNode);
	}
}

void qSlicerMeasureModuleWidget::OnDrawPolygonTriggered()
{
	Q_D(qSlicerMeasureModuleWidget);
	
	if (measurePanel)
	{
		if (this->mrmlScene())
		{
			this->onActiveMarkupMRMLNodeAdded(this->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsClosedCurveNode"));
		}
	}
}

void qSlicerMeasureModuleWidget::OnDrawAngleTriggered()
{
	Q_D(qSlicerMeasureModuleWidget);
	
	if (measurePanel)
	{
		if (this->mrmlScene())
		{
			this->onActiveMarkupMRMLNodeAdded(this->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsAngleNode"));
		}
	}
}

void qSlicerMeasureModuleWidget::onActiveMarkupMRMLNodeAdded(vtkMRMLNode* markupsNode)
{
	Q_D(qSlicerMeasureModuleWidget);
	if (d->logic())
	{
		d->logic()->AddNewDisplayNodeForMarkupsNode(markupsNode);
	}
	// make sure it's set up for the mouse mode tool bar to easily add points to
	// it by making it active in the selection node
	d->setSelectionNodeActivePlaceNode(markupsNode);
	d->setPlaceModeEnabled(true);
}



