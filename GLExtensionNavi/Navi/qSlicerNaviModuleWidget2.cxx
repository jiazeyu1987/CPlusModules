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
#include "qSlicerNaviModuleWidget.h"
#include "ui_qSlicerNaviModuleWidget.h"
#include "qGLMessageDialog.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "vtkSlicerNaviLogic.h"
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
#include "NaviWidget2D.h"
#include "qMRMLSliderWidget.h"
//#include "qMRMLSegmentEditorWidget.h"
//#include "qMRMLSegmentEditorWidget.cxx"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerIOManager.h"
#include <QSettings>
#include "qSlicerModuleManager.h"
//#include "qSlicerSegmentationsModule.h"
#include "NaviWidgetStructure.h"
#include "NaviWidgetPhase.h"
#include "qGLTool.h"
#include "NaviWidget3D.h"
#include <itksys/SystemTools.hxx>
#include "qSlicerSegmentEditorAbstractEffect.h" 
#include "QToolButton.h"
#include "vtkMRMLSegmentEditorNode.h"
#include "vtkMRMLSegmentationDisplayNode.h"
#include "NaviWidgetFiber.h"
#include "vtkSlicerVolumeRenderingLogic.h"
#include "vtkMRMLVolumeRenderingDisplayNode.h"
#include "vtkMRMLVolumePropertyNode.h"
#include <vtkMRMLCrosshairNode.h>
#include <vtkMRMLDiffusionWeightedVolumeNode.h>
#include "vtkTransform.h"
#include "vtkMRMLSliceLogic.h"
#include "NaviDWI2D.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkCollisionDetectionFilter.h"
#include "vtkDataTransfer.h"
#include "vtkHTTPHandler.h"
#include "vtkDataIOManagerLogic.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkImageStencil.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkImageIterator.h"
#include "vtkCharArray.h"
#include "NaviWidgetAblation.h"
#include "PathAdjustment.h"
#include  "QScrollBar.h"
#include "vtkITKLabelShapeStatistics.h"
#include "vtkImageThreshold.h"
#include "vtkImageToImageStencil.h"
#include "vtkImageAccumulate.h"
//#include "qSlicerSubjectHierarchyPluginHandler.h"
//#include "qSlicerSubjectHierarchyCloneNodePlugin.h"
#include "vtkMRMLSubjectHierarchyNode.h"
#include "vtkSlicerSubjectHierarchyModuleLogic.h"
#include "qSlicerSegmentEditorScriptedEffect.h"
#include <PythonQt.h>
class StructureItemInfo 
{
public:       //公有成员，外部可以访问 
    std::string m_Name;
    std::string m_Alias;
    std::string m_IconPath;

    StructureItemInfo::StructureItemInfo(std::string name, std::string alias, std::string path) {
        m_Name = name;
        m_Alias = alias;
        m_IconPath = path; 
    }
    StructureItemInfo::~StructureItemInfo() {}
}; 
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerNaviModuleWidgetPrivate : public Ui_qSlicerNaviModuleWidget
{
    Q_DECLARE_PUBLIC(qSlicerNaviModuleWidget);
protected:
    qSlicerNaviModuleWidget* const q_ptr;
public:
    int m_Index = 1;
    qSlicerNaviModuleWidgetPrivate(qSlicerNaviModuleWidget& object);
    ~qSlicerNaviModuleWidgetPrivate();
    vtkSlicerNaviLogic* logic() const;
    QList<StructureItemInfo*> m_StructureItemList;
    NaviWidget3D* m_Temp3DWidget = nullptr;
    
    QToolButton* toolButton(qSlicerSegmentEditorAbstractEffect* effect);
};

//-----------------------------------------------------------------------------
// qSlicerNaviModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerNaviModuleWidgetPrivate::qSlicerNaviModuleWidgetPrivate(qSlicerNaviModuleWidget& object) : q_ptr(&object)
{
    m_StructureItemList.clear();
    std::string organIconPath = "E:/S/GLModule/GLExtensionNavi/Navi/Resources/organIcon/";
    m_StructureItemList.append(new StructureItemInfo(QStringLiteral("脑").toStdString(), "Brain", organIconPath));
    m_StructureItemList.append(new StructureItemInfo(QStringLiteral("血管").toStdString(), "Blood vessel", organIconPath));
    m_StructureItemList.append(new StructureItemInfo(QStringLiteral("皮肤").toStdString(), "Skin", organIconPath));
    m_StructureItemList.append(new StructureItemInfo(QStringLiteral("骨骼").toStdString(), "Bone", organIconPath));
    m_StructureItemList.append(new StructureItemInfo(QStringLiteral("肿瘤").toStdString(), "Tumor", organIconPath));
    m_StructureItemList.append(new StructureItemInfo(QStringLiteral("计划消融区").toStdString(), "Nerve", organIconPath));
    m_StructureItemList.append(new StructureItemInfo(QStringLiteral("其它").toStdString(), "Other", organIconPath));
}
qSlicerNaviModuleWidgetPrivate::~qSlicerNaviModuleWidgetPrivate() {}

vtkSlicerNaviLogic* qSlicerNaviModuleWidgetPrivate::logic() const {
    Q_Q(const qSlicerNaviModuleWidget);
    return vtkSlicerNaviLogic::SafeDownCast(q->logic());
}



//-----------------------------------------------------------------------------
// qSlicerNaviModuleWidget methods
 
//-----------------------------------------------------------------------------
qSlicerNaviModuleWidget::qSlicerNaviModuleWidget(QWidget* _parent)
    : Superclass(_parent)
    , d_ptr(new qSlicerNaviModuleWidgetPrivate(*this))
{
    
} 

//-----------------------------------------------------------------------------
qSlicerNaviModuleWidget::~qSlicerNaviModuleWidget()
{
}
//-----------------------------------------------------------------------------
void qSlicerNaviModuleWidget::setup()
{
    Q_D(qSlicerNaviModuleWidget);
    d->setupUi(this);
    this->Superclass::setup(); 
    //connect(d_ptr->btnNext, SIGNAL(clicked()), this, SLOT(onGotoNextPage()));
    //connect(d_ptr->btnPreview, SIGNAL(clicked()), this, SLOT(onGotoPreviewPage()));
    connect(d_ptr->listWidget2DMode, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(onChooseItem(QListWidgetItem*)));
    qvtkReconnect(logic(), vtkSegmentation::SegmentAdded,this, SLOT(onSegmentAdded(vtkObject*,void*)));
    qvtkReconnect(logic(), vtkSegmentation::SegmentRemoved, this, SLOT(onSegmentRemoved()));
    qvtkReconnect(logic(), vtkSegmentation::SegmentModified, this, SLOT(onSegmentModified()));
    auto settings = qSlicerApplication::application()->settings;
    
    connect(d_ptr->btn3D, SIGNAL(clicked()), this, SLOT(SetCurrent3DDisplay()));
    connect(d_ptr->btn2D, SIGNAL(clicked()), this, SLOT(SetCurrent2DDisplay()));
    connect(d_ptr->hSliderFusionRatio, SIGNAL(valueChanged(int)),
        this, SLOT(onFusionRatioChanged(int)));
    connect(d_ptr->btnRight, SIGNAL(clicked()),
        this, SLOT(onInCreaseAblationDistance()));
    connect(d_ptr->btnLeft, SIGNAL(clicked()),
        this, SLOT(onDeCreaseAblationDistance()));
    connect(d_ptr->cmbAblation, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onUpdateAblationAreaInfo(int)));
    connect(d_ptr->cmbDir, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onChangeViewDirection(int)));
    connect(d_ptr->listWidgetStructure, &QListWidget::itemDoubleClicked, this, [=]() {onCreateSegmentPipeline(1); });
    connect(d_ptr->listWidgetSequence, &QListWidget::itemDoubleClicked, this, [=]() {onCreateSegmentPipeline(2); });
    connect(d_ptr->btnNew3DModel, &QPushButton::clicked, this, [=]() {onCreateSegmentPipeline(0); });
    connect(d_ptr->btnOK, SIGNAL(clicked()), this, SLOT(onSegment3DConfirm()));
    connect(d_ptr->btnCancel, SIGNAL(clicked()), this, SLOT(onSegment3DCancel()));
    //connect(d_ptr->pushButton_12, SIGNAL(clicked()), this, SLOT(onNormalViewDirection()));
    
    connect(d->btnNewOptFiber, SIGNAL(clicked()), this, SLOT(onNewOpticalFiber()));
    //connect(d_ptr->pushButton_15, SIGNAL(clicked()), this, SLOT(onFiberDirection()));

    connect(d_ptr->btn_addArea, SIGNAL(clicked()), this, SLOT(onAddAblationArea()));
    connect(d_ptr->btn_deletedArea, SIGNAL(clicked()), this, SLOT(onRemoveAblationArea()));
    connect(d_ptr->pushButton_20, SIGNAL(clicked()), this, SLOT(onHDBrainExtract()));
    connect(d_ptr->pushButton_19, SIGNAL(clicked()), this, SLOT(onAdjustFiberPoint()));//路径微调
    connect(d_ptr->pushButton_11, SIGNAL(clicked()), this, SLOT(onAblationArea()));//消融区添加
    //connect(d_ptr->pushButton_18, SIGNAL(clicked()), this, SLOT(onoperation()));//手术视角
    connect(d_ptr->pushButton_18, SIGNAL(clicked()), this, SLOT(onAblationEstimate()));//手术视角
    //connect(d_ptr->pushButton_15, SIGNAL(clicked()), this, SLOT(onFiberDirection()));
    connect(d_ptr->pushButton_14, SIGNAL(clicked()), this, SLOT(onLeksellRecognize()));//手术视角
    connect(d_ptr->pushButton_12, SIGNAL(clicked()), this, SLOT(onFramePositional()));//手术视角
    connect(d_ptr->pushButton_13, SIGNAL(clicked()), this, SLOT(onBrainMatterSegment()));//手术视角
    connect(d_ptr->btn_Areaback, SIGNAL(clicked()), this, SLOT(onbacktab0()));
    connect(d_ptr->btn_Pathback, SIGNAL(clicked()), this, SLOT(onbacktab0()));
    //connect(d_ptr->btn_Pathback_2, SIGNAL(clicked()), this, SLOT(onbacktab0()));
    connect(d_ptr->return_to_panel, &QPushButton::clicked, this, [=]() {d_ptr->tabWidget_3->setCurrentIndex(0); });
    //d_ptr->pushButton_18->setEnabled(false);
    connect(d_ptr->cmbDir_2, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onRelativePositionDirectionChanged(int)));
    connect(d_ptr->btnAdd, SIGNAL(clicked()),
        this, SLOT(onRelativePositionDistanceAdd()));
    connect(d_ptr->btnSub, SIGNAL(clicked()),
        this, SLOT(onRelativePositionDirectionMinus()));
    connect(d_ptr->btnMove, SIGNAL(clicked()),
        this, SLOT(onRelativePositionDirectionMove()));
    connect(d_ptr->tabWidget, &QTabWidget::currentChanged, this, &qSlicerNaviModuleWidget::onTabBarClicked);
    d->listWidget2DMode->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    d->listWidget2DMode->setEditTriggers(QAbstractItemView::DoubleClicked);
    

    //d_ptr->widget_2->hide();
    
	d_ptr->tabWidgetModel->tabBar()->hide();
	d_ptr->tabWidgetAdjust->tabBar()->hide();
	d_ptr->tabWidgetTools->tabBar()->hide();
    d_ptr->tabWidgetFiber->tabBar()->hide();
    d->tabWidget_3->setCurrentIndex(0);
    d->tabWidget_3->tabBar()->hide();
    d->tabWidget_5->tabBar()->hide();
    connect(d_ptr->btnBack2View, SIGNAL(clicked()), this, SLOT(onReturnLastPage()));
    connect(d_ptr->btnBack2Struct, SIGNAL(clicked()), this, SLOT(onReturnLastPage()));
    connect(d_ptr->btnBack2Sequence, SIGNAL(clicked()), this, SLOT(onReturnLastPage()));
    connect(d_ptr->pushButton_28, SIGNAL(clicked()), this, SLOT(DoEnterVesselPanel()));
    connect(d_ptr->btnWave, &QPushButton::clicked, this, &qSlicerNaviModuleWidget::OnWaveChangeClick);
    m_CallbackCommand->SetClientData(reinterpret_cast<void*>(this));
    m_CallbackCommand->SetCallback(qSlicerNaviModuleWidget::processEvents);
    /*d->logic()->AddObserver(9990, this->m_CallbackCommand);*/
    d->tabWidget_5->setTabEnabled(1, true);
    d->tabWidget_5->setTabEnabled(2, true);
	d->listWidgetOpticalFiber->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	d->listWidgetOpticalFiber->verticalScrollBar()->setSingleStep(5);
    //connect(d_ptr->listWidgetOpticalFiber, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(on_click_fibe_widget(QListWidgetItem*)));
    d_ptr->tabWidgetAdjust->hide();

    qvtkReconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
    d->btn2D->animateClick(10);
    pyModulesNames << "GLPyDTI" << "HDBrainExtractionTool" 
        << "VesselSegment" << "VesselnessFiltering"
        << "ExtractCenterline" << "BrainParcellation" 
        << "VesselSegment" << "AblationEstimate"
        << "TractographyDisplay"<<"BrainSegmentDisplay"
        << "BrainStructuresSegmenter"<< "DoctorView"
        << "find_zFrame"<<"GLPyPlan";
    pyModuleContainers << d->DTIPanel << d->PyPanel
        << d->VesselPanel << d->VesselPanel_VMTK
        << d->VesselPanel_VMTK_center_line << d->BrainSegmentPanel
        << d->VesselPanel << d->AblationEstimatePanel
        << d->DTIDisplayPanel<< d->BrainSegmentDisplayPanel
        << d->GrayWhiteMatterPanel<<d->DoctorViewPanel
        << d->PyPanelZFrame<<d->NavigationPanel;

    for (int i = 0; i < pyModulesNames.size(); i++) {
        QString moduleName = pyModulesNames.at(i);
        qSlicerAbstractCoreModule* pyModule = qSlicerApplication::application()->moduleManager()->module(moduleName);
        pyModules.push_back(pyModule);
    }
    
}   

void qSlicerNaviModuleWidget::doFreshACPCCoordinate() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    if (!widget->m_FiberData->m_TargetFiducialNode) {
        return;
    }
    const char* frameX = GetGlobalSaveValue("ACPC_X");
    const char* frameY = GetGlobalSaveValue("ACPC_Y");
    const char* frameZ = GetGlobalSaveValue("ACPC_Z");
    const char* center = GetGlobalSaveValue("ACPC_CENTER");
    if (!frameX || !frameY || !frameZ || !center) {
        return;
    }
    QStringList listX = QString::fromLatin1(frameX).split(",");
    QStringList listY = QString::fromLatin1(frameY).split(",");
    QStringList listZ = QString::fromLatin1(frameZ).split(",");
    QStringList listC = QString::fromLatin1(center).split(",");
    if (listX.size() < 3 || listY.size() < 3 || listZ.size() < 3 || listC.size() < 3) {
        return;
    }
    vtkMatrix4x4* ijk_to_ras = vtkMatrix4x4::New();
    ijk_to_ras->SetElement(0, 0, listX[0].toDouble());
    ijk_to_ras->SetElement(1, 0, listX[1].toDouble());
    ijk_to_ras->SetElement(2, 0, listX[2].toDouble());
    ijk_to_ras->SetElement(3, 0, 0);
    ijk_to_ras->SetElement(0, 1, listY[0].toDouble());
    ijk_to_ras->SetElement(1, 1, listY[1].toDouble());
    ijk_to_ras->SetElement(2, 1, listY[2].toDouble());
    ijk_to_ras->SetElement(3, 1, 0);
    ijk_to_ras->SetElement(0, 2, listZ[0].toDouble());
    ijk_to_ras->SetElement(1, 2, listZ[1].toDouble());
    ijk_to_ras->SetElement(2, 2, listZ[2].toDouble());
    ijk_to_ras->SetElement(3, 2, 0);
    ijk_to_ras->SetElement(0, 3, listC[0].toDouble());
    ijk_to_ras->SetElement(1, 3, listC[1].toDouble());
    ijk_to_ras->SetElement(2, 3, listC[2].toDouble());
    ijk_to_ras->SetElement(3, 3, 1);

    ijk_to_ras->Invert();

    double* point1 = widget->m_FiberData->m_TargetFiducialNode->GetNthControlPoint(0)->Position;
    double old_point[4], new_point[4];
    old_point[0] = point1[0];
    old_point[1] = point1[1];
    old_point[2] = point1[2];
    old_point[3] = 1;
    std::cout << "old_point is :" << point1[0] << "," << point1[1] << "," << point1[2] << "," << std::endl;
    ijk_to_ras->MultiplyPoint(old_point, new_point);
    std::cout << "new_point is :" << new_point[0] << "," << new_point[1] << "," << new_point[2] << "," << std::endl;
    std::cout << "acpc center is :" << listC[0].toDouble() << "," << listC[1].toDouble() << "," << listC[2].toDouble() << "," << std::endl;
    d->lblX2->setText(QString("%1").arg(new_point[0]));
    d->lblY2->setText(QString("%1").arg(new_point[1]));
    d->lblZ2->setText(QString("%1").arg(new_point[2]));
}

