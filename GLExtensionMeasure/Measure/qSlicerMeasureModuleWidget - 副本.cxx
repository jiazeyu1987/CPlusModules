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
#include <QTimer>
#include "qGLMessageDialog.h"
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

  measurePanel = new MeasureToolPanel();
  connect(d->pBtnZoomin, SIGNAL(toggled(bool)), this, SLOT(OnZoomIn(bool)));
  connect(d->pBtnZoomin_2, SIGNAL(toggled(bool)), this, SLOT(OnZoomIn2(bool)));
  connect(d->pBtnZoomout, SIGNAL(toggled(bool)), this, SLOT(OnZoomOut(bool)));
  connect(d->pBtnMove, SIGNAL(toggled(bool)), this, SLOT(OnTranslate(bool)));
  connect(d->pBtnWhirling, SIGNAL(clicked()), this, SLOT(OnScreenShot()));
  connect(d->pBtnGrayscale, SIGNAL(toggled(bool)), this, SLOT(OnGrayScale(bool)));
  connect(d->pBtnFourView, SIGNAL(clicked()), this, SLOT(OnViewType0())); //四视图
  connect(d->pBtnAllView, SIGNAL(clicked()), this, SLOT(OnViewType1())); //六视图
  connect(d->pBtnThreeView, SIGNAL(clicked()), this, SLOT(OnViewType2())); //三视图
  connect(d->pBtnReset, SIGNAL(toggled(bool)), this, SLOT(OnReset(bool))); //图像复位
  connect(d->pBtnMeasure, SIGNAL(toggled(bool)), this, SLOT(OpenMeasureToolPanel()));
  
  m_timer = new QTimer(this);
  m_timer->stop();
  m_timer->setInterval(1000);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(Ontimeout()));

  qvtkConnect(logic(), 9997,this, SLOT(OnSignalChangeLayoutTo4()));
  qvtkConnect(logic(), 9998, this, SLOT(OnSignalChangeLayoutTo3()));
  qvtkConnect(logic(), 9999, this, SLOT(OnSignalChangeLayoutTo6()));
  qvtkConnect(logic(), 9995, this, SLOT(OnSignalChangeLayoutTo5()));

  d->widget_2->hide();

  auto settings = qSlicerApplication::application()->settings;
  int acpc = settings->value("additional_plugins/acpc").toInt();
  if (false) {
	  d->pBtnACPC->setVisible(false);
	  d->label_10->setVisible(false);
  }
  else {
	  d->pBtnACPC->setVisible(true);
	  d->label_10->setVisible(true);
	  connect(d->pBtnACPC, SIGNAL(toggled(bool)), this, SLOT(OnOpenACPCPanel(bool)));
  }
}

void qSlicerMeasureModuleWidget::OnOpenACPCPanel(bool checked) {
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("ACPC");
	//bool flag = d->pBtnACPC->isChecked();
	if (checked)
	{
		d->pBtnACPC->setAutoExclusive(false);
		d->pBtnACPC->setStyleSheet("border-image: url(:/blue/tool_acpc.png)");
		std::cout << "OnOpenACPCPanel" << std::endl;
		qSlicerMainWindow* mw = dynamic_cast<qSlicerMainWindow*>(qSlicerApplication::application()->mainWindow());
		qSlicerLayoutManager* lm = qSlicerApplication::application()->layoutManager();
		lm->setCurrentPage(10);
	} 
	else
	{
		d->pBtnACPC->setAutoExclusive(true);
		d->pBtnACPC->setStyleSheet("border-image: url(:/blue/tool_acpc_disable.png)");
	}
}

void qSlicerMeasureModuleWidget::OpenMeasureToolPanel()
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("mt");

	bool flag = d->pBtnMeasure->isChecked();
	bool flag2 = d->pBtnMeasure->isDown();
	if (flag)
	{
		//d->pBtnMeasure->setStyleSheet("background-color: #cbd8e9");
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/Measure.png)");
		if (measurePanel == NULL)
		{
			measurePanel = new MeasureToolPanel;
		}
		//measurePanel->setAttribute(Qt::WA_DeleteOnClose);
		measurePanel->setWindowFlags(measurePanel->windowFlags() | Qt::WindowStaysOnTopHint);
		//measurePanel->setGeometry(QRect(125, 400, 94, 225));
		measurePanel->setGeometry(QRect(125, 420, 94, 450));
		measurePanel->show();
		measurePanel->OnReset();
		connect(measurePanel, SIGNAL(drawLineTriggered()), this, SLOT(OnDrawLineTriggered()));
		connect(measurePanel, SIGNAL(drawAreaTriggered()), this, SLOT(OnDrawPolygonTriggered()));
		connect(measurePanel, SIGNAL(drawAngleTriggered()), this, SLOT(OnDrawAngleTriggered()));
		connect(measurePanel, SIGNAL(drawPointTriggered()), this, SLOT(OnDrawPointTriggered()));
		connect(measurePanel, SIGNAL(drawROITriggered()), this, SLOT(OnDrawROITriggered()));
		connect(measurePanel, SIGNAL(drawPlaneTriggered()), this, SLOT(OnDrawPlaneTriggered()));
	}
	else
	{
		//d->pBtnMeasure->setStyleSheet("background-color: #19254D");//主背景色
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/tool_measure_disable.png)");//unchecked状态
		measurePanel->close();
		  
	}
   
}