void qSlicerNaviModuleWidget::doFreshFrameCoordinate() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    if (!widget->m_FiberData->m_TargetFiducialNode) {
        return;
    }
    d->lblRAN->setText(QString("%1").arg(widget->m_FiberData->m_RelativePositionDistance));
    d->cmbDir_2->setCurrentIndex(widget->m_FiberData->m_RelativePositionDirection);
    d->lblDistance->setText(QString("%1").arg(widget->m_FiberData->m_RelativePositionDistance));
    if (widget->m_FiberData->m_RelativePositionDistance == 0) {
        d->cmbDir_2->setCurrentIndex(0);
    }
    else {
        d->cmbDir_2->setCurrentIndex(1);
    }
    const char* frameX = GetGlobalSaveValue("leksell_frameX");
    const char* frameY = GetGlobalSaveValue("leksell_frameY");
    const char* frameZ = GetGlobalSaveValue("leksell_frameZ");
    const char* center = GetGlobalSaveValue("leksell_center");
    if (!frameX || !frameY || !frameZ || !center) {
        return;
    }
    std::cout << "frameX:" << frameX << std::endl;
    std::cout << "frameY:" << frameY << std::endl;
    std::cout << "frameZ:" << frameZ << std::endl;
    std::cout << "center:" << center << std::endl;
    QStringList listX = QString::fromLatin1(frameX).split(",");
    QStringList listY = QString::fromLatin1(frameY).split(",");
    QStringList listZ = QString::fromLatin1(frameZ).split(",");
    QStringList listC = QString::fromLatin1(center).split(",");
    vtkMatrix4x4* ijk_to_ras = vtkMatrix4x4::New();
    ijk_to_ras->SetElement(0, 0, listX[0].toDouble());
    ijk_to_ras->SetElement(1, 0, listX[1].toDouble());
    ijk_to_ras->SetElement(2, 0, listX[2].toDouble());
    ijk_to_ras->SetElement(3, 0, 0);
    ijk_to_ras->SetElement(0, 1, listY[0].toDouble());
    ijk_to_ras->SetElement(1, 1, listY[1].toDouble());
    ijk_to_ras->SetElement(2, 1, listY[2].toDouble());
    ijk_to_ras->SetElement(3, 1, 0);
    ijk_to_ras->SetElement(0, 2, listZ[0].toDouble());
    ijk_to_ras->SetElement(1, 2, listZ[1].toDouble());
    ijk_to_ras->SetElement(2, 2, listZ[2].toDouble());
    ijk_to_ras->SetElement(3, 2,0);
    ijk_to_ras->SetElement(0, 3, listC[0].toDouble());
    ijk_to_ras->SetElement(1, 3, listC[1].toDouble());
    ijk_to_ras->SetElement(2, 3, listC[2].toDouble());
    ijk_to_ras->SetElement(3, 3, 1);

    ijk_to_ras->Invert();

    double* point1 = widget->m_FiberData->m_TargetFiducialNode->GetNthControlPoint(0)->Position;
    double old_point[4], new_point[4],new_point2[4];
    old_point[0] = point1[0];
    old_point[1] = point1[1];
    old_point[2] = point1[2];
    old_point[3] = 1;
    std::cout << "old_point2 is :" << point1[0] << "," << point1[1] << "," << point1[2] << "," << std::endl;
    ijk_to_ras->MultiplyPoint(old_point, new_point);
    std::cout << "frame matrix:";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << ijk_to_ras->GetElement(i,j)<<",";
        }
    }
    std::cout << std::endl;
    std::cout << "new_point2 is :" << new_point[0] << "," << new_point[1] << "," << new_point[2] << "," << std::endl;
    std::cout << "frame center is :" << listC[0].toDouble() << "," << listC[1].toDouble() << "," << listC[2].toDouble() << "," << std::endl;
    
    auto ct_transform_node = vtkMRMLTransformNode::SafeDownCast(mrmlScene()->GetFirstNodeByName("CT-Transform"));
    vtkMatrix4x4* transform_ct = vtkMatrix4x4::New();
    if (ct_transform_node) {
       // ct_transform_node->GetMatrixTransformFromParent()->MultiplyPoint(new_point, new_point2);
        ct_transform_node->GetMatrixTransformToWorld(transform_ct);
        transform_ct->Invert();
    }

    auto final_matrix = vtkMatrix4x4::New();
    vtkMatrix4x4::Multiply4x4(ijk_to_ras, transform_ct, final_matrix);
    final_matrix->MultiplyPoint(old_point, new_point2);
    d->lblX->setText(QString("%1").arg(new_point2[0]));
    d->lblY->setText(QString("%1").arg(new_point2[1]));
    d->lblZ->setText(QString("%1").arg(new_point2[2]));

    if (!widget->m_FiberData->m_InputFiducialNode) {
        return;
    }
    double* point2 = widget->m_FiberData->m_InputFiducialNode->GetNthControlPoint(0)->Position;

    double fiber_vector[3];
    fiber_vector[0] = point2[0] - point1[0];
    fiber_vector[1] = point2[1] - point1[1];
    fiber_vector[2] = point2[2] - point1[2];
    double x_vector[3];
    x_vector[0] = listX[0].toDouble();
    x_vector[1] = listX[1].toDouble();
    x_vector[2] = listX[2].toDouble();
    double y_vector[3];
    y_vector[0] = listY[0].toDouble();
    y_vector[1] = listY[1].toDouble();
    y_vector[2] = listY[2].toDouble();

    auto angle1 = vtkMath::AngleBetweenVectors(fiber_vector, x_vector) * 180 / 3.1415926;
    auto angle2 = vtkMath::AngleBetweenVectors(fiber_vector, y_vector) * 180 / 3.1415926;
    d->lblARC->setText(QString("%1").arg(angle1));
    d->lblRAN->setText(QString("%1").arg(angle2));
    widget->SetFrameInfo(QString("%1").arg(new_point2[0]),
        QString("%1").arg(new_point2[1]),
        QString("%1").arg(new_point2[2]),
        QString("%1").arg(angle1),
        QString("%1").arg(angle2));
}

void qSlicerNaviModuleWidget::onRelativePositionDirectionChanged(int i) {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    widget->changeRelativePositionDirection(i);
}
void qSlicerNaviModuleWidget::onRelativePositionDistanceAdd() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    if (widget->m_FiberData->m_ModelNode == nullptr) {
        return;
    }
    widget->ModifyRelativePositionDistance(0.5);
    d->lblDistance->setText(QString("%1").arg(widget->m_FiberData->m_RelativePositionDistance));
}
void qSlicerNaviModuleWidget::onRelativePositionDirectionMinus() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    if (widget->m_FiberData->m_ModelNode == nullptr) {
        return;
    }
    widget->ModifyRelativePositionDistance(-0.5);
    d->lblDistance->setText(QString("%1").arg(widget->m_FiberData->m_RelativePositionDistance));
}
void qSlicerNaviModuleWidget::onRelativePositionDirectionMove() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    if (widget->m_FiberData->m_ModelNode == nullptr) {
        return;
    }

    auto index = widget->m_FiberData->m_RelativePositionDistance;
    auto len1 = widget->m_BaseInfo.length;
    vtkMRMLTransformNode* linearTransformNode = widget->m_FiberData->m_ModelNode->GetParentTransformNode();
    vtkSmartPointer<vtkMatrix4x4> matirx1 = widget->m_FiberData->m_ModelNode->GetParentTransformNode()->GetMatrixTransformFromParent();

    double vector1[3];
    vector1[0] = widget->m_FiberData->m_PointInput[0] - widget->m_FiberData->m_PointTarget[0];
    vector1[1] = widget->m_FiberData->m_PointInput[1] - widget->m_FiberData->m_PointTarget[1];
    vector1[2] = widget->m_FiberData->m_PointInput[2] - widget->m_FiberData->m_PointTarget[2];
    double newp[3];

    if(widget->m_FiberData->m_RelativePositionDirection == 0){
        newp[0] = widget->m_FiberData->m_PointTarget[0] + vector1[0] * index / len1;
        newp[1] = widget->m_FiberData->m_PointTarget[1] + vector1[1] * index / len1;
        newp[2] = widget->m_FiberData->m_PointTarget[2] + vector1[2] * index / len1;
    }
    else {
        newp[0] = widget->m_FiberData->m_PointTarget[0] - vector1[0] * index / len1;
        newp[1] = widget->m_FiberData->m_PointTarget[1] - vector1[1] * index / len1;
        newp[2] = widget->m_FiberData->m_PointTarget[2] - vector1[2] * index / len1;
    }

    matirx1->SetElement(0, 3, newp[0]);
    matirx1->SetElement(1, 3, newp[1]);
    matirx1->SetElement(2, 3, newp[2]);

    //玄学,不清楚原理,下面的会自动变为负值
    matirx1->SetElement(0, 2, -matirx1->GetElement(0, 2));
    matirx1->SetElement(1, 2, -matirx1->GetElement(1, 2));
    matirx1->SetElement(2, 0, -matirx1->GetElement(2, 0));
    matirx1->SetElement(2, 1, -matirx1->GetElement(2, 1));
    linearTransformNode->SetMatrixTransformToParent(matirx1.GetPointer());
}

void qSlicerNaviModuleWidget::doFreshCoordinate() {
    doFreshACPCCoordinate();
    doFreshFrameCoordinate();
}

void qSlicerNaviModuleWidget::onFramePositional() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    doFreshCoordinate();
    d->tabWidget_3->setCurrentIndex(8);
}

void qSlicerNaviModuleWidget::onAblationEstimate() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    d->logic()->InvokeEvent(vtkSlicerNaviLogic::EventType::OpenAblationEstimatePanel);
    d->tabWidget_3->setCurrentIndex(5);
}


void qSlicerNaviModuleWidget::OnWaveChangeClick() {
    Q_D(qSlicerNaviModuleWidget);
    FiberData::fiber_type = (++FiberData::fiber_type) % 2;
    QString info = FiberData::fiber_type == 0 ? "980" : "1064";
    d->btnWave->setText(info);

    int n = d->listWidgetOpticalFiber->count();
    for(int i = 0 ; i < n ; i++)
    {
        QListWidgetItem* item = d->listWidgetOpticalFiber->item(i);
        auto widget = dynamic_cast<NaviWidgetFiber*>(d->listWidgetOpticalFiber->itemWidget(item));
        widget->ChangeWave();
    }
}

void qSlicerNaviModuleWidget::onLogicModified(vtkObject* vtkNotUsed(caller), vtkObject* callData)
{
    Q_D(qSlicerNaviModuleWidget);

    // the call data should be the annotation hierarchy node that was just
    // added, passed along from the logic
    if (callData == nullptr)
    {
        return;
    }
    vtkMRMLColorNode* node = nullptr;
    node = reinterpret_cast<vtkMRMLColorNode*>(callData);
    if (node == nullptr)
    {
        return;
    }
    auto help_node = node->GetAttribute("Help_Node");
    auto type = node->GetAttribute("Help_Name");
    auto value = node->GetAttribute("Help_Info_0");
    SafeRemove(node);
    if (help_node && strcmp(help_node, "true") == 0) {

        if (type && value) {
            if (strcmp(type, "show_stripped_brain") == 0) {
                mrmlScene()->RemoveNode(node);
                DoFresh2DListView();
                DoFresh3DListView();
            }
            if (strcmp(type, "fresh_2d") == 0) {
                mrmlScene()->RemoveNode(node);
                DoFresh2DListView();
            }
            if (strcmp(type, "fresh_3d") == 0) {
                mrmlScene()->RemoveNode(node);
                DoFresh3DListView();
            }
            if (strcmp(type, "return_to_panel") == 0) {
                mrmlScene()->RemoveNode(node);
                d->tabWidget_3->setCurrentIndex(0);
            }
            if (strcmp(type, "show_dti") == 0) {
                mrmlScene()->RemoveNode(node);
            }
        }
    }
}

void qSlicerNaviModuleWidget::onBrainMatterSegment() {
    Q_D(qSlicerNaviModuleWidget);
    /*auto mm = qSlicerApplication::application()->moduleManager();
    auto module = mm->module("BrainStructuresSegmenter");

    d->tabWidget_3->setCurrentIndex(7);*/
    auto striped_brain = mrmlScene()->GetFirstNodeByName("stripped_brain");
    if (!striped_brain) {
        QStringList tmpList = { QStringLiteral("确定")};
        MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("提示:"), QStringLiteral("需要先去除颅骨"), MessageButtonType::BUTTON_OK, tmpList, true);
        return;
    }
    GLTool::progressBar->setText(QStringLiteral("正在进行灰质识别"));
    GLTool::progressBar->show();
    GLTool::progressBar->setProgressValue(31);
    PythonQtObjectPtr context = PythonQt::self()->getMainModule();
    context.evalScript(QString("import slicer.util as util; widget = util.getModuleWidget('BrainStructuresSegmenter'); widget.onSeperateGrayMatter()"));
    GLTool::progressBar->setProgressValue(61);
    GLTool::progressBar->setText(QStringLiteral("正在进行白质识别"));
    context.evalScript(QString("import slicer.util as util; widget = util.getModuleWidget('BrainStructuresSegmenter'); widget.onSeperateWhiteMatter()"));
    GLTool::progressBar->setProgressValue(100);
    GLTool::progressBar->close();
    DoFresh3DListView();
}

void qSlicerNaviModuleWidget::onLeksellRecognize() {
    Q_D(qSlicerNaviModuleWidget);
    //d->tabWidget_3->setCurrentIndex(3);
    GLTool::progressBar->setText(QStringLiteral("正在进行头架识别"));
    GLTool::progressBar->show();
    GLTool::progressBar->setProgressValue(31);
    PythonQtObjectPtr context = PythonQt::self()->getMainModule();
    context.evalScript(QString("import slicer.util as util; widget = util.getModuleWidget('find_zFrame'); widget.onsegmentButton()"));
    GLTool::progressBar->setProgressValue(100);
    GLTool::progressBar->close();
}

void qSlicerNaviModuleWidget::DoResetFiberSettingUI() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(0);
}

void qSlicerNaviModuleWidget::onHDBrainExtract() {
    Q_D(qSlicerNaviModuleWidget);
    //d->tabWidget_3->setCurrentIndex(4);
    /*GLTool::progressBar->setText(QStringLiteral("正在进行颅骨去除,时间较长,请耐心等待"));
    GLTool::progressBar->show();
    GLTool::progressBar->setProgressValue(31);*/
    PythonQtObjectPtr context = PythonQt::self()->getMainModule();
    context.evalScript(QString("import slicer.util as util; widget = util.getModuleWidget('HDBrainExtractionTool'); widget.onApplyButton()"));
    /*GLTool::progressBar->setProgressValue(100);
    GLTool::progressBar->close();*/
}

void qSlicerNaviModuleWidget::processEvents(vtkObject* caller, unsigned long eid, void* clientData, void* callData) {
    qSlicerNaviModuleWidget* self = reinterpret_cast<qSlicerNaviModuleWidget*>(clientData);
    if (eid == vtkSlicerNaviLogic::EventType::CloseNaviSettingPanel) {
        self->DoResetFiberSettingUI();
    }
    if (eid == vtkSlicerNaviLogic::EventType::Fresh2DView) {
        self->DoFresh2DListView();
    }
    if (eid == vtkSlicerNaviLogic::EventType::Fresh3DView) {
        self->DoFresh3DListView();
    }
}

void qSlicerNaviModuleWidget::onbacktab0()
{
    Q_D(qSlicerNaviModuleWidget);

    d->tabWidget_3->setCurrentIndex(0);
}



void qSlicerNaviModuleWidget::onAblationArea(){
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    d->tabWidget_3->setCurrentIndex(1);
}

void qSlicerNaviModuleWidget::onPathAdjustment() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(2);
}

void qSlicerNaviModuleWidget::DoCenterInputPoint() {
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    auto p1 = widget->m_FiberData->m_PointInput;
    vtkMRMLSliceNode::JumpAllSlices(mrmlScene(), p1[0], p1[1], p1[2], 0);
}

void qSlicerNaviModuleWidget::DoCenterTargetPoint() {
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    auto p1 = widget->m_FiberData->m_PointTarget;
    vtkMRMLSliceNode::JumpAllSlices(mrmlScene(), p1[0], p1[1], p1[2], 0);
}

void qSlicerNaviModuleWidget::onAllUnvisible()
{
    Q_D(qSlicerNaviModuleWidget);
    int num = d->listWidget2DMode->count();
    for (int i = 0; i < num; ++i)
    {
        QListWidgetItem* item = d->listWidget2DMode->item(i);
        NaviWidget2D* widget2D = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(item));
        if (widget2D) {
            widget2D->SetWidget2DVisible(false);
        }
    }
}

void qSlicerNaviModuleWidget::onoperation() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(3);
}

void qSlicerNaviModuleWidget::DoEnterVesselPanel() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(9);
}

void qSlicerNaviModuleWidget::onReturnLastPage()
{
    Q_D(qSlicerNaviModuleWidget);
    int currentPage = d->tabWidgetModel->currentIndex();
    d->tabWidgetModel->setCurrentIndex(currentPage-1);
}

void qSlicerNaviModuleWidget::onStateChanged2D(int state) {}
void qSlicerNaviModuleWidget::onStateChanged(int state) {
    Q_D(qSlicerNaviModuleWidget);
    d->EffectsWidget->onCreateSurfaceToggled(true);
}


void qSlicerNaviModuleWidget::onFusionRatioChanged(int value) {
    Q_D(qSlicerNaviModuleWidget);
    d->lineEditFusionRatio->setText(QString("%1").arg(value));
    DoSetOpacityRatio(value);
} 

void qSlicerNaviModuleWidget::onCreateSegmentPipeline(int seq) {
    Q_D(qSlicerNaviModuleWidget);
    if (seq == 0) {
        d->tabWidgetModel->setCurrentIndex(1);
        
    }
    if (seq == 1) {
        auto name = d->m_StructureItemList[d->listWidgetStructure->currentRow()]->m_Name;
        auto alias = d->m_StructureItemList[d->listWidgetStructure->currentRow()]->m_Alias;
        auto path = d->m_StructureItemList[d->listWidgetStructure->currentRow()]->m_IconPath;
        if (d->m_Temp3DWidget) {
            d->m_Temp3DWidget->SetTitle(QString::fromStdString(name));
            d->m_Temp3DWidget->SetPicture(QString::fromStdString(path), QString::fromStdString(alias));
            d->m_Temp3DWidget->hideDeleteBtn();
            d->m_Temp3DWidget->hideModifyBtn();
        }
        d->tabWidgetModel->setCurrentIndex(2);
    }
    if (seq == 2) {
        m_State = qSlicerNaviModuleWidget::WaitForAddSegment;

        //获取选中的VolumeNode
        QList<QListWidgetItem*> selected = d->listWidgetSequence->selectedItems();
        if (selected.size() != 1) {
            return;
        }
        QListWidgetItem* selectedItem = selected.at(0);
        NaviWidgetPhase* widget = dynamic_cast<NaviWidgetPhase*>(d->listWidgetSequence->itemWidget(selectedItem));
        auto master_node = widget->GetMyNode();
        
        //获取唯一的EditeorNode
        auto segmentEditorSingletonTag = "SegmentEditor";
        auto segmentEditorNode = mrmlScene()->GetSingletonNode(segmentEditorSingletonTag, "vtkMRMLSegmentEditorNode");
        if (!segmentEditorNode) {
            segmentEditorNode = mrmlScene()->AddNewNodeByClass("vtkMRMLSegmentEditorNode");
            segmentEditorNode->SetSingletonTag(segmentEditorSingletonTag);
        }
        
        //初始化分割页面
        vtkMRMLSegmentEditorNode* editorNode = vtkMRMLSegmentEditorNode::SafeDownCast(segmentEditorNode);
        d->EffectsWidget->setMRMLSegmentEditorNode(editorNode);
        auto label = d->m_Temp3DWidget->m_Data->structure_label;
        d->EffectsWidget->AddNewSegmentationNode(label);
        d->EffectsWidget->SetMasterNode(master_node);
        d->EffectsWidget->onAddSegment();
        d->EffectsWidget->onCreateSurfaceToggled(true);
        d->EffectsWidget->type = d->m_Temp3DWidget->m_Data->structure_alias;
        //d->EffectsWidget->updateEffectList();
        Reinit();
        d->m_Temp3DWidget->SetMasterNode(master_node);

        d->tabWidgetModel->setCurrentIndex(3);
    }
}



//创建临时的3D模型View
void qSlicerNaviModuleWidget::DoInitTemp3DWidget() {
    Q_D(qSlicerNaviModuleWidget);
    if (d->m_Temp3DWidget) {
        d->example_item_widget_layout->removeWidget(d->m_Temp3DWidget);
        delete d->m_Temp3DWidget;
    }
        
    d->m_Temp3DWidget = nullptr;
    d->m_Temp3DWidget = new NaviWidget3D();
    d->example_item_widget_layout->addWidget(d->m_Temp3DWidget);
}



//模型序列选择界面初始化
void qSlicerNaviModuleWidget::DoFreshPhaseListView()
{
    Q_D(qSlicerNaviModuleWidget);
    d->listWidgetSequence->clear();
    std::vector<vtkMRMLNode*> procNodes;
    int numProcNodes = qSlicerApplication::application()->mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", procNodes);
    for (int i = 0; i < numProcNodes; i++) {
        vtkMRMLScalarVolumeNode* procNode = vtkMRMLScalarVolumeNode::SafeDownCast(procNodes[i]);
        const char* is_registed = procNode->GetAttribute("is_registed");
        const char* alias_name = procNode->GetAttribute("alias_name");
        if (is_registed&&strcmp(is_registed, "true") == 0||alias_name && std::string(alias_name) == "basic") {
            NaviWidgetPhase* itemWidget = new NaviWidgetPhase(d->listWidgetSequence);
            itemWidget->SetMyNode(procNode);
            QListWidgetItem* structItem = new QListWidgetItem(d->listWidgetSequence);
            int width = d->listWidgetSequence->geometry().width();
            structItem->setSizeHint(QSize(width, 140));
            d->listWidgetSequence->setItemWidget(structItem, itemWidget);
        }
        
    }
}

void qSlicerNaviModuleWidget::DoFreshStructureListView()
{
    Q_D(qSlicerNaviModuleWidget);
    d->listWidgetStructure->clear();

    for (int i = 0; i < d->m_StructureItemList.size(); i++) {
        StructureItemInfo* item = d->m_StructureItemList.at(i);

        NaviWidgetStructure* itemWidget = new NaviWidgetStructure(d->listWidgetStructure);
        itemWidget->SetName(QString::fromStdString(item->m_Name));
        itemWidget->SetPath(QString::fromStdString(item->m_IconPath), QString::fromStdString(item->m_Alias));
        QListWidgetItem* structItem = new QListWidgetItem(d->listWidgetStructure);
        int width = d->listWidgetStructure->geometry().width();
        structItem->setSizeHint(QSize(width, 96));
        d->listWidgetStructure->setItemWidget(structItem, itemWidget);
    }
}

void qSlicerNaviModuleWidget::SetCurrent2DDisplay()
{
    Q_D(qSlicerNaviModuleWidget);
    connect(d->btnAllUnvisible, SIGNAL(clicked()), this, SLOT(onAllUnvisible()));
    DoFresh2DListView();
    DoFresh3DListView();
}

void qSlicerNaviModuleWidget::SetCurrent3DDisplay() {
    Q_D(qSlicerNaviModuleWidget);
    DoFresh3DListView();
}

void qSlicerNaviModuleWidget::OnBrainParcellation() {
    vtkMRMLNode* node = mrmlScene()->GetFirstNodeByName("BrainParcellationSegmentation");
    if (!node) {
        node = mrmlScene()->AddNewNodeByClass("vtkMRMLSegmentationNode");
        node->SetName("BrainParcellationSegmentation");
    }
    vtkMRMLSegmentationNode* segnode = vtkMRMLSegmentationNode::SafeDownCast(node);
    vtkMRMLDisplayNode* displayNode = segnode->GetDisplayNode();
    if (!displayNode) {
        segnode->CreateDefaultDisplayNodes();
    }
    displayNode = segnode->GetDisplayNode();
    displayNode->RemoveAllViewNodeIDs();
    displayNode->AddViewNodeID("vtkMRMLViewNode2");
    qSlicerApplication::application()->layoutManager()->setCurrentModule("BrainParcellation");
}

void qSlicerNaviModuleWidget::showBlendProportion(NaviWidget2D* Item2D)
{
    Q_D(qSlicerNaviModuleWidget);
    int num = d_ptr->listWidget2DMode->count();
    if (Item2D->pSwitchControl->isToggled()==false)
    {
		for (int i = 0; i < num; ++i)
		{
			QListWidgetItem* Item = d_ptr->listWidget2DMode->item(i);
			NaviWidget2D* Other2D = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(Item));
            if (!Other2D)
            {
                break;
            }
			Other2D->showMask(Other2D, false);
		}
        d_ptr->tabWidgetAdjust->hide();
        return;
    }

    bool flag = false;
    NaviWidget2D* widget2D = nullptr;
    for (int i=0;i< num;++i)
    {
        QListWidgetItem* widgetItem = d_ptr->listWidget2DMode->item(i);
        QWidget* widget = d->listWidget2DMode->itemWidget(widgetItem);
        widget2D = dynamic_cast<NaviWidget2D*>(widget);
		if (!widget2D)
		{
			continue;
		}
        if (widget2D->pSwitchControl->isToggled() && widget2D!=Item2D)
        {
            flag = true;
            break;
        }
    }
    if (!flag)
    {
        return;
    }
    if (Item2D->pSwitchControl->isToggled())
    {
        Item2D->SetWidget2DVisible(true);
        widget2D->SetWidget2DVisible(true);
        d_ptr->tabWidgetAdjust->show();
        DoSelectVolumeNode(Item2D->m_Node, widget2D->m_Node,0.5);
		for (int i = 0; i < num; ++i)
		{
			QListWidgetItem* Item = d_ptr->listWidget2DMode->item(i);
			NaviWidget2D* Other2D = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(Item));
			if (!Other2D)
			{
				break;
			}
			if (Other2D != Item2D && Other2D != widget2D)
			{
				Other2D->showMask(Other2D, true);
			}
		}
    }
}

void qSlicerNaviModuleWidget::onChooseItem(QListWidgetItem* item) {
    Q_D(qSlicerNaviModuleWidget);
    /*NaviWidget2D* widget1 = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(item));
    if (!widget1) {
        return;
    }
    if (widget1->m_Visibility == false) {
        return;
    }
    DoSelectVolumeNode(widget1->m_Node);
    Reinit();*/
}

void qSlicerNaviModuleWidget::onGotoNextPage() {
    qSlicerApplication::application()->layoutManager()->setCurrentPage(qSlicerApplication::application()->layoutManager()->m_Page + 1);
}
/**
    根据mrmlScene中的volumeNode的配准情况来刷新2D中的列表
*/
void qSlicerNaviModuleWidget::DoFresh2DListView() {
    Q_D(qSlicerNaviModuleWidget);
    m_Map2D.clear();
    d->listWidget2DMode->clear();
    std::vector<vtkMRMLNode*> procNodes;
    int numProcNodes = qSlicerApplication::application()->mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", procNodes);
    for (int i = 0; i < procNodes.size(); i++)
    {
        vtkMRMLScalarVolumeNode* procNode = vtkMRMLScalarVolumeNode::SafeDownCast(procNodes[i]);
        const char* is_registed = procNode->GetAttribute("is_registed");
        const char* alias_name = procNode->GetAttribute("alias_name");
        if (alias_name) {
            if (strcmp(alias_name, "basic") == 0) {
                if (m_Map2D[QString::fromLatin1(alias_name)] == nullptr) {
                    AddNodeTo2DList(procNode, true);
                }
            }
        }
        if (is_registed) {
            if (m_Map2D[QString::fromLatin1(alias_name)] == nullptr) {
                AddNodeTo2DList(procNode, true);

            }
        }

    }
    return;
}

void qSlicerNaviModuleWidget::DoShowDepthestNode() {
    Q_D(qSlicerNaviModuleWidget);
    auto biggest = -1;
    for (int j = 0; j < d->listWidget2DMode->count(); j++)
    {
        QListWidgetItem* item = d->listWidget2DMode->item(j);
        NaviWidget2D* widget = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(item));
        if (widget && widget->m_Visibility) {
            biggest = j;
        }
    }
    if (biggest != -1) {
        QListWidgetItem* item = d->listWidget2DMode->item(biggest);
        NaviWidget2D* widget = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(item));
        DoSelectVolumeNode(widget->m_Node);
    }
    else {
        auto em_node = mrmlScene()->GetFirstNodeByName("empty_volume_node_black");
        if (!em_node) {
            auto basic_node = vtkMRMLScalarVolumeNode::SafeDownCast(mrmlScene()->GetFirstNodeByName("basic"));
            if (!basic_node) {
                return;
            }
            auto em_node = vtkMRMLScalarVolumeNode::SafeDownCast(mrmlScene()->AddNewNodeByClass("vtkMRMLScalarVolumeNode", "empty_volume_node_black"));
            vtkNew<vtkImageData> clonedVolumeData;
            clonedVolumeData->SetDimensions(basic_node->GetImageData()->GetDimensions());
            clonedVolumeData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
            clonedVolumeData->GetPointData()->GetScalars()->Fill(1);
            em_node->SetAndObserveImageData(clonedVolumeData.GetPointer());
            em_node->SetOrigin(basic_node->GetOrigin());
            em_node->CopyOrientation(basic_node);
            em_node->SetSpacing(basic_node->GetSpacing());
            em_node->RemoveAllDisplayNodeIDs();
            em_node->CreateDefaultDisplayNodes();
            DoSelectVolumeNode(em_node);
        }
        else {
            DoSelectVolumeNode(em_node);
        }
    }
    Reinit();
}

void qSlicerNaviModuleWidget::OnVisibleChagned(NaviWidget2D* NW2D) {
    DoShowDepthestNode();
}