void qSlicerMeasureModuleWidget::OnZoomIn2(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("zi");
	if (checked)
	{
		d->pBtnZoomin_2->setAutoExclusive(false);
		d->pBtnZoomin_2->setStyleSheet("border-image:url(:/blue/RotatePoint.png)");
		/*vtkSmartPointer<vtkCollection> nodes;
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
			node->SetSliceIntersectionVisibility(true);
		}*/
		if (checked)
			qSlicerApplication::application()->layoutManager()->setCurrentPage(1001);
		else
			qSlicerApplication::application()->layoutManager()->setCurrentPage(qSlicerApplication::application()->layoutManager()->getOldPage());
	} 
	else
	{
		d->pBtnZoomin_2->setAutoExclusive(true);
		d->pBtnZoomin_2->setStyleSheet("border-image:url(:/blue/RotatePoint_disable.png)");
	}
	
}

void qSlicerMeasureModuleWidget::OnZoomIn(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("zi");
	if (checked)
	{
		d->pBtnZoomin->setAutoExclusive(false);
		d->pBtnZoomin->setStyleSheet("border-image: url(:/blue/Whirling.png)");
		std::vector<vtkMRMLNode*> sliceWidgets;
		QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
		foreach(const QString & viewName, names)
		{
			vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
				qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
			vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
			sw->UpdateLeftButtonEventByModifiers(12);
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
			node->SetSliceIntersectionVisibility(true);
		}
	}
	else
	{
		d->pBtnZoomin->setAutoExclusive(true);
		d->pBtnZoomin->setStyleSheet("border-image: url(:/blue/Whirling_disable.png)");
	}
}

void qSlicerMeasureModuleWidget::OnZoomOut(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("zo");
	if (checked)
	{
		d->pBtnZoomout->setAutoExclusive(false);
		d->pBtnZoomout->setStyleSheet("border-image: url(:/blue/Zoomin.png)");
		std::vector<vtkMRMLNode*> sliceWidgets;
		QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
		foreach(const QString & viewName, names)
		{
			vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
				qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
			vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
			qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceController()->setSliceLink(true);
			sw->UpdateLeftButtonEventByModifiers(vtkEvent::AltModifier);
		}
	}
	else
	{
		d->pBtnZoomout->setAutoExclusive(true);
		d->pBtnZoomout->setStyleSheet("border-image: url(:/blue/tool_zoomin_disable.png)");
	}
}

void qSlicerMeasureModuleWidget::OnTranslate(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("tran");
	if (checked)
	{
		d->pBtnMove->setAutoExclusive(false);
		d->pBtnMove->setStyleSheet("border-image: url(:/blue/Move.png)");
		std::vector<vtkMRMLNode*> sliceWidgets;
		QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
		foreach(const QString & viewName, names)
		{
			vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
				qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
			vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
			sw->UpdateLeftButtonEventByModifiers(vtkEvent::ShiftModifier);
		}
	}
	else
	{
		d->pBtnMove->setAutoExclusive(true);
		d->pBtnMove->setStyleSheet("border-image: url(:/blue/tool_move_disable.png)");
	}
}

void qSlicerMeasureModuleWidget::OnGrayScale(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("gray");
	if(checked){
		d->pBtnGrayscale->setAutoExclusive(false);
		d->pBtnGrayscale->setStyleSheet("border-image: url(:/blue/Grayscale.png)");
		vtkMRMLInteractionNode* interactionNode = nullptr;
		if (mrmlScene())
		{
			interactionNode = vtkMRMLInteractionNode::SafeDownCast(mrmlScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
		} 
	
		if (interactionNode != nullptr)
		{
			interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::AdjustWindowLevel);
		}
	}
	else
	{
		d->pBtnGrayscale->setAutoExclusive(true);
		d->pBtnGrayscale->setStyleSheet("border-image: url(:/blue/tool_grayscale_disable.png)");
	}
}