void qSlicerNaviModuleWidget::UpdateListWidget(NaviWidget2D* NW2D)
{
    Q_D(qSlicerNaviModuleWidget);
    isHiddenVolume = NW2D->hiddenPanelVisible;
    int index = GetIndexOfWidget2D(NW2D);
    int width = d->listWidget2DMode->geometry().width();
    QListWidgetItem* item = d->listWidget2DMode->item(index);
    if (m_LastVolListWidget)
    {
        QWidget* widget = d->listWidget2DMode->itemWidget(m_LastVolListWidget);
        if (widget == NULL) return;
        NaviWidget2D* modelWidget = dynamic_cast<NaviWidget2D*>(widget);
        if (modelWidget) modelWidget->SetDropdownBtnStatus(false, false);
        m_LastVolListWidget->setSizeHint(QSize(width, 95));
    }
    int height = 190;
    if (!isHiddenVolume) height = 95;
    item->setSizeHint(QSize(width, height));
    if (isHiddenVolume) m_LastVolListWidget = item;
    else m_LastVolListWidget = NULL;
    /*std::string s = NW2D->m_Label.toStdString();
    if (mapIsVolumeBtnPressed.contains(s))
        mapIsVolumeBtnPressed[s] = isHiddenVolume;*/

        //如果打开，隐藏其他
        /*if (isHiddenVolume)
        {
            QMutableMapIterator<std::string, bool> i(mapIsVolumeBtnPressed);
            while (i.hasNext())
            {
                i.next();
                if (i.value() && i.key() != s)
                    i.value() = false;
            }
        }*/
        //整个列表全部刷新
          //Refresh2DListView();

        //设置点击项为当前项
    NaviWidget2D* mw = qobject_cast<NaviWidget2D*>(sender());
}

int qSlicerNaviModuleWidget::GetIndexOfWidget2D(NaviWidget2D* NW2D) {
    Q_D(qSlicerNaviModuleWidget);
    for (int j = 0; j < d->listWidget2DMode->count(); j++)
    {
        QListWidgetItem* item = d->listWidget2DMode->item(j);
        NaviWidget2D* widget = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(item));
        if (widget == NW2D) {
            return j;
        }
    }
    return -1;
}

vtkMRMLVolumeRenderingDisplayNode* qSlicerNaviModuleWidget::GetVolumeRenderingDisplayNode(vtkMRMLVolumeNode* node) {
    vtkSlicerVolumeRenderingLogic* vrlLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(appLogic()->GetModuleLogic("VolumeRendering"));
    if (!vrlLogic) {
        return nullptr;
    }
    vtkMRMLVolumeRenderingDisplayNode* displayNode = vrlLogic->GetFirstVolumeRenderingDisplayNode(node);
    if (!displayNode) {
        displayNode = vrlLogic->CreateVolumeRenderingDisplayNode();
        displayNode->SetVisibility(0);
        mrmlScene()->AddNode(displayNode);
        node->AddAndObserveDisplayNodeID(displayNode->GetID());
        int wasModifying = displayNode->StartModify();
        displayNode->SetIgnoreVolumeDisplayNodeThreshold(1);
        //vrlLogic->UpdateDisplayNodeFromVolumeNode(displayNode, node);
        displayNode->SetIgnoreVolumeDisplayNodeThreshold(0);
        displayNode->EndModify(wasModifying);
    }
    vtkMRMLVolumePropertyNode* propNode = displayNode->GetVolumePropertyNode();
    if (propNode == nullptr)
    {
        propNode = vtkMRMLVolumePropertyNode::New();
        mrmlScene()->AddNode(propNode);
    }
    displayNode->SetAndObserveVolumePropertyNodeID(propNode->GetID());

    vtkMRMLNode* roiNode = displayNode->GetROINode();
    if (roiNode == nullptr && displayNode->GetROINode() == nullptr)
    {
        roiNode = mrmlScene()->AddNewNodeByClass("vtkMRMLAnnotationROINode");
        vtkMRMLAnnotationROINode* annotationROINode = vtkMRMLAnnotationROINode::SafeDownCast(roiNode);
        if (annotationROINode)
        {
            annotationROINode->CreateDefaultDisplayNodes();
            annotationROINode->SetInteractiveMode(1);
            annotationROINode->SetDisplayVisibility(displayNode->GetCroppingEnabled());
        }
    }
    if (roiNode != nullptr)
    {
        displayNode->SetAndObserveROINodeID(roiNode->GetID());
    }
    FitROIToVolume(displayNode);
    return displayNode;
}


void qSlicerNaviModuleWidget::FitROIToVolume(vtkMRMLVolumeRenderingDisplayNode* vspNode)
{
    // Resize the ROI to fit the volume
    vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(vspNode->GetVolumeNode());

    if (!volumeNode)
    {
        return;
    }

    vtkMRMLAnnotationROINode* roiNode = vspNode->GetROINode();
    if (roiNode)
    {
        MRMLNodeModifyBlocker blocker(roiNode);

        double xyz[3] = { 0.0 };
        double center[3] = { 0.0 };

        vtkMRMLSliceLogic::GetVolumeRASBox(volumeNode, xyz, center);
        for (int i = 0; i < 3; i++)
        {
            xyz[i] *= 0.5;
        }

        roiNode->SetXYZ(center);
        roiNode->SetRadiusXYZ(xyz);
    }
}

QString qSlicerNaviModuleWidget::GetSegmentStastics(vtkMRMLSegmentationNode* snode) {
    vtkOrientedImageData* idata = vtkOrientedImageData::New();
    vtkSegment* segment = snode->GetSegmentation()->GetNthSegment(0);
    if (!segment) {
        return "";
    }
    snode->GetBinaryLabelmapRepresentation(snode->GetSegmentation()->GetSegmentIdBySegment(segment), idata);
    if (!idata) {
        return "";
    }
    int labelValue = 1;
    int backgroundValue = 0;
    vtkSmartPointer<vtkImageThreshold> thresh = vtkSmartPointer<vtkImageThreshold>::New();
    thresh->SetInputData(idata);
    thresh->ThresholdByLower(0);
    thresh->SetInValue(backgroundValue);
    thresh->SetOutValue(labelValue);
    thresh->SetOutputScalarType(VTK_UNSIGNED_CHAR);
    thresh->Update();

    vtkSmartPointer<vtkImageToImageStencil> stencil = vtkSmartPointer<vtkImageToImageStencil>::New();
    stencil->SetInputData(thresh->GetOutput());
    stencil->ThresholdByUpper(labelValue);
    stencil->Update();

    vtkSmartPointer<vtkImageAccumulate> stat = vtkSmartPointer<vtkImageAccumulate>::New();
    stat->SetInputData(thresh->GetOutput());
    stat->SetStencilData(stencil->GetOutput());
    stat->Update();

    auto num = stat->GetVoxelCount();
    return QStringLiteral("体积:%1").arg(num);
}

void qSlicerNaviModuleWidget::DoFresh3DListView()
{
    Q_D(qSlicerNaviModuleWidget);
    // list clear
    d->listWidget3DMode->disconnect();
    d->listWidget3DMode->clear();

    auto nodes = mrmlScene()->GetNodesByClass("vtkMRMLSegmentationNode");
    int numberOfSceneViews = nodes->GetNumberOfItems();
    for (int n = 0; n < numberOfSceneViews; ++n)
    {
        vtkMRMLSegmentationNode* t_node =
            vtkMRMLSegmentationNode::SafeDownCast(nodes->GetItemAsObject(n));
        auto structure_name = t_node->GetAttribute(NaviWidget3D::structure_name.c_str());
        if (!structure_name) {
            continue;
        }
        AddNodeTo3DList(t_node);
    }
    return;
}


void qSlicerNaviModuleWidget::AddNodeTo2DList(vtkMRMLScalarVolumeNode* procNode, bool b) {
    Q_D(qSlicerNaviModuleWidget);

    auto logic = qSlicerApplication::application()->applicationLogic()->GetModuleLogic("VolumeRendering");
    auto vrlogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(logic);
    char* name = procNode->GetName();
    const char* is_registed_node = procNode->GetAttribute("is_registed");
    const char* alias_name = procNode->GetAttribute("alias_name");

    {
        //procNode->RemoveAllDisplayNodeIDs();
        auto displayNode = procNode->GetDisplayNode();
        if (!displayNode) {
            procNode->CreateDefaultDisplayNodes();
        }
        /*vtkSmartPointer<vtkMRMLVolumeRenderingDisplayNode> displayNode1 =
            vtkSmartPointer<vtkMRMLVolumeRenderingDisplayNode>::Take(vrlogic->CreateVolumeRenderingDisplayNode());
        mrmlScene()->AddNode(displayNode1);
        procNode->AddAndObserveDisplayNodeID(displayNode1->GetID());
        vrlogic->UpdateDisplayNodeFromVolumeNode(displayNode1, procNode);*/
    }
    {
        NaviWidget2D* itemWidget;
        QListWidgetItem* item;
        int width = d->listWidget2DMode->geometry().width();

        itemWidget = new NaviWidget2D(d->listWidget2DMode);
        m_Map2D[QString::fromLatin1(alias_name)] = itemWidget;
        itemWidget->setEnabled(true);
        item = new QListWidgetItem(d->listWidget2DMode);
        d->listWidget2DMode->setItemWidget(item, itemWidget);
        d->listWidget2DMode->addItem(item);
        item->setSizeHint(QSize(width, 92));
        d->listWidget2DMode->setItemSelected(item, true);
        if (alias_name && std::string(alias_name) == "basic") {
            d->listWidget2DMode->insertItem(0, item);
        }
        vtkSlicerVolumeRenderingLogic* vrlLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(appLogic()->GetModuleLogic("VolumeRendering"));
        itemWidget->vrlLogic = vrlLogic;
        itemWidget->SetNode(mrmlScene(), m_PresetScene, procNode, GetVolumeRenderingDisplayNode(procNode));
        auto visible_2d = procNode->GetAttribute("visible_2d");
        auto visible_3d = procNode->GetAttribute("visible_3d");
        auto visible_roi = procNode->GetAttribute("visible_roi");
        auto color_node_id = procNode->GetAttribute("color_node_id");
        auto preset_shift = procNode->GetAttribute("preset_shift");
        auto node_roi = procNode->GetAttribute("node_roi");
        auto show_name = procNode->GetAttribute("show_name");
        auto extra_image_path = procNode->GetAttribute("extra_image_path");
        auto presetNodeID = procNode->GetAttribute("preset_index");

        if (extra_image_path) {
            QString p1 = QString::fromLatin1(extra_image_path);
            itemWidget->SetExtraImage(p1);
        }





        if (show_name) {
            itemWidget->SetName(QString::fromLocal8Bit(show_name));
        }
        connect(itemWidget, SIGNAL(onRongjiChanged()), this, SLOT(onItemRongjiChanged()));
        connect(itemWidget, SIGNAL(showBlendPro(NaviWidget2D*)), this, SLOT(showBlendProportion(NaviWidget2D*)));
        connect(itemWidget, SIGNAL(onOpacityChanged(int)), this, SLOT(onItemOpacityChanged(int)));
        connect(itemWidget, SIGNAL(visible_changed(NaviWidget2D*)), this, SLOT(OnVisibleChagned(NaviWidget2D*)));
        connect(itemWidget, SIGNAL(displayHiddenPanel(NaviWidget2D*)), this, SLOT(UpdateListWidget(NaviWidget2D*)));

        if (color_node_id) {
            auto color_node = mrmlScene()->GetNodeByID(color_node_id);
            itemWidget->DoSetColorTableNode(color_node);
        }
        else {
            auto grayNode = mrmlScene()->GetFirstNodeByName("Grey");
            itemWidget->DoSetColorTableNode(grayNode);
        }

        if (node_roi && visible_roi) {
            if (strcmp(visible_roi, "true") == 0 || strcmp(visible_roi, "1") == 0) {
                itemWidget->ui.checkBox_3->setChecked(true);
            }
            else {
                itemWidget->ui.checkBox_3->setChecked(false);
            }
        }

        bool use_old_preset = false;
        if (presetNodeID) {
            auto presetNode = vrlogic->GetPresetsScene()->GetNodeByID(presetNodeID);
            if (presetNode) {
                itemWidget->ui.PresetComboBox->blockSignals(true);
                itemWidget->ui.PresetComboBox->setCurrentNode(presetNode);
                itemWidget->ui.PresetComboBox->blockSignals(false);;
                use_old_preset = true;
            }
        }
        if (!use_old_preset) {
            if (procNode->GetImageData()) {
                double* scalarRange = procNode->GetImageData()->GetScalarRange();
                double scalarRangeSize = scalarRange[1] - scalarRange[0];
                auto vspNode = vtkMRMLVolumeRenderingDisplayNode::SafeDownCast(vrlLogic->GetFirstVolumeRenderingDisplayNode(procNode));

                if (strcmp(name, "VesselnessFiltered") == 0) {
                    itemWidget->ui.PresetComboBox->setCurrentNode(vrlogic->GetPresetByName("Vesselness"));
                }
                else if (procNode->GetImageData()->GetScalarType() == VTK_UNSIGNED_CHAR)
                {
                    itemWidget->ui.PresetComboBox->setCurrentNode(vrlogic->GetPresetByName("US-Fetal"));
                }

                else if (scalarRangeSize > 50.0 && scalarRangeSize < 1500.0 && vrlogic->GetPresetByName("MR-Default"))
                {
                    itemWidget->ui.PresetComboBox->setCurrentNode(vrlogic->GetPresetByName("MR-Default"));
                }
                else if (scalarRangeSize >= 1500.0 && scalarRangeSize < 10000.0 && vrlogic->GetPresetByName("CT-Chest-Contrast-Enhanced"))
                {
                    itemWidget->ui.PresetComboBox->setCurrentNode(vrlogic->GetPresetByName("CT-Chest-Contrast-Enhanced"));
                }
            }
        }

        if (visible_3d) {
            if (strcmp(visible_3d, "true") == 0 || strcmp(visible_3d, "1") == 0) {
                itemWidget->ui.chkVolume->setChecked(true);
            }
            else {
                itemWidget->ui.chkVolume->setChecked(false);
            }
        }

        if (visible_2d) {
            if (strcmp(visible_2d, "true") == 0 || strcmp(visible_2d, "1") == 0) {
                itemWidget->ui.checkBox->setChecked(true);
            }
            else {
                itemWidget->ui.checkBox->setChecked(false);
            }
        }

    }

}


void qSlicerNaviModuleWidget::AddNodeTo3DList(vtkMRMLSegmentationNode* t_node) {
    Q_D(qSlicerNaviModuleWidget);
    auto structure_name = t_node->GetAttribute(NaviWidget3D::structure_name.c_str());
    auto volumes = GetSegmentStastics(t_node);
    NaviWidget3D* itemWidget = new NaviWidget3D();
    Widget3DData* widget3dData = new Widget3DData();
    itemWidget->SetData(widget3dData);
    QListWidgetItem* item = new QListWidgetItem(d->listWidget3DMode);
    d->listWidget3DMode->setItemWidget(item, itemWidget);
    d->listWidget3DMode->addItem(item);

    //NaviWidget3D* itemWidget2 = new NaviWidget3D();
    //itemWidget2->SetData(widget3dData);
    //QListWidgetItem* item2 = new QListWidgetItem(d->listWidget2DMode);
    //d->listWidget2DMode->setItemWidget(item2, itemWidget2);
    //d->listWidget2DMode->addItem(item2);
    ////connect(itemWidget2, SIGNAL(need_update()), this, SLOT(update_show()));
    //connect(itemWidget2, SIGNAL(removeWidget3D(NaviWidget3D*)), this, SLOT(onRemoveSegment(NaviWidget3D*)));
    //connect(itemWidget2, SIGNAL(modifyWidget3D(NaviWidget3D*)), this, SLOT(onModifySegment(NaviWidget3D*)));
    ////connect(itemWidget2, SIGNAL(clicked(NaviWidget3D*)), this, SLOT(on_click_widget(NaviWidget3D*)));
    ////connect(itemWidget2, SIGNAL(updateOpacityAndColor()), this, SLOT(Update3DOpacityAndColor()));


    int width = d->listWidget3DMode->geometry().width();
    item->setSizeHint(QSize(width, 110));
    //item2->setSizeHint(QSize(width, 110));

    //connect(itemWidget, SIGNAL(need_update()), this, SLOT(update_show()));
    connect(itemWidget, SIGNAL(removeWidget3D(NaviWidget3D*)), this, SLOT(onRemoveSegment(NaviWidget3D*)));
    connect(itemWidget, SIGNAL(modifyWidget3D(NaviWidget3D*)), this, SLOT(onModifySegment(NaviWidget3D*)));
    //connect(itemWidget, SIGNAL(clicked(NaviWidget3D*)), this, SLOT(on_click_widget(NaviWidget3D*)));
    //connect(itemWidget, SIGNAL(updateOpacityAndColor()), this, SLOT(Update3DOpacityAndColor()));
    widget3dData->segmentDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(t_node->GetDisplayNode());
    widget3dData->m_SegmentationNode = t_node;
    widget3dData->segment = t_node->GetSegmentation()->GetNthSegment(0);
    widget3dData->segmentID = "0";
    const char* special_type = t_node->GetAttribute("special_type");

    //for HDBrainSegmentations Only
    if (special_type) {
        t_node->CreateClosedSurfaceRepresentation();
        vtkMRMLSegmentationDisplayNode* displayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(t_node->GetDisplayNode());
        displayNode->SetVisibility(1);
        displayNode->SetAllSegmentsVisibility(true);
    }
    auto visible_2d = t_node->GetAttribute(NaviWidget3D::visible_2d.c_str());
    auto visible_3d = t_node->GetAttribute(NaviWidget3D::visible_3d.c_str());
    auto opacity_all = t_node->GetAttribute(NaviWidget3D::opacity_all.c_str());
    auto color_red = t_node->GetAttribute(NaviWidget3D::color_red.c_str());
    auto color_green = t_node->GetAttribute(NaviWidget3D::color_green.c_str());
    auto color_blue = t_node->GetAttribute(NaviWidget3D::color_blue.c_str());
    auto master_node_id = t_node->GetAttribute(NaviWidget3D::master_node_id.c_str());

    if (master_node_id) {
        auto master_node = mrmlScene()->GetNodeByID(master_node_id);
        if (master_node) {
            widget3dData->m_MasterNode = vtkMRMLScalarVolumeNode::SafeDownCast(master_node);
        }
    }
    widget3dData->m_SegmentationNode = t_node;
    widget3dData->segmentDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(t_node->GetDisplayNode());
    if (color_red) {
        widget3dData->m_red = QString::fromLocal8Bit(color_red).toInt();
        widget3dData->m_green = QString::fromLocal8Bit(color_green).toInt();
        widget3dData->m_blue = QString::fromLocal8Bit(color_blue).toInt();
        itemWidget->freshColor();
        //itemWidget2->freshColor();
    }

    if (visible_2d) {
        if (QString::fromLocal8Bit(visible_2d) == "true") {
            itemWidget->ui.btnVisible->setChecked(true);
            //itemWidget2->ui.btnVisible->setChecked(true);
        }
        else {
            itemWidget->ui.btnVisible->setChecked(false);
            //itemWidget2->ui.btnVisible->setChecked(false);
        }
    }
    if (structure_name == QStringLiteral("肿瘤") ||
        structure_name == QStringLiteral("计划消融区")) {
        itemWidget->ui.lblName_2->setText(volumes);
        //itemWidget2->ui.lblName_2->setText(volumes);
    }
    else {
        itemWidget->ui.lblName_2->setText("");
        //itemWidget2->ui.lblName_2->setText("");
    }

    if (visible_3d) {
        if (QString::fromLocal8Bit(visible_3d) == "true") {
            itemWidget->ui.btn3D->setChecked(true);
            //itemWidget2->ui.btn3D->setChecked(true);
        }
        else {
            itemWidget->ui.btn3D->setChecked(false);
            //itemWidget2->ui.btn3D->setChecked(false);
        }
    }

    if (opacity_all) {
        int opa = QString::fromLocal8Bit(opacity_all).toInt();
        itemWidget->ui.horizontalSlider->setValue(opa);
        // itemWidget2->ui.horizontalSlider->setValue(opa);
    }

    itemWidget->SetTitle(QString::fromLocal8Bit(structure_name));
    //itemWidget2->SetTitle(QString::fromLocal8Bit(structure_name));
}

void qSlicerNaviModuleWidget::onSegmentAdded(vtkObject*, void* callData) {
    Q_D(qSlicerNaviModuleWidget);
    const char* segmentIDPtr = reinterpret_cast<const char*>(callData);
    if (!segmentIDPtr) {
        return;
    }
    std::string segmentIDStr = std::string(segmentIDPtr);
    if (m_State == qSlicerNaviModuleWidget::WaitForAddSegment) {
        vtkMRMLSegmentationNode* snode = vtkMRMLSegmentationNode::SafeDownCast(d->EffectsWidget->segmentationNode());
        vtkSegment* seg = snode->GetSegmentation()->GetSegment(segmentIDPtr);
        vtkMRMLSegmentationDisplayNode* display = vtkMRMLSegmentationDisplayNode::SafeDownCast(snode->GetDisplayNode());
        d->m_Temp3DWidget->m_Data->segmentID = segmentIDPtr;
        d->m_Temp3DWidget->m_Data->segment = seg;
        d->m_Temp3DWidget->SetSegmentInfo(seg, segmentIDPtr, display,snode);
    }
    m_State = qSlicerNaviModuleWidget::HoldSegment;
    //d->tabWidgetModel->setCurrentIndex(3);
}


void qSlicerNaviModuleWidget::onSegmentRemoved() {
    Q_D(qSlicerNaviModuleWidget);
    
}


void qSlicerNaviModuleWidget::onSegmentModified() {
    
}

void qSlicerNaviModuleWidget::onEnterInit() {
    Q_D(qSlicerNaviModuleWidget);
   
    DoInitTemp3DWidget();
    if(d->EffectsWidget->mrmlScene() == nullptr){
    d->EffectsWidget->setMRMLScene(mrmlScene());
    d->EffectsWidget->updateEffectList();
    }
    GLTool::progressBar->setProgressValue(31);
    DoFresh2DListView();
    DoFreshPhaseListView();
    DoFresh3DListView();
    DoFreshStructureListView();
    DoFreshFiberListView();
    AddSignalNode("layout_4", QStringList());
    GLTool::progressBar->setProgressValue(41);
    vtkSlicerVolumeRenderingLogic* vrlLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(appLogic()->GetModuleLogic("VolumeRendering"));
    m_PresetScene = vrlLogic->GetPresetsScene();
    GLTool::progressBar->setProgressValue(51);
    d->tabWidget->setCurrentIndex(0);
    d->tabWidgetModel->setCurrentIndex(0);
    d->btn2D->animateClick(10);
    GLTool::progressBar->setProgressValue(71);
    for (int i = 0; i < pyModulesNames.size(); i++) {
        qInfo() << pyModulesNames.at(i) << "->is entering";
       pyModuleContainers.at(i)->setModule(pyModules.at(i));
       pyModuleContainers.at(i)->show();
    }
    GLTool::progressBar->setProgressValue(81);
    d->logic()->AddObserver(vtkSlicerNaviLogic::EventType::CloseNaviSettingPanel, this->m_CallbackCommand);
    d->logic()->AddObserver(vtkSlicerNaviLogic::EventType::Fresh2DView, this->m_CallbackCommand);
    d->logic()->AddObserver(vtkSlicerNaviLogic::EventType::Fresh3DView, this->m_CallbackCommand);
    ATB(0); //@SXM
    DoShowDepthestNode();
    GLTool::progressBar->setProgressValue(100);
    GLTool::progressBar->close();
}



void qSlicerNaviModuleWidget::onGotoPreviewPage() {
    qSlicerApplication::application()->layoutManager()->setCurrentPage(qSlicerApplication::application()->layoutManager()->m_Page - 1);
}

void qSlicerNaviModuleWidget::onItemRongjiChanged() {
    std::vector< NaviBase2D*> tmpVector;
    for (auto it = m_Map2D.begin(); it != m_Map2D.end(); it++)
    {
        NaviWidget2D* val = it.value();
        if (val && val->m_Rongji == 1) {
            tmpVector.push_back(val);
        }
    }

    if (tmpVector.size() > 1) {
        m_Rongji0 = tmpVector.at(0);
        m_Rongji1 = tmpVector.at(1);
        DoSelectVolumeNode(m_Rongji0->m_Node, m_Rongji1->m_Node, 0.5);
    }
}


void qSlicerNaviModuleWidget::onItemOpacityChanged(int value) {
    DoSelectVolumeNode(m_Rongji0->m_Node, m_Rongji1->m_Node, value/100.0);
}

void qSlicerNaviModuleWidget::enter() {
    this->Superclass::enter();
    Q_D(qSlicerNaviModuleWidget);

    GLTool::progressBar = new qSlicerProgressBar;
    GLTool::progressBar->InitProgressBar(QStringLiteral("正在打开模块..."), 0, 100);
    //GLTool::progressBar->show();
    GLTool::progressBar->setProgressValue(1);
    qInfo() << "qSlicerNaviModuleWidget::enter";
    SetGlobalSaveValue("navi_set", "1");
    GLTool::progressBar->setProgressValue(11);
    SetGlobalSaveValue("current_page", "4");
    GLTool::progressBar->setProgressValue(21);
    onEnterInit();
    
}

void qSlicerNaviModuleWidget::DoUpdateToServer() {
   /* vtkDataTransfer* dt = vtkDataTransfer::New();
    char* dest = "http://127.0.0.1/index.php";
    dt->SetSourceURI("E:/S/README.txt");
    dt->SetDestinationURI(dest);
    dt->SetHandler(vtkHTTPHandler::New());
    dt->SetTransferType(vtkDataTransfer::RemoteUpload);
    vtkSmartPointer<vtkDataIOManagerLogic> dataIOManagerLogic;
    dataIOManagerLogic = vtkSmartPointer<vtkDataIOManagerLogic>::New();
    dataIOManagerLogic->SetMRMLApplicationLogic(appLogic());
    dataIOManagerLogic->SetMRMLScene(mrmlScene());
    bool b = dt->GetHandler()->CanHandleURI(dest);
    dataIOManagerLogic->ApplyTransfer(dt);*/

    /*vtkDataTransfer* dt = vtkDataTransfer::New();
    char* dest = "http://127.0.0.1/index.php";
    dt->SetSourceURI(dest);
    dt->SetDestinationURI("E:/S/README.txt");
    dt->SetHandler(vtkHTTPHandler::New());
    dt->SetTransferType(vtkDataTransfer::RemoteDownload);
    vtkSmartPointer<vtkDataIOManagerLogic> dataIOManagerLogic;
    dataIOManagerLogic = vtkSmartPointer<vtkDataIOManagerLogic>::New();
    dataIOManagerLogic->SetMRMLApplicationLogic(appLogic());
    dataIOManagerLogic->SetMRMLScene(mrmlScene());
    bool b = dt->GetHandler()->CanHandleURI(dest);
    dataIOManagerLogic->ApplyTransfer(dt);*/
}

void qSlicerNaviModuleWidget::doTestAddData() {
    

    QString filename = "test.ini";
    QSettings* settings = new QSettings(filename, QSettings::IniFormat);
    QString paths = settings->value("loaded_dicoms").toString();
    QString start_page = settings->value("boot_config/start_page").toString();
    if (start_page != "4") {
        return;
    }
    qSlicerIOManager* iomanager = qSlicerApplication::application()->ioManager();

    QStringList datas = settings->value("loaded_dicoms/test_navi").toStringList();
    for (int i = 0; i < datas.size(); i++) {
        QString val = datas.at(i);
        iomanager->loadFile(val);
        std::string name = itksys::SystemTools::GetFilenameWithoutExtension(val.toStdString());
        vtkMRMLScalarVolumeNode* node_t = vtkMRMLScalarVolumeNode::SafeDownCast(mrmlScene()->GetFirstNodeByName(name.c_str()));
        node_t->SetAttribute("alias_name", name.c_str());
        node_t->SetAttribute("is_registed","true");
        if (i == 0) {
            node_t->SetAttribute("alias_name", "basic");
        }
    }


    

    vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
    vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
    if (lnode) {
        lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutThreeOverThreeView);
    }


    /*vtkMRMLNode* nodef = mrmlScene()->GetFirstNodeByName("T1");
    vtkMRMLScalarVolumeNode* volume = vtkMRMLScalarVolumeNode::SafeDownCast(nodef);
    double* spacing = volume->GetSpacing();
    double spa[3];
    spa[0] = spacing[0];
    spa[1] = spacing[1];
    spa[2] = spacing[2];
    
    auto origin = volume->GetOrigin();
    vtkMatrix4x4* matirx = vtkMatrix4x4::New();
    volume->GetRASToIJKMatrix(matirx);
    vtkMatrix4x4* matirx1 = vtkMatrix4x4::New();
    volume->GetIJKToRASMatrix(matirx1);
    vtkMatrix4x4* matirx2 = vtkMatrix4x4::New();
    volume->GetIJKToRASDirectionMatrix(matirx2);
    auto volume1 = vtkSlicerVolumesLogic::CloneVolume(mrmlScene(), volume, "abc", true);
    DoClearVolume(volume1);


    volume1->SetAttribute("alias_name", "aaaaa");
    volume1->SetAttribute("is_registed", "true");*/

}

void qSlicerNaviModuleWidget::onFiberDirection() {
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    widget->OnFiberViewDirection();
}

void qSlicerNaviModuleWidget::onChangeViewDirection(int i) {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        d->cmbDir->setCurrentIndex(0);
        return;
    }
    if (i == 1) {
        onNormalViewDirection();
    }
    if (i == 2) {
        widget->OnFiberViewDirection();
    }
    if (i == 3) {
        d->logic()->InvokeEvent(vtkSlicerNaviLogic::EventType::OpenDoctorViewPanel);
        d->tabWidget_3->setCurrentIndex(6);
    }
    d->cmbDir->setCurrentIndex(0);
}


void qSlicerNaviModuleWidget::onUpdateAblationAreaInfo(int num) {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    if (d->cmbAblation->currentIndex() == 0) {
        d->label_11->setText(QString(""));
        return;
    }
    auto t1 = widget->m_FiberData->start_pos[d->cmbAblation->currentIndex()];
    auto t2 = widget->m_FiberData->start_pos[d->cmbAblation->currentIndex()-1];
    d->label_11->setText(QString("%1mm").arg(t1-t2));
}

void qSlicerNaviModuleWidget::onDeCreaseAblationDistance() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    widget->m_FiberData->start_pos[d->cmbAblation->currentIndex()] -= 0.5;
    widget->update_ablation_position(d->cmbAblation->currentIndex());
    onUpdateAblationAreaInfo(0);
}

void qSlicerNaviModuleWidget::onInCreaseAblationDistance() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    widget->m_FiberData->start_pos[d->cmbAblation->currentIndex()] +=0.5;
    widget->update_ablation_position(d->cmbAblation->currentIndex());
    onUpdateAblationAreaInfo(0);
}

void qSlicerNaviModuleWidget::update_ablation_number() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    d->cmbAblation->clear();
    auto m_AblationNum = widget->m_FiberData->m_AblationNum;
    for (int i = 0; i < m_AblationNum; i++) {
        if (i > 0) {
            d->cmbAblation->addItem(QString("%1").arg(i+1));
        }
        else {
            d->cmbAblation->addItem(QStringLiteral("选择"));
        }
    }
}