//重置按钮
void qSlicerMeasureModuleWidget::OnResetButton(QString bn) {
	Q_D(qSlicerMeasureModuleWidget);
	{ 
		d->pBtnMove->setChecked(false); //取消选中
		d->pBtnZoomin->setChecked(false); //取消选中
		d->pBtnZoomout->setChecked(false); //取消选中
	}
	if (bn == "mt")
	{
		d->pBtnMeasure->setAutoExclusive(false);
	}
	else
	{
		d->pBtnMeasure->setChecked(false);
		d->pBtnMeasure->setAutoExclusive(true);
	}
	if(bn!= "tran")
	{
		d->pBtnMove->setChecked(false);
	}
	if(bn!="gray")
	{
		d->pBtnGrayscale->setChecked(false);
	}
	if (bn != "reset")
	{
		d->pBtnReset->setChecked(false);//取消复位选中
	}
	//else
	//{
	//	d->pBtnACPC->setChecked(false);//取消ACPC选中
	//}
	if (bn != "ACPC")
	{
		d->pBtnACPC->setChecked(false);//取消ACPC选中
		d->pBtnACPC->setAutoExclusive(true);
	}
	{ 
		if (measurePanel)
			measurePanel->close();
	}
	if (measurePanel)
		measurePanel->OnReset(); //置直线，角度，面积未选中
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


	std::vector<vtkMRMLNode*> sliceWidgets;
	QStringList names = qSlicerApplication::application()->layoutManager()->sliceViewNames();
	foreach(const QString & viewName, names)
	{
		vtkMRMLCrosshairDisplayableManager* crosshairDisplayableManager = vtkMRMLCrosshairDisplayableManager::SafeDownCast(
			qSlicerApplication::application()->layoutManager()->sliceWidget(viewName)->sliceView()->displayableManagerByClassName("vtkMRMLCrosshairDisplayableManager"));
		vtkMRMLSliceIntersectionWidget* sw = crosshairDisplayableManager->GetSliceIntersectionWidget();
		sw->Reset();
	}
}

void qSlicerMeasureModuleWidget::Ontimeout()
{
	Q_D(qSlicerMeasureModuleWidget);
	m_timer->stop();
	d->pBtnWhirling->setStyleSheet("border-image: url(:/blue/tool_screenshot_disable.png)");
	qGLMessageDialog::ShowInformation(QStringLiteral("截图完成!"), true);
}
void qSlicerMeasureModuleWidget::OnScreenShot()
{
	Q_D(qSlicerMeasureModuleWidget);

	//d->pBtnWhirling->setStyleSheet("background-color:#cbd8e9");
	d->pBtnWhirling->setStyleSheet("border-image: url(:/blue/printScreen.png)");
	
	m_timer->start();

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

}

void qSlicerMeasureModuleWidget::OnRotate(bool checked)
{
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("rotate");
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
	DoFreshRoomState();
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
	DoFreshRoomState();
}
void qSlicerMeasureModuleWidget::OnViewType1() {
	Q_D(qSlicerMeasureModuleWidget);
	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
	vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
	if (lnode) {
		lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutThreeOverThreeView);
	}
	DoFreshRoomState();
}
void qSlicerMeasureModuleWidget::OnViewType2() {
	Q_D(qSlicerMeasureModuleWidget);
	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
	vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
	if (lnode) {
		lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutConventionalPlotView);
	}
	DoFreshRoomState();
}
void qSlicerMeasureModuleWidget::OnReset(bool checked) {
	Q_D(qSlicerMeasureModuleWidget);
	OnResetButton("reset");
	bool flag = d->pBtnReset->isChecked();
	if (checked)
	{
		d->pBtnReset->setAutoExclusive(false);
		d->pBtnReset->setStyleSheet("border-image: url(:/blue/Reset.png)");
		Reinit();
	}
	else
	{
		d->pBtnReset->setAutoExclusive(true);
		d->pBtnReset->setStyleSheet("border-image: url(:/blue/tool_reset_disable.png)");
	}
}
void qSlicerMeasureModuleWidget::OnFresh() {
	Q_D(qSlicerMeasureModuleWidget);
}

void qSlicerMeasureModuleWidget::OnDrawPointTriggered() 
{
	Q_D(qSlicerMeasureModuleWidget);
	if (measurePanel)
	{
		measurePanel->close();
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/measurePoint.png)");
		this->onActiveMarkupMRMLNodeAdded(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsFiducialNode"));
	}
}

void qSlicerMeasureModuleWidget::OnDrawPlaneTriggered()
{
	Q_D(qSlicerMeasureModuleWidget);
	if (measurePanel)
	{
		measurePanel->close();
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/measurePlane.png)");
		this->onActiveMarkupMRMLNodeAdded(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsPlaneNode"));
	}
}

void qSlicerMeasureModuleWidget::OnDrawROITriggered() {
	Q_D(qSlicerMeasureModuleWidget);
	if (measurePanel)
	{
		measurePanel->close();
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/measureVolume.png)");
		this->onActiveMarkupMRMLNodeAdded(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsROINode"));
	}
}

void qSlicerMeasureModuleWidget::OnDrawLineTriggered()
{
	Q_D(qSlicerMeasureModuleWidget);
	if (measurePanel)
	{
		measurePanel->close();
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/measureLine.png)");
		this->onActiveMarkupMRMLNodeAdded(qSlicerApplication::application()->mrmlScene()->AddNewNodeByClass("vtkMRMLMarkupsLineNode"));
	}
}

void qSlicerMeasureModuleWidget::OnDrawPolygonTriggered()
{
	Q_D(qSlicerMeasureModuleWidget);
	if (measurePanel)
	{
		measurePanel->close();
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/measureArea.png)");
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
		measurePanel->close();
		d->pBtnMeasure->setStyleSheet("border-image: url(:/blue/measureAngle.png)");
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