void qSlicerNaviModuleWidget::onAddAblationArea() {
    Q_D(qSlicerNaviModuleWidget);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    widget->OnAddAblationArea();
}

void qSlicerNaviModuleWidget::onAdjustFiberPoint() {
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    widget->onAdjustFiberPoint();
    widget->ChangeWave();
}

void qSlicerNaviModuleWidget::onRemoveAblationArea() {
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    widget->onRemoveAblationArea();
}

void qSlicerNaviModuleWidget::onNormalViewDirection() {
    auto lm = qSlicerApplication::application()->layoutManager();
    auto sliceNames = lm->sliceViewNames();
    for (int i = 0; i < sliceNames.size(); i++) {
        auto node = lm->sliceWidget(sliceNames.at(i))->mrmlSliceNode();
        node->SetOrientationToDefault();
    }
    Reinit();
}

void qSlicerNaviModuleWidget::onNewOpticalFiber() {
    Q_D(qSlicerNaviModuleWidget);
   /* for (int i = 0; i < TestFiberWidgetVector.size(); i++) {
        TestFiberWidgetVector.at(i)->m_FiberData->m_Active = false;
    }*/

    QString len1 = QString::number(d->listWidgetOpticalFiber->count()+1);
    QString tagname = QStringLiteral("光纤") + len1;

    auto fiber_ptr = new FiberData();
    /*point_set_node->SetStringProperty("stl_point_set_name", tagname.toLatin1());
    point_set_node->SetVisibility(false);
    auto fiber_ptr = new FiberData(tagname, point_set_node);

    */
    NaviWidgetFiber* itemWidget = new NaviWidgetFiber(fiber_ptr, true, d->listWidgetOpticalFiber, this->mrmlScene(), d->logic());
    TestFiberWidgetVector.append(itemWidget);
    itemWidget->SetDeleteable(false);
    itemWidget->ui.textEdit->setText(tagname);
    QListWidgetItem* item = new QListWidgetItem(d->listWidgetOpticalFiber);
    d->listWidgetOpticalFiber->setItemWidget(item, itemWidget);
    d->listWidgetOpticalFiber->addItem(item);
    //itemWidget->Refresh();
    if (!itemWidget->m_FiberData->m_IsShuqian) {
        int width = d->listWidgetOpticalFiber->geometry().width();
        item->setSizeHint(QSize(width, 140));
    }
    else {
        int width = d->listWidgetOpticalFiber->geometry().width();
        item->setSizeHint(QSize(width, 489));
    }


    connect(itemWidget, SIGNAL(delete_fiber(NaviWidgetFiber*)), this, SLOT(OnDeleteFiberWidget(NaviWidgetFiber*)));
    //connect(itemWidget, SIGNAL(need_update()), this, SLOT(update_show()));
    //connect(itemWidget, SIGNAL(show_fiber(NaviWidgetFiber*, QString)), this, SLOT(on_show_fiber(NaviWidgetFiber*, QString)));
    //connect(itemWidget, SIGNAL(move_fiber(NaviWidgetFiber*)), this, SLOT(on_move_fiber(NaviWidgetFiber*)));
    
    connect(itemWidget, SIGNAL(clicked(NaviWidgetFiber*)), this, SLOT(UpdateFiberList(NaviWidgetFiber*)));
    connect(itemWidget, &NaviWidgetFiber::ablation_number_changed, this, &qSlicerNaviModuleWidget::update_ablation_number);
    connect(itemWidget, &NaviWidgetFiber::rotate_to_vector, this, &qSlicerNaviModuleWidget::on_rotate_to_vector);
    connect(itemWidget, &NaviWidgetFiber::fiber_depth, this, &qSlicerNaviModuleWidget::on_fiber_depth);
    connect(itemWidget, &NaviWidgetFiber::estimate, this, &qSlicerNaviModuleWidget::on_estimate);
    connect(itemWidget, &NaviWidgetFiber::center_target, this, &qSlicerNaviModuleWidget::DoCenterTargetPoint);
    connect(itemWidget, &NaviWidgetFiber::center_input, this, &qSlicerNaviModuleWidget::DoCenterInputPoint);
    itemWidget->SetFixedFiberType(fiber_ptr->m_FiberType);
    
    int fiberCount = d->listWidgetOpticalFiber->count();
    m_LastFiberItem = d->listWidgetOpticalFiber->item(fiberCount - 1);
    m_WidgetItem2Fiber[item] = itemWidget;
    for(int i= fiberCount -1;i>0;i--)
    {
        QListWidgetItem* item = d->listWidgetOpticalFiber->item(i-1);
        item->setSizeHint(QSize(540, 140));
    
    }

    auto sliceController = qSlicerApplication::application()->layoutManager()->sliceWidget("Red")->sliceController();
    auto slider = sliceController->sliceOffsetSlider();
    auto max = slider->maximum();
    auto min = slider->minimum();
    auto value = slider->value();
    auto index = (value - min) * 400 / (max - min);
    itemWidget->SetSliderValue(index);
}

void qSlicerNaviModuleWidget::onTabBarClicked(int index) {
    Q_D(qSlicerNaviModuleWidget);
    if (index == 0) {
        d->btn2D->animateClick(10);
    }
    if (index == 1) {
        DoFreshFiberListView();
        d->tabWidget_3->setCurrentIndex(0);
    }
}

void qSlicerNaviModuleWidget::UpdateFiberList(NaviWidgetFiber* fiber)
{
    Q_D(qSlicerNaviModuleWidget);
    QListWidgetItem* listitem = nullptr;
	for (QMap< QListWidgetItem*, NaviWidgetFiber* >::const_iterator it = m_WidgetItem2Fiber.begin();it!=m_WidgetItem2Fiber.end();++it)
	{
		if (it.value() == fiber)
		{
            listitem = it.key();
			break;
		}
	}
    if (!listitem || listitem == m_LastFiberItem) {
        return;
    }
	int width = d->listWidgetOpticalFiber->geometry().width();
    listitem->setSizeHint(QSize(width, 489));
    if (m_LastFiberItem)
    {
        m_LastFiberItem->setSizeHint(QSize(width, 140));
    }
    m_LastFiberItem = listitem;
    update_ablation_number();
    doFreshCoordinate();
    d->logic()->InvokeEvent(11311);
}

//@ww
//int now_click, last_click = 0;
//void qSlicerNaviModuleWidget::on_click_fiber_widget(NaviWidgetFiber* widget) {
//    Q_D(qSlicerNaviModuleWidget);
//
//    int now_click = d->listWidgetOpticalFiber->currentRow();
//    
//       
//    if (now_click == -1)
//        now_click = 0;
//
//    if (now_click != d->listWidgetOpticalFiber->count() - 1) {
//        QListWidgetItem* nowitem0 = d->listWidgetOpticalFiber->item(d->listWidgetOpticalFiber->count() - 1);
//        nowitem0->setSizeHint(QSize(540, 140));
//    }
//
//    QListWidgetItem* nowitem = d->listWidgetOpticalFiber->item(now_click);
//    nowitem->setSizeHint(QSize(540, 414));
//
//
//    if (now_click != last_click) {
//        QListWidgetItem* lastitem = d->listWidgetOpticalFiber->item(last_click);
//        if(lastitem){
//        lastitem->setSizeHint(QSize(540, 140));
//        last_click = now_click;
//        }
//    }
//    widget->OnFiberViewDirection();
//    auto sliceController = qSlicerApplication::application()->layoutManager()->sliceWidget("Red")->sliceController();
//    auto slider = sliceController->sliceOffsetSlider();
//    auto max = slider->maximum();
//    auto min = slider->minimum();
//    auto value = slider->value();
//    auto index = (value - min) * 400 / (max - min);
//    widget->SetSliderValue(index);
//    doFreshCoordinate();
//    update_ablation_number();
//    onUpdateAblationAreaInfo(0);
//}


vtkSmartPointer<vtkImageData> qSlicerNaviModuleWidget::polyDataToImageData(vtkSmartPointer<vtkPolyData> polydata, double spacing[3])
{
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    double bounds[6];
    polydata->GetBounds(bounds);
    imageData->SetSpacing(spacing);

    //compute dimensions
    int dim[3];
    for (int i = 0; i < 3; ++i) {
        dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
    }
    imageData->SetDimensions(dim);
    imageData->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

    double origin[3];
    origin[0] = bounds[0] + spacing[0] / 2;
    origin[1] = bounds[2] + spacing[1] / 2;
    origin[2] = bounds[4] + spacing[2] / 2;
    imageData->SetOrigin(origin);

    imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

    //fill the image with foreground voxels:
    unsigned char inval = 255;
    unsigned char outval = 0;
    vtkIdType count = imageData->GetNumberOfPoints();
    for (vtkIdType i = 0; i < count; ++i) {
        imageData->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }

    //polygonal data --> image stencil:
    vtkSmartPointer<vtkPolyDataToImageStencil> pdtoImageStencil = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    pdtoImageStencil->SetInputData(polydata);
    pdtoImageStencil->SetOutputOrigin(origin);
    pdtoImageStencil->SetOutputSpacing(spacing);
    pdtoImageStencil->SetOutputWholeExtent(imageData->GetExtent());
    pdtoImageStencil->Update();

    //cut the corresponding white image and set the background:
    vtkSmartPointer<vtkImageStencil> imageStencil = vtkSmartPointer<vtkImageStencil>::New();
    imageStencil->SetInputData(imageData);
    imageStencil->SetStencilConnection(pdtoImageStencil->GetOutputPort());
    imageStencil->ReverseStencilOff();
    imageStencil->SetBackgroundValue(outval);
    imageStencil->Update();

    imageData->DeepCopy(imageStencil->GetOutput());
    return imageData;
}

void qSlicerNaviModuleWidget::DoVolumeMapImageData(vtkMRMLScalarVolumeNode* volume, vtkImageData* imageData,vtkMatrix4x4* segment_ras_to_ijk_matrix) {
    vtkMatrix4x4* rasToIjk = vtkMatrix4x4::New();
    volume->GetRASToIJKMatrix(rasToIjk);
    double bounds[6] = { 1,-1,1,-1,1,-1 };
    auto extent  = imageData->GetExtent();
    for (int i = extent[0]; i < extent[1]; i++)
        for (int j = extent[2]; j < extent[3]; j++)
            for (int k = extent[4]; k < extent[5]; k++) {
                double arr2[4];
                double tt[4] = { 1 };
                tt[0] = i;
                tt[1] = j;
                tt[2] = k;
                tt[3] = 1;
                double ss[4] = { 1 };
                segment_ras_to_ijk_matrix->MultiplyPoint(tt, ss);
                
                unsigned char* pPixel = (unsigned char*)(imageData->GetScalarPointer(round(tt[0]), round(tt[1]), round(tt[2])));
                if (!pPixel) {
                    continue;
                }
                auto t1 = *(pPixel + 0);
                auto t2 = *(pPixel + 1);
                auto t3 = *(pPixel + 2);
                if (t1 == 0 && t2 == 0 && t3 == 0) {

                }
                else {
                    rasToIjk->MultiplyPoint(ss, arr2);
                    unsigned char* pixel = (unsigned char*)(volume->GetImageData()->GetScalarPointer(arr2[0], arr2[1], arr2[2]));
                    if (!pixel) {
                        continue;
                    }
                    *pixel = 1;
                    *(pixel + 1) = 1;
                    *(pixel + 2) = 1;
                }

            }
}

void qSlicerNaviModuleWidget::DoVolumeMapImageData(vtkMRMLScalarVolumeNode* volume, vtkOrientedImageData* imageData) {
    vtkMatrix4x4* rasToIjk = vtkMatrix4x4::New();
    volume->GetRASToIJKMatrix(rasToIjk);
    vtkMatrix4x4* segment_ras_to_ijk_matrix = vtkMatrix4x4::New();
    imageData->GetWorldToImageMatrix(segment_ras_to_ijk_matrix);
    double bounds[6] = { 1,-1,1,-1,1,-1 };
    imageData->GetBounds(bounds);
    for (int i = bounds[0]; i < bounds[1]; i++)
        for (int j = bounds[2]; j < bounds[3]; j++)
            for (int k = bounds[4]; k < bounds[5]; k++) {
                double arr2[4];
                double tt[4] = { 1 };
                tt[0] = i;
                tt[1] = j;
                tt[2] = k;
                tt[3] = 1;
                double ss[4] = { 1 };
                segment_ras_to_ijk_matrix->MultiplyPoint(tt, ss);
                unsigned char* pPixel = (unsigned char*)(imageData->GetScalarPointer(round(ss[0]), round(ss[1]), round(ss[2])));
                if (!pPixel) {
                    continue;
                }
                auto t1 = *(pPixel + 0);
                auto t2 = *(pPixel + 1);
                auto t3 = *(pPixel + 2);
                if (t1 == 0 && t2 == 0 && t3 == 0) {

                }
                else {
                    rasToIjk->MultiplyPoint(tt, arr2);
                    unsigned char* pixel = (unsigned char*)(volume->GetImageData()->GetScalarPointer(arr2[0], arr2[1], arr2[2]));
                    if (!pixel) {
                        continue;
                    }
                    if (*pixel == 1) {
                        *pixel = 2;
                        *(pixel + 1) = 2;
                        *(pixel + 2) = 2;
                    }
                    else {
                        *pixel = 1;
                        *(pixel + 1) = 1;
                        *(pixel + 2) = 1;
                    }
                    
                }

            }
}

void qSlicerNaviModuleWidget::DoVolumeMapModel(vtkMRMLScalarVolumeNode* volume, vtkMRMLModelNode* model) {
    double* spacing = volume->GetSpacing();
    double spa[3];
    spa[0] = spacing[0];
    spa[1] = spacing[1];
    spa[2] = spacing[2];
    auto fiberimage1 = polyDataToImageData(model->GetPolyData(), spa);
    DoVolumeMapImageData(volume, fiberimage1, model->GetParentTransformNode()->GetMatrixTransformToParent());
}

//void qSlicerNaviModuleWidget::DoVolumeMapModel(vtkMRMLScalarVolumeNode* volume, vtkMRMLModelNode* model) {
//    double bounds[6] = { 1 };
//    model->GetBounds(bounds);
//}


void qSlicerNaviModuleWidget::DoClearVolume(vtkMRMLScalarVolumeNode* volume) {
    auto image1 = volume->GetImageData();

    auto extentFiber = image1->GetExtent();
    for (int i = extentFiber[0]; i < extentFiber[1]; i++)
        for (int j = extentFiber[2]; j < extentFiber[3]; j++)
            for (int k = extentFiber[4]; k < extentFiber[5]; k++)
            {
                unsigned char* pixel = (unsigned char*)(volume->GetImageData()->GetScalarPointer(i, j, k));
                *pixel = 0;
                *(pixel + 1) = 0;
                *(pixel + 2) = 0;
            }
}


void qSlicerNaviModuleWidget::DoStatisticVolume(vtkMRMLScalarVolumeNode* volume, double bounds[6]) {
    auto image1 = volume->GetImageData();
    auto node = mrmlScene()->GetFirstNodeByName("pointset007");
    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    volume->GetRASToIJKMatrix(matrix);
    if (!node) {
        vtkMRMLMarkupsFiducialNode* mnode = vtkMRMLMarkupsFiducialNode::New();
        mnode->SetName("pointset007");
        mrmlScene()->AddNode(mnode);
    }
    node = mrmlScene()->GetFirstNodeByName("pointset007");
    vtkMRMLMarkupsFiducialNode* fnode = vtkMRMLMarkupsFiducialNode::SafeDownCast(node);
    fnode->RemoveAllControlPoints();
    for (double i =  bounds[0]; i < bounds[1]; i++)
        for (double j = bounds[2]; j < bounds[3]; j++)
            for (double k = bounds[4]; k < bounds[5]; k++)
            {
                double arr1[4] = { i,j,k,1 };
                double arr2[4] = { 1 };
                matrix->MultiplyPoint(arr1, arr2);
                unsigned char* pixel = (unsigned char*)(volume->GetImageData()->GetScalarPointer(round(arr2[0]), round(arr2[1]), round(arr2[2])));
                if (!pixel) {
                    continue;
                }
                if (*pixel == 2) {
                    fnode->AddFiducial(i,j,k);
                }
            }
            
    
    
}

void qSlicerNaviModuleWidget::on_estimate(NaviWidgetFiber* fiber) 
{
    Q_D(qSlicerNaviModuleWidget);

    PythonQt::init();
    PythonQtObjectPtr context = PythonQt::self()->getMainModule();
    auto segment_node_id = fiber->m_FiberData->m_ProtectionNode->GetID();
    if (!segment_node_id) {
        return;
    }
    if (!fiber->m_FiberData->m_FiberVesselNode) {
        fiber->m_FiberData->m_FiberVesselNode = vtkMRMLScalarVolumeNode::SafeDownCast(mrmlScene()->AddNewNodeByClass("vtkMRMLScalarVolumeNode"));
    }
    fiber->m_FiberData->m_FiberVesselNode->SetAttribute("FiberVesselGuest", "1");
    fiber->m_FiberData->m_ProtectionNode->SetAttribute("FiberVesselHost", "1");
    
    context.evalScript(QString("import slicer.util as util; vessel_logic = util.getModuleLogic('VesselSegment'); vessel_logic.estimate()"));
    fiber->m_FiberData->m_ProtectionNode->SetAttribute("FiberVesselHost", nullptr);
    fiber->m_FiberData->m_FiberVesselNode->SetAttribute("FiberVesselGuest",nullptr);

    auto vessel_node_real = mrmlScene()->GetFirstNodeByName("VesselnessFiltered");
    auto color_node = mrmlScene()->GetFirstNodeByName("ProceduralColor");
    auto displ = fiber->m_FiberData->m_FiberVesselNode->GetDisplayNode();
    if (!displ) {
        fiber->m_FiberData->m_FiberVesselNode->CreateDefaultDisplayNodes();
        displ = fiber->m_FiberData->m_FiberVesselNode->GetDisplayNode();
    }
    if(color_node)
        displ->SetAndObserveColorNodeID(color_node->GetID());
    DoSelectVolumeNode(vessel_node_real, fiber->m_FiberData->m_FiberVesselNode, fiber->ui.textEdit_2->toPlainText().toInt()/100.0);
}

void qSlicerNaviModuleWidget::on_fiber_depth(NaviWidgetFiber* fiber, int index) {
    Q_D(qSlicerNaviModuleWidget);

    auto sliceController = qSlicerApplication::application()->layoutManager()->sliceWidget("Red")->sliceController();
    auto slider = sliceController->sliceOffsetSlider();
    auto max = slider->maximum();
    auto min = slider->minimum();
    auto val = min+(max - min) / fiber->m_BaseInfo.length * index;
    slider->setValue(val);
    


    //on_estimate(fiber);
}



void qSlicerNaviModuleWidget::on_rotate_to_vector(double* m_PointInput, double* m_PointTarget) {
    double dx = m_PointTarget[0];
    double dy = m_PointTarget[1];
    double dz = m_PointTarget[2];
    


    double vector1[3];
    vector1[0] = m_PointInput[0] - m_PointTarget[0];
    vector1[1] = m_PointInput[1] - m_PointTarget[1];
    vector1[2] = m_PointInput[2] - m_PointTarget[2];
    double vector0[3];
    vector0[0] = 0;
    vector0[1] = 0;
    vector0[2] = 1;
    double rotationVector_Local[3] = { 0.0 };
    double angle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(vector0, vector1));
    vtkMath::Cross(vector0, vector1, rotationVector_Local);
    vtkNew<vtkMatrix4x4> modelToParentTransform;
    modelToParentTransform->Identity();
    modelToParentTransform->SetElement(0, 3, m_PointTarget[0]);
    modelToParentTransform->SetElement(1, 3, m_PointTarget[1]);
    modelToParentTransform->SetElement(2, 3, m_PointTarget[2]);
    vtkNew<vtkTransform> handleToWorldMatrix;
    handleToWorldMatrix->PostMultiply();
    handleToWorldMatrix->Concatenate(modelToParentTransform);
    handleToWorldMatrix->Translate(-m_PointTarget[0], -m_PointTarget[1], -m_PointTarget[2]);
    handleToWorldMatrix->RotateWXYZ(angle, rotationVector_Local);
    handleToWorldMatrix->Translate(m_PointTarget);
    modelToParentTransform->DeepCopy(handleToWorldMatrix->GetMatrix());

   

    ControllerMapType::iterator cit;
    auto lm = qSlicerApplication::application()->layoutManager();
    auto sliceNames = lm->sliceViewNames();
    for (int i = 0; i < sliceNames.size(); i++) {
        vtkMRMLSliceNode* sliceNode = lm->sliceWidget(sliceNames.at(i))->mrmlSliceNode();
        vtkNew<vtkMatrix4x4> ijkToRAS;
        sliceNode->SetOrientationToDefault();
        vtkMatrix4x4* sliceToRAS = sliceNode->GetSliceToRAS();
        sliceToRAS->Multiply4x4(modelToParentTransform, sliceToRAS, sliceToRAS);
        sliceNode->GetSliceToRAS()->DeepCopy(sliceToRAS);
        sliceNode->UpdateMatrices();
    }
    vtkMRMLSliceNode::JumpAllSlices(mrmlScene(), dx, dy, dz, 1);
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    auto p1 = widget->m_FiberData->m_PointTarget;
    vtkMRMLSliceNode::JumpAllSlices(mrmlScene(), p1[0], p1[1], p1[2], 0);
}

NaviWidgetFiber* qSlicerNaviModuleWidget::GetSelectedFiberWidget() {
    Q_D(qSlicerNaviModuleWidget);
    QList<QListWidgetItem*> vec = d->listWidgetOpticalFiber->selectedItems();
    if (vec.size() > 0) {
        auto item = vec.at(0);
        auto widget = d->listWidgetOpticalFiber->itemWidget(item);
        NaviWidgetFiber* fiberWidget = dynamic_cast<NaviWidgetFiber*>(widget);
        return fiberWidget;
    }
    int n = d->listWidgetOpticalFiber->count();
    if (n > 0) {
        QListWidgetItem* item = d->listWidgetOpticalFiber->item(n-1);
        auto widget = d->listWidgetOpticalFiber->itemWidget(item);
        NaviWidgetFiber* fiberWidget = dynamic_cast<NaviWidgetFiber*>(widget);
        return fiberWidget;
    }
    return nullptr;
}

void qSlicerNaviModuleWidget::DoGetFiberWidgets() {
    Q_D(qSlicerNaviModuleWidget);
}

void qSlicerNaviModuleWidget::DoFreshFiberListView() {
    Q_D(qSlicerNaviModuleWidget);
    for (int i = 0; i < TestFiberWidgetVector.size(); i++) {
        if(TestFiberWidgetVector.at(i))
        delete TestFiberWidgetVector.at(i);       
    }
    d->listWidgetOpticalFiber->clear();
    TestFiberWidgetVector.clear();
    auto nodes = mrmlScene()->GetNodesByClass("vtkMRMLModelNode");
    int numberOfSceneViews = nodes->GetNumberOfItems();
    for (int n = 0; n < numberOfSceneViews; ++n)
    {
        vtkMRMLModelNode* modelNode =
            vtkMRMLModelNode::SafeDownCast(nodes->GetItemAsObject(n));
        auto model_type = modelNode->GetAttribute(NaviWidgetFiber::fiber_type.c_str());
        if (!model_type) {
            continue;
        }
        auto head_type = modelNode->GetAttribute(NaviWidgetFiber::stl_head.c_str());
        if (!head_type) {
            continue;
        }
        auto stl_head_node = modelNode->GetAttribute(NaviWidgetFiber::stl_head_node.c_str());
        auto model_name = modelNode->GetAttribute(NaviWidgetFiber::fiber_name.c_str());
        auto input_id = modelNode->GetAttribute(NaviWidgetFiber::input_node_id.c_str());
        auto target_id = modelNode->GetAttribute(NaviWidgetFiber::output_node_id.c_str());
        auto color_red = modelNode->GetAttribute(NaviWidgetFiber::color_red.c_str());
        auto color_green = modelNode->GetAttribute(NaviWidgetFiber::color_green.c_str());
        auto color_blue = modelNode->GetAttribute(NaviWidgetFiber::color_blue.c_str());
        auto fiber_visible = modelNode->GetAttribute(NaviWidgetFiber::fiber_visible.c_str());
        auto r_d_direction = modelNode->GetAttribute(NaviWidgetFiber::r_d_direction.c_str());
        auto r_d_distance = modelNode->GetAttribute(NaviWidgetFiber::r_d_distance.c_str());

        auto fiber_ablation_visible = modelNode->GetAttribute(NaviWidgetFiber::fiber_ablation_visible.c_str());
        auto fiber_save_depth = modelNode->GetAttribute(NaviWidgetFiber::fiber_save_depth.c_str());
        auto ablation_node_id = modelNode->GetAttribute(NaviWidgetFiber::ablation_node_id.c_str());

        auto red_node_id = modelNode->GetAttribute(NaviWidgetFiber::red_projection.c_str());
        auto green_node_id = modelNode->GetAttribute(NaviWidgetFiber::green_projection.c_str());
        auto yellow_node_id = modelNode->GetAttribute(NaviWidgetFiber::yellow_projection.c_str());
        
        auto protect_area_id = modelNode->GetAttribute(NaviWidgetFiber::protect_area_id.c_str());
        auto protect_area_visible = modelNode->GetAttribute(NaviWidgetFiber::protect_area_visible.c_str());
        auto protect_area_diameter = modelNode->GetAttribute(NaviWidgetFiber::protect_area_diameter.c_str());

        auto fiber_ptr = new FiberData();
        NaviWidgetFiber* itemWidget = new NaviWidgetFiber(fiber_ptr, true, d->listWidgetOpticalFiber, this->mrmlScene(), d->logic());
        TestFiberWidgetVector.append(itemWidget);
        itemWidget->SetDeleteable(false);
        itemWidget->ui.textEdit->setText(QString::fromLocal8Bit(model_name));
        QListWidgetItem* item = new QListWidgetItem(d->listWidgetOpticalFiber);
        d->listWidgetOpticalFiber->setItemWidget(item, itemWidget);
        d->listWidgetOpticalFiber->addItem(item);
        m_WidgetItem2Fiber[item] = itemWidget;
        itemWidget->ui.widgetSliderBkg->show();

        int width = d->listWidgetOpticalFiber->geometry().width();
        if (!itemWidget->m_FiberData->m_IsShuqian) {
            
            item->setSizeHint(QSize(width, 140));
        }
        else {
            item->setSizeHint(QSize(width, 489));
        }
        itemWidget->SetFixedFiberType(QString::fromLatin1(model_type));


        vtkMRMLMarkupsFiducialNode* input = vtkMRMLMarkupsFiducialNode::SafeDownCast(mrmlScene()->GetNodeByID(input_id));
        itemWidget->m_FiberData->m_InputFiducialNode = input;
        vtkMRMLMarkupsFiducialNode* target = vtkMRMLMarkupsFiducialNode::SafeDownCast(mrmlScene()->GetNodeByID(target_id));
        itemWidget->m_FiberData->m_TargetFiducialNode = target;
        itemWidget->m_FiberData->m_ModelNode = modelNode;
        itemWidget->m_FiberData->m_ModelNode->CreateDefaultDisplayNodes();
        itemWidget->DoUpdateFiberPosition();
        itemWidget->DoUpdateDistance();
        
        if (red_node_id != nullptr) {
            auto red_node = vtkMRMLMarkupsLineNode::SafeDownCast(mrmlScene()->GetNodeByID(red_node_id));
            itemWidget->m_FiberData->m_RedProjectionNode = red_node;
        }
        //std::cout << "red_node_id:" << red_node_id <<std::endl;
        if (green_node_id != nullptr) {
            auto green_node = vtkMRMLMarkupsLineNode::SafeDownCast(mrmlScene()->GetNodeByID(green_node_id));
            itemWidget->m_FiberData->m_GreenProjectionNode = green_node;
        }
        if (yellow_node_id != nullptr) {
            auto yellow_node = vtkMRMLMarkupsLineNode::SafeDownCast(mrmlScene()->GetNodeByID(yellow_node_id));
            itemWidget->m_FiberData->m_YellowProjectionNode = yellow_node;
        }

        if (stl_head_node != nullptr) {
            auto stl_Node2 = mrmlScene()->GetNodeByID(stl_head_node);
            itemWidget->m_FiberData->m_ModelNode2 = vtkMRMLModelNode::SafeDownCast(stl_Node2);
        }

        if (r_d_direction) {
            itemWidget->m_FiberData->m_RelativePositionDirection = QString::fromLocal8Bit(r_d_direction).toInt();
        }
        else {
            itemWidget->m_FiberData->m_RelativePositionDirection = 0;
        }

        if (r_d_distance) {
            itemWidget->m_FiberData->m_RelativePositionDistance = QString::fromLocal8Bit(r_d_distance).toInt();
        }
        else {
            itemWidget->m_FiberData->m_RelativePositionDistance = 0;
        }

        if(color_red){
            itemWidget->m_FiberData->m_red = QString::fromLocal8Bit(color_red).toInt();
        }
        else {
            itemWidget->m_FiberData->m_red = 230;
        }

        if (color_green) {
            itemWidget->m_FiberData->m_green = QString::fromLocal8Bit(color_green).toInt();
        }
        else {
            itemWidget->m_FiberData->m_green = 230;
        }

        if (color_blue) {
            itemWidget->m_FiberData->m_blue = QString::fromLocal8Bit(color_blue).toInt();
        }
        else {
            itemWidget->m_FiberData->m_blue = 77;
        }
        itemWidget->freshColor();

        QString q_fiber_visible = "true";
        if (fiber_visible != nullptr) {
            q_fiber_visible  = QString::fromLocal8Bit(fiber_visible);
        }
        
        QString q_fiber_ablation_visible = "true";
        if (fiber_ablation_visible != nullptr) {
            q_fiber_ablation_visible = QString::fromLocal8Bit(fiber_ablation_visible);
        }
        if (q_fiber_ablation_visible == "true" || q_fiber_ablation_visible == "1") {
            itemWidget->OnChangeAblactionVisible(true);
        }
        else {
            itemWidget->OnChangeAblactionVisible(false);
        }

        if (q_fiber_visible == "true" || q_fiber_visible == "1") {
            itemWidget->OnChangeVisible(true);
        }
        else {
            itemWidget->OnChangeVisible(false);
        }

        if (ablation_node_id) {
            auto ablation_node = mrmlScene()->GetNodeByID(ablation_node_id);
            itemWidget->m_FiberData->m_SegmentationNode = vtkMRMLSegmentationNode::SafeDownCast(ablation_node);
            itemWidget->DoUpdateAblationNumber();
        }

        if (protect_area_id) {
            itemWidget->m_FiberData->m_ProtectionNode = vtkMRMLSegmentationNode::SafeDownCast(mrmlScene()->GetNodeByID(protect_area_id));
           // auto segmentid = itemWidget->m_FiberData->m_ProtectionNode->GetSegmentation()->GetNthSegmentID(0);
           // itemWidget->m_FiberData->m_ProtectionNode->GetClosedSurfaceRepresentation(segmentid, itemWidget->m_FiberData->cyl);
        }
        if (protect_area_visible) {
            if (protect_area_visible == "true") {
                itemWidget->m_FiberData->m_ProtectionNodeVisibility = false;
                itemWidget->OnChangeProtectVisible();
            }
            else {
                itemWidget->m_FiberData->m_ProtectionNodeVisibility = false;
                itemWidget->OnChangeProtectVisible();
            }
        }
        if (protect_area_diameter) {
            double diameter = QString::fromLatin1(protect_area_diameter).toDouble();
            itemWidget->ui.horizontalSlider_4->setValue(diameter);
        }

        connect(itemWidget, SIGNAL(delete_fiber(NaviWidgetFiber*)), this, SLOT(OnDeleteFiberWidget(NaviWidgetFiber*)));
        //connect(itemWidget, SIGNAL(need_update()), this, SLOT(update_show()));
        //connect(itemWidget, SIGNAL(show_fiber(NaviWidgetFiber*, QString)), this, SLOT(on_show_fiber(NaviWidgetFiber*, QString)));
        //connect(itemWidget, SIGNAL(move_fiber(NaviWidgetFiber*)), this, SLOT(on_move_fiber(NaviWidgetFiber*)));

        connect(itemWidget, SIGNAL(clicked(NaviWidgetFiber*)), this, SLOT(UpdateFiberList(NaviWidgetFiber*)));
        connect(itemWidget, &NaviWidgetFiber::ablation_number_changed, this, &qSlicerNaviModuleWidget::update_ablation_number);
        connect(itemWidget, &NaviWidgetFiber::rotate_to_vector, this, &qSlicerNaviModuleWidget::on_rotate_to_vector);
        connect(itemWidget, &NaviWidgetFiber::fiber_depth, this, &qSlicerNaviModuleWidget::on_fiber_depth);
        connect(itemWidget, &NaviWidgetFiber::estimate, this, &qSlicerNaviModuleWidget::on_estimate);
        connect(itemWidget, &NaviWidgetFiber::center_target, this, &qSlicerNaviModuleWidget::DoCenterTargetPoint);
        connect(itemWidget, &NaviWidgetFiber::center_input, this, &qSlicerNaviModuleWidget::DoCenterInputPoint);

        //int dp = QString::fromLocal8Bit(fiber_save_depth).toInt();
        //itemWidget->ui.horizontalSlider->setValue(dp);
        
    }
    m_LastFiberItem = nullptr;
}

void qSlicerNaviModuleWidget::OnDeleteFiberWidget(NaviWidgetFiber* fiber) {
    Q_D(qSlicerNaviModuleWidget);
    QStringList tmpList = { QStringLiteral("确定"), QStringLiteral("取消") };
    MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("删除提示:"), QStringLiteral("是否删除？"), MessageButtonType::BUTTON_OK_AND_CANCEL, tmpList, true);
    if (res == MessageResult::CANCEL)
    {
        return;
    }
    int n = d->listWidgetOpticalFiber->count();
    for (int i = 0; i < n; i++) {
        QListWidgetItem* item = d->listWidgetOpticalFiber->item(i);
        QWidget* widget = d->listWidgetOpticalFiber->itemWidget(item);
        NaviWidgetFiber* fiberWidget = dynamic_cast<NaviWidgetFiber*>(widget);
        if (fiberWidget && fiberWidget == fiber) {
            fiberWidget->DeleteFromScene();
            d->listWidgetOpticalFiber->takeItem(i);
            for (int i = 0; i < TestFiberWidgetVector.size(); i++) {
                if(TestFiberWidgetVector.at(i) == fiberWidget){
                    TestFiberWidgetVector.takeAt(i);
                    break;
                }
            }
        }
    }

    for (QMap< QListWidgetItem*, NaviWidgetFiber* >::iterator it = m_WidgetItem2Fiber.begin(); it != m_WidgetItem2Fiber.end(); ++it)
    {
        if (it.value() == fiber)
        {
            m_WidgetItem2Fiber.erase(it);
            break;
        }
    }
    
    
}



void qSlicerNaviModuleWidget::exit() {
    qInfo() << "qSlicerNaviModuleWidget::exit";
    this->Superclass::exit();
    Q_D(qSlicerNaviModuleWidget);;
    m_Map2D.clear();
    d->listWidget2DMode->clear();
    if (d->tabWidgetModel->currentIndex() == 3) {
        onSegment3DCancel();
    }

    for (int i = 0; i < pyModulesNames.size(); i++) {
        qInfo() << pyModulesNames.at(i) << "->" << "is exiting";
        pyModuleContainers.at(i)->setModule(nullptr);
        pyModuleContainers.at(i)->show();
    }
    d->logic()->RemoveObserver(this->m_CallbackCommand);

    ViewWidgetVector3D.clear();
    d->listWidget3DMode->clear();
}


void qSlicerNaviModuleWidget::onModifySegment(NaviWidget3D* widget3d) {
    Q_D(qSlicerNaviModuleWidget);
    if (d->m_Temp3DWidget) {
        d->example_item_widget_layout->removeWidget(d->m_Temp3DWidget);
        delete d->m_Temp3DWidget;
    }
    d->m_Temp3DWidget = widget3d;
    d->m_Temp3DWidget->hideDeleteBtn();
    d->m_Temp3DWidget->hideModifyBtn();
    d->example_item_widget_layout->addWidget(d->m_Temp3DWidget);
 
    m_State = qSlicerNaviModuleWidget::WaitForAddSegment;

    //获取唯一的EditeorNode
    auto segmentEditorSingletonTag = "SegmentEditor";
    auto segmentEditorNode = mrmlScene()->GetSingletonNode(segmentEditorSingletonTag, "vtkMRMLSegmentEditorNode");
    if (!segmentEditorNode) {
        segmentEditorNode = mrmlScene()->AddNewNodeByClass("vtkMRMLSegmentEditorNode");
        segmentEditorNode->SetSingletonTag(segmentEditorSingletonTag);
    }


    vtkMRMLSubjectHierarchyNode* m_HierarchyNode = mrmlScene()->GetSubjectHierarchyNode();
    vtkSlicerSubjectHierarchyModuleLogic* logic = vtkSlicerSubjectHierarchyModuleLogic::SafeDownCast(appLogic()->GetModuleLogic("SubjectHierarchy"));
    auto t_id = m_HierarchyNode->GetItemByName(widget3d->m_Data->m_SegmentationNode->GetName());
    vtkIdType clonedItemID = vtkSlicerSubjectHierarchyModuleLogic::CloneSubjectHierarchyItem(m_HierarchyNode, t_id);
    auto cloned_node = vtkMRMLSegmentationNode::SafeDownCast(m_HierarchyNode->GetItemDataNode(clonedItemID));
    auto displayNode = cloned_node->GetDisplayNode();
    if(displayNode == nullptr){
        cloned_node->CreateDefaultDisplayNodes();
        displayNode = cloned_node->GetDisplayNode();
        
        displayNode->SetVisibility2D(false);
        displayNode->SetVisibility3D(false);
    }
    displayNode->SetVisibility(false);

    //初始化分割页面
    vtkMRMLSegmentEditorNode* editorNode = vtkMRMLSegmentEditorNode::SafeDownCast(segmentEditorNode);
    d->EffectsWidget->setMRMLSegmentEditorNode(editorNode);
    d->EffectsWidget->setSegmentationNode(widget3d->m_Data->m_SegmentationNode);
    d->EffectsWidget->setClonedNode(cloned_node);
    if (!widget3d->m_Data->m_MasterNode) {
        widget3d->m_Data->m_MasterNode = vtkMRMLScalarVolumeNode::SafeDownCast(mrmlScene()->GetFirstNodeByName("basic"));
    }
    d->EffectsWidget->SetMasterNode(widget3d->m_Data->m_MasterNode);
    d->EffectsWidget->selectFirstSegment();
    d->EffectsWidget->onCreateSurfaceToggled(true);
    d->EffectsWidget->type = widget3d->m_Data->structure_alias;
    d->EffectsWidget->updateEffectList();
    Reinit();
    d->tabWidgetModel->setCurrentIndex(3);;
}

void qSlicerNaviModuleWidget::onRemoveSegment(NaviWidget3D* widget3d) {
    Q_D(qSlicerNaviModuleWidget);
	QStringList tmpList = { QStringLiteral("确定"), QStringLiteral("取消") };
	MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("删除提示:"), QStringLiteral("是否删除？"), MessageButtonType::BUTTON_OK_AND_CANCEL, tmpList, true);
	if (res == MessageResult::CANCEL)
	{
		return;
	}
    //获取唯一的EditeorNode
    auto segmentEditorSingletonTag = "SegmentEditor";
    auto segmentEditorNode = mrmlScene()->GetSingletonNode(segmentEditorSingletonTag, "vtkMRMLSegmentEditorNode");
    if (!segmentEditorNode) {
        segmentEditorNode = mrmlScene()->AddNewNodeByClass("vtkMRMLSegmentEditorNode");
        segmentEditorNode->SetSingletonTag(segmentEditorSingletonTag);
    }

    //初始化分割页面
    vtkMRMLSegmentEditorNode* editorNode = vtkMRMLSegmentEditorNode::SafeDownCast(segmentEditorNode);
    vtkSegment* segment = widget3d->m_Data->segment;
    vtkMRMLSegmentationNode* segment_node = widget3d->m_Data->m_SegmentationNode;
    if (segment_node) {
        mrmlScene()->RemoveNode(segment_node);
    }

    for (int i = 0; i < ViewWidgetVector3D.size(); i++) {
        if (ViewWidgetVector3D.at(i)->segment == segment) {
            ViewWidgetVector3D.removeAt(i);
            break;
        }
    }
    
   
    DoFresh3DListView();
}



//新建光纤
void qSlicerNaviModuleWidget::onSegment3DCancel()
{
    Q_D(qSlicerNaviModuleWidget);
    d->logic()->DeleteSegment(d->m_Temp3DWidget->m_Data->segmentID);
    d->m_Temp3DWidget->m_Data->segment = nullptr;
    


    vtkMRMLNode* clonedNode = d->EffectsWidget->getClonedNode();
    if (clonedNode) {
        auto origin_name = d->m_Temp3DWidget->m_Data->m_SegmentationNode->GetName();
        if (d->m_Temp3DWidget->m_Data->m_SegmentationNode)
        {
            d->m_Temp3DWidget->m_Data->m_SegmentationNode->GetDisplayNode()->SetVisibility(false);
            SafeRemove(d->m_Temp3DWidget->m_Data->m_SegmentationNode);
        }
        d->m_Temp3DWidget->m_Data->m_SegmentationNode = vtkMRMLSegmentationNode::SafeDownCast(clonedNode);
        d->m_Temp3DWidget->m_Data->segmentDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(d->m_Temp3DWidget->m_Data->m_SegmentationNode->GetDisplayNode());
        if (!d->m_Temp3DWidget->m_Data->segmentDisplayNode) {
            d->m_Temp3DWidget->m_Data->m_SegmentationNode->CreateDefaultDisplayNodes();
            d->m_Temp3DWidget->m_Data->segmentDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(d->m_Temp3DWidget->m_Data->m_SegmentationNode->GetDisplayNode());

        }
        d->m_Temp3DWidget->m_Data->m_SegmentationNode->SetName(origin_name);
        d->m_Temp3DWidget->m_Data->segmentDisplayNode->SetVisibility(true);
    }
    else {
        SafeRemove(d->m_Temp3DWidget->m_Data->m_SegmentationNode);
    }
    
    m_State = qSlicerNaviModuleWidget::None;
    d->tabWidgetModel->setCurrentIndex(0);
    DoInitTemp3DWidget();
    d->EffectsWidget->setActiveEffect(nullptr);
    d->EffectsWidget->uninstallKeyboardShortcuts();
    d->btn2D->animateClick(10);
    DoFresh3DListView();
}

//确认分割
void qSlicerNaviModuleWidget::onSegment3DConfirm()
{
    Q_D(qSlicerNaviModuleWidget);
    
    
    
    vtkMRMLNode* clonedNode = d->EffectsWidget->getClonedNode();
    if (clonedNode) {
        d->EffectsWidget->setClonedNode(nullptr);
        SafeRemove(clonedNode);
    }
    else {
        ViewWidgetVector3D.push_back(d->m_Temp3DWidget->m_Data);
    }

    m_State = qSlicerNaviModuleWidget::None;
    d->tabWidgetModel->setCurrentIndex(0);
    DoInitTemp3DWidget();
    d->EffectsWidget->setActiveEffect(nullptr);
    d->EffectsWidget->uninstallKeyboardShortcuts();
    d->btn2D->animateClick(10);
    DoFresh3DListView();
}

void qSlicerNaviModuleWidget::onActiveMarkupMRMLNodeAdded(vtkMRMLNode* markupsNode)
{
    Q_D(qSlicerNaviModuleWidget);
    if (d->logic())
    {
        d->logic()->AddNewDisplayNodeForMarkupsNode(markupsNode);
        d->logic()->SetActiveListID(markupsNode);
        d->logic()->SetPlaceMode();
    }
    
}


bool qSlicerNaviModuleWidget::AutoTestBreak(int i) {
    Q_D(qSlicerNaviModuleWidget);

	auto xml_root = qSlicerApplication::application()->layoutManager()->m_XMLRoot;
	if (!xml_root)
	{
		return false;
	}
	auto xml_navi = xml_root->GetChild("navi");
	if (!xml_navi) {
		return false;
	}
    if (i == 0) {
		int delay = 0;
		itk::DOMNode::ChildrenListType groupList;
        xml_navi->GetChildren("group", groupList);
		for (auto& g : groupList)
		{
			//QApplication::processEvents();
			itk::DOMNode::ChildrenListType clickList;
			g->GetChildren("click", clickList);
			for (auto& c : clickList)
			{
				std::string s_delay = GetXMLText(c, "delay");
				std::string screen_x = GetXMLText(c, "x");
				std::string screen_y = GetXMLText(c, "y");
				std::string s_testid = GetXMLText(c, "testId");
				std::string screenshot = GetXMLText(c, "savescreen");
                std::string s_fibername = GetXMLText(c, "fibername");

				delay += QString::fromStdString(s_delay).toInt();

				if (screen_x != "" && screen_y != "") {
					double x = QString::fromStdString(screen_x).toDouble();
					double y = QString::fromStdString(screen_y).toDouble();
					auto pressEvent = new QMouseEvent(QEvent::MouseButtonPress, QPoint(x, y), QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
					auto releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(x, y), QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

					QTimer::singleShot(delay, [=]() {
						//QApplication::sendEvent(w2, pressEvent);
						//QApplication::sendEvent(w2, releaseEvent);
						if (screen_x == "1680" && (screen_y == "785" || screen_y == "400"))
						{
							::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);

							::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
							::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);

							::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
							::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);


						}
                        else if ((screen_x == "1719" && screen_y == "452") || (screen_x == "1555" && screen_y == "548"))
                        {
							::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);

							::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
                            ::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, (x + 90) / 1920 * 65535, y / 1080 * 65535, 0, 0);
							::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, (x+90) / 1920 * 65535, y / 1080 * 65535, 0, 0);
                        }
						else
						{
							::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
							::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
							::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
						}
						}
					);
				}
				if (!s_fibername.empty())
				{
					QTimer::singleShot(delay+500, [=]() {
						int i = d->listWidgetOpticalFiber->currentRow();
						NaviWidgetFiber* item = dynamic_cast<NaviWidgetFiber*>(d->listWidgetOpticalFiber->item(i));
						item->SetFiberName(QString::fromStdString(s_fibername));
						});
				}
				if (screenshot == "ss")
				{
					QTimer::singleShot(delay + 1000, [=]() {
						qSlicerApplication::SaveScreenShot(QString::fromStdString(s_testid));
						});
				}
			}//for

		}//for
		auto exit = xml_navi->GetChild("exit");
		if (exit)
		{
			auto exitmark = exit->GetTextChild()->GetText();
			if (exitmark == "0") {
				QTimer::singleShot(delay + 5000, [=]() {
					//AddSignalNode("exit_with_save", "4");
					});
				qSlicerApplication::application()->layoutManager()->m_XMLRoot = nullptr;

			}
		}
    }
    if (i == 1) {
        
    }
    if (i == 2) {
        
    }
    return false;
}