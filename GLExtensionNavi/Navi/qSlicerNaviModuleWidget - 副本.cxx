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
    std::string organIconPath = "E:/S/Resources/organIcon/";
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
    int brain_segment = settings->value("additional_plugins/brain_segment").toInt();
    if (brain_segment == 1) {
        d_ptr->pushButton->setEnabled(true);
        connect(d_ptr->pushButton, SIGNAL(clicked()), this, SLOT(OnBrainParcellation()));
    }
    else {
        d_ptr->pushButton->setEnabled(false);
    }
    
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
    connect(d_ptr->m_Auto1_1, SIGNAL(clicked()), this, SLOT(onEnterInit()()));
    connect(d_ptr->m_Auto1_2, SIGNAL(clicked()), this, SLOT(onTest2()));
    connect(d_ptr->m_Auto1_3, SIGNAL(clicked()), this, SLOT(onTest3()));
    connect(d_ptr->m_Auto1_4, SIGNAL(clicked()), this, SLOT(onTest4()));
    connect(d_ptr->m_Auto1_5, SIGNAL(clicked()), this, SLOT(onTest5()));
    connect(d_ptr->m_Auto1_6, SIGNAL(clicked()), this, SLOT(onTest6()));
    connect(d_ptr->m_Auto1_7, SIGNAL(clicked()), this, SLOT(onTest7()));
    connect(d_ptr->m_Auto1_8, SIGNAL(clicked()), this, SLOT(onTest8()));
    connect(d_ptr->m_Auto1_9, SIGNAL(clicked()), this, SLOT(onTest8()));
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
    connect(d_ptr->btn_Areaback, SIGNAL(clicked()), this, SLOT(onbacktab0()));
    connect(d_ptr->btn_Pathback, SIGNAL(clicked()), this, SLOT(onbacktab0()));
    connect(d_ptr->btn_Pathback_2, SIGNAL(clicked()), this, SLOT(onbacktab0()));
    //d_ptr->pushButton_18->setEnabled(false);

   
    d->listWidget2DMode->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    d->listWidget2DMode->setEditTriggers(QAbstractItemView::DoubleClicked);
    

    d_ptr->widget_2->hide();
    
	d_ptr->tabWidgetModel->tabBar()->hide();
	d_ptr->tabWidgetAdjust->tabBar()->hide();
	d_ptr->tabWidget_2->tabBar()->hide();
	d_ptr->tabWidgetTools->tabBar()->hide();
    d->tabWidget_3->setCurrentIndex(0);
    d->tabWidget_3->tabBar()->hide();
    connect(d_ptr->btnBack2View, SIGNAL(clicked()), this, SLOT(onReturnLastPage()));
    connect(d_ptr->btnBack2Struct, SIGNAL(clicked()), this, SLOT(onReturnLastPage()));
    connect(d_ptr->btnBack2Sequence, SIGNAL(clicked()), this, SLOT(onReturnLastPage()));
    connect(d_ptr->pushButton_28, SIGNAL(clicked()), this, SLOT(DoCenterTargetPoint()));
    connect(d_ptr->btnWave, &QPushButton::clicked, this, &qSlicerNaviModuleWidget::OnWaveChangeClick);
    /*m_CallbackCommand->SetClientData(reinterpret_cast<void*>(this));
    m_CallbackCommand->SetCallback(qSlicerNaviModuleWidget::processEvents);
    d->logic()->AddObserver(9990, this->m_CallbackCommand);*/
  
	d->listWidgetOpticalFiber->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	d->listWidgetOpticalFiber->verticalScrollBar()->setSingleStep(5);
    //connect(d_ptr->listWidgetOpticalFiber, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(on_click_fibe_widget(QListWidgetItem*)));
    


    QString moduleName = "GLPyDTI";
    auto mm = qSlicerApplication::application()->moduleManager();

    auto md = mm->module(moduleName);
    d->DTIPanel->setModule(md);
    d->DTIPanel->show();

    auto md2 = mm->module("HDBrainExtractionTool");
    d->PyPanel->setModule(md2);
    d->PyPanel->show();

    auto vessel_segment_module = mm->module("VesselSegment");
    d->VesselPanel->setModule(vessel_segment_module);
    d->VesselPanel->show();

    auto brain_segment_module = mm->module("BrainParcellation");
    d->BrainSegmentPanel->setModule(brain_segment_module);
    d->BrainSegmentPanel->show();

    auto vessel_module = mm->module("VesselSegment");
    d->VesselPanel->setModule(vessel_module);
    d->VesselPanel->show();

    auto ablation_estimate_module = mm->module("AblationEstimate");
    d->AblationEstimatePanel->setModule(ablation_estimate_module);
    d->AblationEstimatePanel->show();

    qvtkReconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));

    d->btn2D->animateClick(10);
}   


void qSlicerNaviModuleWidget::onAblationEstimate() {
    Q_D(qSlicerNaviModuleWidget);
    auto mm = qSlicerApplication::application()->moduleManager();
    auto ablation_estimate_module = mm->module("AblationEstimate");
    d->AblationEstimatePanel->setModule(nullptr);
    d->AblationEstimatePanel->setModule(ablation_estimate_module);
    d->AblationEstimatePanel->show();
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

    if (help_node && strcmp(help_node, "true") == 0) {

        if (type && value) {
            if (strcmp(type, "show_stripped_brain") == 0) {
                mrmlScene()->RemoveNode(node);
                DoFresh2DListView();
                DoFresh3DListView();
            }
            if (strcmp(type, "return_to_panel") == 0) {
                mrmlScene()->RemoveNode(node);
                d->tabWidget_3->setCurrentIndex(0);
            }
            if (strcmp(type, "show_dti") == 0) {
                mrmlScene()->RemoveNode(node);
                auto mm = qSlicerApplication::application()->moduleManager();
                auto md2 = mm->module("TractographyDisplay");
                d->DTIDisplayPanel->setModule(md2);
                d->DTIDisplayPanel->show();
            }
            if (strcmp(type, "brain_segment_complete") == 0) {
                mrmlScene()->RemoveNode(node);
                auto mm = qSlicerApplication::application()->moduleManager();
                auto brain_segment_display_module = mm->module("BrainSegmentDisplay");
                d->BrainSegmentDisplayPanel->setModule(nullptr);
                d->BrainSegmentDisplayPanel->setModule(brain_segment_display_module);
                d->BrainSegmentDisplayPanel->show();
            }
        }
    }
}


void qSlicerNaviModuleWidget::onLeksellRecognize() {
    qSlicerApplication::application()->layoutManager()->setCurrentPage(11);
}


void qSlicerNaviModuleWidget::onHDBrainExtract() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(4);
}

void qSlicerNaviModuleWidget::processEvents(vtkObject* caller, unsigned long eid, void* clientData, void* callData) {
    qSlicerNaviModuleWidget* self = reinterpret_cast<qSlicerNaviModuleWidget*>(clientData);
    if (eid == 9990) {
        vtkMatrix4x4* modelToParentTransform = reinterpret_cast<vtkMatrix4x4*>(callData);
        ControllerMapType::iterator cit;
        for (cit = self->ControllerMap.begin(); cit != self->ControllerMap.end(); ++cit)
        {
            qMRMLSliceControllerWidget* controll = dynamic_cast<qMRMLSliceControllerWidget*>(cit->second);
            if (controll) {
                vtkMRMLSliceNode* sliceNode = controll->mrmlSliceNode();
                vtkNew<vtkMatrix4x4> ijkToRAS;
                sliceNode->SetOrientationToDefault();

                vtkMatrix4x4* sliceToRAS = sliceNode->GetSliceToRAS();

                sliceToRAS->Multiply4x4(modelToParentTransform, sliceToRAS, sliceToRAS);
                sliceNode->GetSliceToRAS()->DeepCopy(sliceToRAS);
                sliceNode->UpdateMatrices();
            }
        }
        self->Reinit();
        
    }
}

void qSlicerNaviModuleWidget::onbacktab0()
{
    Q_D(qSlicerNaviModuleWidget);

    d->tabWidget_3->setCurrentIndex(0);
}



void qSlicerNaviModuleWidget::onAblationArea(){
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(1);
}

void qSlicerNaviModuleWidget::onPathAdjustment() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(2);
}

void qSlicerNaviModuleWidget::DoCenterInputPoint() {
    onFiberDirection();
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    Reinit();
    auto p1 = widget->m_FiberData->m_PointInput;
    vtkMRMLSliceNode::JumpAllSlices(mrmlScene(), p1[0], p1[1], p1[2], 0);
}

void qSlicerNaviModuleWidget::DoCenterTargetPoint() {
    onFiberDirection();
    auto widget = GetSelectedFiberWidget();
    if (!widget) {
        return;
    }
    Reinit();
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
            widget2D->SetWidget2DUnvisible();
        }
    }
}

void qSlicerNaviModuleWidget::onoperation() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_3->setCurrentIndex(3);
}

void qSlicerNaviModuleWidget::onReturnLastPage()
{
    Q_D(qSlicerNaviModuleWidget);
    int currentPage = d->tabWidgetModel->currentIndex();
    d->tabWidgetModel->setCurrentIndex(currentPage-1);
    cout << "btn_3D" << endl;
}

void qSlicerNaviModuleWidget::onStateChanged2D(int state) {}
void qSlicerNaviModuleWidget::onStateChanged(int state) {
    Q_D(qSlicerNaviModuleWidget);
    d->EffectsWidget->onCreateSurfaceToggled(true);
}


void qSlicerNaviModuleWidget::onFusionRatioChanged(int value) {
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
        d->EffectsWidget->updateEffectList();
        Reinit();


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
    d->tabWidget_2->setCurrentIndex(0);
    connect(d->btnAllUnvisible, SIGNAL(clicked()), this, SLOT(onAllUnvisible()));
    DoFresh2DListView();
    DoFresh3DListView();
}

void qSlicerNaviModuleWidget::SetCurrent3DDisplay() {
    Q_D(qSlicerNaviModuleWidget);
    d->tabWidget_2->setCurrentIndex(1);
    cout << "btn_3D" << endl;
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

void qSlicerNaviModuleWidget::onChooseItem(QListWidgetItem* item) {
    Q_D(qSlicerNaviModuleWidget);
    NaviWidget2D* widget1 = dynamic_cast<NaviWidget2D*>(d->listWidget2DMode->itemWidget(item));
    if (!widget1) {
        return;
    }
    if (widget1->m_Visibility == false) {
        return;
    }
    DoSelectVolumeNode(widget1->m_Node);
    Reinit();

}

void qSlicerNaviModuleWidget::onGotoNextPage() {
    qSlicerApplication::application()->layoutManager()->setCurrentPage(qSlicerApplication::application()->layoutManager()->m_Page + 1);
}

/**
    根据mrmlScene中的volumeNode的配准情况来刷新2D中的列表
*/
void qSlicerNaviModuleWidget::DoFresh2DListView() {
    Q_D(qSlicerNaviModuleWidget);
    d->listWidget2DMode->clear();
    m_LastVolListWidget = NULL;
    std::vector<vtkMRMLNode*> dwiNodes;

    std::vector<vtkMRMLNode*> procNodes;
    int numProcNodes = qSlicerApplication::application()->mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", procNodes);
    for (int i = 0; i < numProcNodes; i++)
    {
        vtkMRMLScalarVolumeNode* procNode = vtkMRMLScalarVolumeNode::SafeDownCast(procNodes[i]);
        vtkMRMLDiffusionWeightedVolumeNode* dwiNode = vtkMRMLDiffusionWeightedVolumeNode::SafeDownCast(procNodes[i]);
        char* name = procNode->GetName();
        auto tName = QString::fromStdString(std::string(name));
        const char* is_registed_node = procNode->GetAttribute("is_registed");
        const char* alias_name = procNode->GetAttribute("alias_name");
        if (is_registed_node && std::string(is_registed_node) == "true") {

        }
        else {
            if (alias_name && std::string(alias_name) == "basic") {
                DoSelectVolumeNode(procNode);
                Reinit();
            }
            else {
                if (dwiNode) {
                
                }
                else {
                    continue;
                }
                
            }
        }
        
        

        if(dwiNode){
            NaviDWI2D* dwiWidget = new NaviDWI2D(d->listWidget2DMode);
            dwiWidget->SetNode(mrmlScene(), m_PresetScene,procNode, GetVolumeRenderingDisplayNode(procNode));
            QListWidgetItem* item = new QListWidgetItem(d->listWidget2DMode);
            d->listWidget2DMode->setItemWidget(item, dwiWidget);
            d->listWidget2DMode->addItem(item);
            int width = d->listWidget2DMode->geometry().width();
            item->setSizeHint(QSize(width, 162));
            d->listWidget2DMode->setItemSelected(item, true);
            connect(dwiWidget, SIGNAL(onRongjiChanged()), this, SLOT(onItemRongjiChanged()));
            connect(dwiWidget, SIGNAL(onOpacityChanged(int)), this, SLOT(onItemOpacityChanged(int)));
            Vector2D.push_back(dwiWidget);
        }
        else
        {
            NaviWidget2D* itemWidget = new NaviWidget2D(d->listWidget2DMode);
            vtkSlicerVolumeRenderingLogic* vrlLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(appLogic()->GetModuleLogic("VolumeRendering"));
            itemWidget->vrlLogic = vrlLogic;
            itemWidget->SetNode(mrmlScene(), m_PresetScene, procNode, GetVolumeRenderingDisplayNode(procNode));
            auto visible_2d = procNode->GetAttribute("visible_2d");
            auto visible_3d = procNode->GetAttribute("visible_3d");
            auto visible_roi = procNode->GetAttribute("visible_roi");
            auto color_node_id = procNode->GetAttribute("color_node_id");
            auto preset_shift = procNode->GetAttribute("preset_shift");
            auto node_roi = procNode->GetAttribute("node_roi");
            QListWidgetItem* item = new QListWidgetItem(d->listWidget2DMode);

            

            
            d->listWidget2DMode->setItemWidget(item, itemWidget);
            d->listWidget2DMode->addItem(item);
            int width = d->listWidget2DMode->geometry().width();
            item->setSizeHint(QSize(width, 162));
            d->listWidget2DMode->setItemSelected(item, true);
            connect(itemWidget, SIGNAL(onRongjiChanged()), this, SLOT(onItemRongjiChanged()));
            connect(itemWidget, SIGNAL(onOpacityChanged(int)), this, SLOT(onItemOpacityChanged(int)));
            connect(itemWidget, SIGNAL(visible_changed(NaviWidget2D*)), this, SLOT(OnVisibleChagned(NaviWidget2D*)));
            connect(itemWidget, SIGNAL(displayHiddenPanel(NaviWidget2D*)), this, SLOT(UpdateListWidget(NaviWidget2D*)));
            Vector2D.push_back(itemWidget);

            if (color_node_id) {
                auto color_node = mrmlScene()->GetNodeByID(color_node_id);
                itemWidget->DoSetColorTableNode(color_node);
            }
            else {
                auto grayNode = mrmlScene()->GetFirstNodeByName("Icon");
                itemWidget->DoSetColorTableNode(grayNode);
            }

            if (node_roi&& visible_roi) {
                if (strcmp(visible_roi,"true") == 0 || strcmp(visible_roi, "1") == 0) {
                    itemWidget->ui.checkBox_3->setChecked(true);
                }
                else {
                    itemWidget->ui.checkBox_3->setChecked(false);
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
                if (strcmp(visible_2d, "true")==0 || strcmp(visible_2d, "1") == 0) {
                    itemWidget->ui.checkBox->setChecked(true);
                }
                else {
                    itemWidget->ui.checkBox->setChecked(false);
                }
            }
        }
    }
    return;
}

void qSlicerNaviModuleWidget::OnVisibleChagned(NaviWidget2D* NW2D) {
    if (NW2D->m_Visibility) {
        if(NW2D->m_Node)
        DoSelectVolumeNode(NW2D->m_Node);
    }
    else {
        DoSelectVolumeNode(nullptr);
    }
    Reinit();
}

void qSlicerNaviModuleWidget::UpdateListWidget(NaviWidget2D* NW2D)
{
    Q_D(qSlicerNaviModuleWidget);
	isHiddenVolume = NW2D->hiddenPanelVisible;
    
	int index = -1;
    for (int i = 0; i < Vector2D.size(); ++i)
    {
        if (Vector2D[i] == NW2D)
        {
            index = i;
            break;
        }
    }

	if (index == -1) return;
	int width = d->listWidget2DMode->geometry().width();
	QListWidgetItem* item = d->listWidget2DMode->item(index);
	if (m_LastVolListWidget)
	{
		QWidget* widget = d->listWidget2DMode->itemWidget(m_LastVolListWidget);
		if (widget == NULL) return;
		NaviWidget2D* modelWidget = dynamic_cast<NaviWidget2D*>(widget);
		if (modelWidget) modelWidget->SetDropdownBtnStatus(false, false);
		m_LastVolListWidget->setSizeHint(QSize(width, 160));
	}
	int height = 225;
	if (!isHiddenVolume) height = 160;
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
        auto volumes = GetSegmentStastics(t_node);
        NaviWidget3D* itemWidget = new NaviWidget3D();
        Widget3DData* widget3dData = new Widget3DData();
        itemWidget->SetData(widget3dData);
        QListWidgetItem* item = new QListWidgetItem(d->listWidget3DMode);
        d->listWidget3DMode->setItemWidget(item, itemWidget);
        d->listWidget3DMode->addItem(item);

        NaviWidget3D* itemWidget2 = new NaviWidget3D();
        itemWidget2->SetData(widget3dData);
        QListWidgetItem* item2 = new QListWidgetItem(d->listWidget2DMode);
        d->listWidget2DMode->setItemWidget(item2, itemWidget2);
        d->listWidget2DMode->addItem(item2);

        int width = d->listWidget3DMode->geometry().width();
        item->setSizeHint(QSize(width, 110));
        item2->setSizeHint(QSize(width, 110));

        //connect(itemWidget, SIGNAL(need_update()), this, SLOT(update_show()));
        connect(itemWidget, SIGNAL(removeWidget3D(NaviWidget3D*)), this, SLOT(onRemoveSegment(NaviWidget3D*)));
        //connect(itemWidget, SIGNAL(clicked(NaviWidget3D*)), this, SLOT(on_click_widget(NaviWidget3D*)));
        //connect(itemWidget, SIGNAL(updateOpacityAndColor()), this, SLOT(Update3DOpacityAndColor()));
        //connect(itemWidget2, SIGNAL(need_update()), this, SLOT(update_show()));
        connect(itemWidget2, SIGNAL(removeWidget3D(NaviWidget3D*)), this, SLOT(onRemoveSegment(NaviWidget3D*)));
        //connect(itemWidget2, SIGNAL(clicked(NaviWidget3D*)), this, SLOT(on_click_widget(NaviWidget3D*)));
        //connect(itemWidget2, SIGNAL(updateOpacityAndColor()), this, SLOT(Update3DOpacityAndColor()));
        widget3dData->segmentDisplayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(t_node->GetDisplayNode());
        widget3dData->m_SegmentationNode = t_node;
        widget3dData->segment = t_node->GetSegmentation()->GetNthSegment(0);
        widget3dData->segmentID = "0";
        const char* special_type = t_node->GetAttribute("special_type");

        //for HDBrainSegmentations Only
        if (special_type && strcmp(special_type, "stripped_brain_segment") == 0) {
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

        if (color_red) {
            widget3dData->m_red = QString::fromLocal8Bit(color_red).toInt();
            widget3dData->m_green = QString::fromLocal8Bit(color_green).toInt();
            widget3dData->m_blue = QString::fromLocal8Bit(color_blue).toInt();
            itemWidget->freshColor();
            itemWidget2->freshColor();
        }

        if (visible_2d) {
            if (QString::fromLocal8Bit(visible_2d) == "true") {
                itemWidget->ui.btnVisible->setChecked(true);
                itemWidget2->ui.btnVisible->setChecked(true);
            }
            else {
                itemWidget->ui.btnVisible->setChecked(false);
                itemWidget2->ui.btnVisible->setChecked(false);
            }
        }
        itemWidget->ui.lblName_2->setText(volumes);
        itemWidget2->ui.lblName_2->setText(volumes);

        if (visible_3d) {
            if (QString::fromLocal8Bit(visible_3d) == "true") {
                itemWidget->ui.btn3D->setChecked(true);
                itemWidget2->ui.btn3D->setChecked(true);
            }
            else {
                itemWidget->ui.btn3D->setChecked(false);
                itemWidget2->ui.btn3D->setChecked(false);
            }
        }

        if (opacity_all) {
            int opa = QString::fromLocal8Bit(opacity_all).toInt();
            itemWidget->ui.horizontalSlider->setValue(opa);
            itemWidget2->ui.horizontalSlider->setValue(opa);
        }

        itemWidget->SetTitle(QString::fromLocal8Bit(structure_name));
        itemWidget2->SetTitle(QString::fromLocal8Bit(structure_name));
    }
    return;

    int i = 0;
    for (QVector<Widget3DData*>::iterator objectIter = ViewWidgetVector3D.begin();
        objectIter != ViewWidgetVector3D.end();
        ++objectIter)
    {
        i += 1;
        Widget3DData* widget3dData = (*objectIter);
        NaviWidget3D* itemWidget = new NaviWidget3D();
        itemWidget->SetData(widget3dData);
        QListWidgetItem* item = new QListWidgetItem(d->listWidget3DMode);
        d->listWidget3DMode->setItemWidget(item, itemWidget);
        d->listWidget3DMode->addItem(item);
        int width = d->listWidget3DMode->geometry().width();
        item->setSizeHint(QSize(width, 110));
        //connect(itemWidget, SIGNAL(need_update()), this, SLOT(update_show()));
        connect(itemWidget,SIGNAL(removeWidget3D(NaviWidget3D*)),this,SLOT(onRemoveSegment(NaviWidget3D * )));
        //connect(itemWidget, SIGNAL(clicked(NaviWidget3D*)), this, SLOT(on_click_widget(NaviWidget3D*)));
        //connect(itemWidget, SIGNAL(updateOpacityAndColor()), this, SLOT(Update3DOpacityAndColor()));
    }

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
   

    auto fc = GetGlobalSaveValue("navi_clear");
    if (fc && strcmp(fc, "true") == 0) {
        //d->logic()->clear();
        d->listWidgetOpticalFiber->clear();
        TestFiberWidgetVector.clear();

        m_Rongji1 = nullptr;
        m_Rongji0 = nullptr;
        m_PresetScene = nullptr;
        m_State = qSlicerNaviModuleWidget::None;
        
        
        SetGlobalSaveValue("navi_clear", "false");
    }
    DoInitTemp3DWidget();
    if (qSlicerApplication::application()->layoutManager()->m_StartPage == 4) {
        doTestAddData();
    }

    d->EffectsWidget->setMRMLScene(mrmlScene());
    d->EffectsWidget->updateEffectList();

    DoFresh2DListView();
    DoFreshPhaseListView();
    DoFresh3DListView();
    DoFreshStructureListView();
    DoFreshFiberListView();
    AddSignalNode("layout_4", QStringList());

    vtkSlicerVolumeRenderingLogic* vrlLogic = vtkSlicerVolumeRenderingLogic::SafeDownCast(appLogic()->GetModuleLogic("VolumeRendering"));
    m_PresetScene = vrlLogic->GetPresetsScene();
    
    d->tabWidget->setCurrentIndex(0);
    d->tabWidgetModel->setCurrentIndex(0);
    d->btn2D->animateClick(10);
    //ATB(2);
    ATB(0); //@SXM
}

void qSlicerNaviModuleWidget::onTest2() {
    Q_D(qSlicerNaviModuleWidget);
    
}

void qSlicerNaviModuleWidget::onTest3() {
    Q_D(qSlicerNaviModuleWidget);
    d->listWidgetStructure->setCurrentRow(0);
    m_State = qSlicerNaviModuleWidget::WaitForAddSegment;
}
void qSlicerNaviModuleWidget::onTest4() {
    Q_D(qSlicerNaviModuleWidget);
    
}
void qSlicerNaviModuleWidget::onTest5() {}
void qSlicerNaviModuleWidget::onTest6() {}
void qSlicerNaviModuleWidget::onTest7() {}
void qSlicerNaviModuleWidget::onTest8() {}
void qSlicerNaviModuleWidget::onTest9() {}

void qSlicerNaviModuleWidget::onGotoPreviewPage() {
    qSlicerApplication::application()->layoutManager()->setCurrentPage(qSlicerApplication::application()->layoutManager()->m_Page - 1);
}

void qSlicerNaviModuleWidget::onItemRongjiChanged() {
    std::vector< NaviBase2D*> tmpVector;
    for (int i = 0; i < Vector2D.size(); i++) {
        if (Vector2D.at(i)->m_Rongji == 1) {
            tmpVector.push_back(Vector2D.at(i));
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
   
    SetGlobalSaveValue("navi_set", "1");
    QTimer::singleShot(1000, [this]() {onEnterInit(); });
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


    
    cout << "Enter Reigster Widget" << endl;

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
        return;
    }
    if (i == 1) {
        ControllerMapType::iterator cit;
        for (cit = ControllerMap.begin(); cit != ControllerMap.end(); ++cit)
        {
            qMRMLSliceControllerWidget* controll = dynamic_cast<qMRMLSliceControllerWidget*>(cit->second);
            if (controll) {
                vtkMRMLSliceNode* sliceNode = controll->mrmlSliceNode();
                vtkNew<vtkMatrix4x4> ijkToRAS;
                sliceNode->SetOrientationToDefault();
                sliceNode->UpdateMatrices();
            }
        }
        Reinit();
    }
    if (i == 2) {
        widget->OnFiberViewDirection();
    }
    if (i == 3) {
    
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
    ControllerMapType::iterator cit;
    for (cit = ControllerMap.begin(); cit != ControllerMap.end(); ++cit)
    {
        qMRMLSliceControllerWidget* controll = dynamic_cast<qMRMLSliceControllerWidget*>(cit->second);
        if (controll) {
            vtkMRMLSliceNode* sliceNode = controll->mrmlSliceNode();
            sliceNode->SetOrientationToDefault();
        }
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
        item->setSizeHint(QSize(width, 412));
    }


    connect(itemWidget, SIGNAL(delete_fiber(NaviWidgetFiber*)), this, SLOT(OnDeleteFiberWidget(NaviWidgetFiber*)));
    //connect(itemWidget, SIGNAL(need_update()), this, SLOT(update_show()));
    connect(itemWidget, SIGNAL(show_fiber(NaviWidgetFiber*, QString)), this, SLOT(on_show_fiber(NaviWidgetFiber*, QString)));
    connect(itemWidget, SIGNAL(move_fiber(NaviWidgetFiber*)), this, SLOT(on_move_fiber(NaviWidgetFiber*)));
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
    m_WidgetItem2Fiber[m_LastFiberItem] = itemWidget;
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
    if (listitem==m_LastFiberItem)
    {
        return;
    }
	int width = d->listWidgetOpticalFiber->geometry().width();
    listitem->setSizeHint(QSize(width, 412));
    if (m_LastFiberItem)
    {
        m_LastFiberItem->setSizeHint(QSize(width, 140));
    }
    m_LastFiberItem = listitem;
}

int now_click, last_click = 0;
void qSlicerNaviModuleWidget::on_click_fiber_widget(NaviWidgetFiber* widget) {
    Q_D(qSlicerNaviModuleWidget);

    int now_click = d->listWidgetOpticalFiber->currentRow();
    
       
    if (now_click == -1)
        now_click = 0;

    if (now_click != d->listWidgetOpticalFiber->count() - 1) {
        QListWidgetItem* nowitem0 = d->listWidgetOpticalFiber->item(d->listWidgetOpticalFiber->count() - 1);
        nowitem0->setSizeHint(QSize(540, 140));
    }

    QListWidgetItem* nowitem = d->listWidgetOpticalFiber->item(now_click);
    nowitem->setSizeHint(QSize(540, 414));


    if (now_click != last_click) {
        QListWidgetItem* lastitem = d->listWidgetOpticalFiber->item(last_click);
        if(lastitem){
        lastitem->setSizeHint(QSize(540, 140));
        last_click = now_click;
        }
    }
    widget->OnFiberViewDirection();
    auto sliceController = qSlicerApplication::application()->layoutManager()->sliceWidget("Red")->sliceController();
    auto slider = sliceController->sliceOffsetSlider();
    auto max = slider->maximum();
    auto min = slider->minimum();
    auto value = slider->value();
    auto index = (value - min) * 400 / (max - min);
    widget->SetSliderValue(index);

    update_ablation_number();
    onUpdateAblationAreaInfo(0);
}


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

    vtkMRMLNode* nodeSource = mrmlScene()->GetFirstNodeByName("abc");
    vtkMRMLScalarVolumeNode* volume = vtkMRMLScalarVolumeNode::SafeDownCast(nodeSource);
    DoClearVolume(volume);

    DoVolumeMapModel(volume, fiber->m_FiberData->m_ModelNode);
    
    for (QVector<Widget3DData*>::iterator objectIter = ViewWidgetVector3D.begin();
        objectIter != ViewWidgetVector3D.end();
        ++objectIter)
    {
        Widget3DData* widget3dData = (*objectIter);
        vtkSegment* seg = widget3dData->segment;
        vtkMRMLSegmentationNode* segment_node = widget3dData->m_SegmentationNode;

        std::string masterRepresentationName = segment_node->GetSegmentation()->GetMasterRepresentationName();
        vtkOrientedImageData* currentLabelmap = vtkOrientedImageData::SafeDownCast(
            seg->GetRepresentation(masterRepresentationName));
        DoVolumeMapImageData(volume, currentLabelmap);
    }

    double bounds[6] = { 1 };
    fiber->m_FiberData->m_ModelNode->GetRASBounds(bounds);
    DoStatisticVolume(volume, bounds);
    Reinit();
    return;










    vtkMRMLTransformNode* linearTransformNode = fiber->m_FiberData->m_ModelNode->GetParentTransformNode();
    vtkSmartPointer<vtkMatrix4x4> matirx1 = linearTransformNode->GetMatrixTransformToParent();
    for (QVector<Widget3DData*>::iterator objectIter = ViewWidgetVector3D.begin();
        objectIter != ViewWidgetVector3D.end();
        ++objectIter)
    {
        Widget3DData* widget3dData = (*objectIter);
        vtkSegment* seg = widget3dData->segment;
        auto collisionDetection = vtkCollisionDetectionFilter::New();
        vtkPolyData* polyData = vtkPolyData::SafeDownCast(seg->GetRepresentation(vtkSegmentationConverter::GetClosedSurfaceRepresentationName()));
        collisionDetection->SetInputData(0, polyData);
        vtkPolyData* polyData2 = fiber->m_FiberData->m_ModelNode->GetPolyData();
        collisionDetection->SetInputData(1, polyData2);
        auto matrix = vtkMatrix4x4::New();
        collisionDetection->SetMatrix(0, matrix);
        collisionDetection->SetMatrix(1, matirx1);
        collisionDetection->SetBoxTolerance(0.0);
        collisionDetection->SetCellTolerance(0.0);
        collisionDetection->SetNumberOfCellsPerNode(1);
        collisionDetection->SetCollisionModeToFirstContact();
        collisionDetection->SetOpacity(0.3);
        collisionDetection->Update();
        int numberOfCollisions = collisionDetection->GetNumberOfContacts();
        cout << "Number of Collision:" << numberOfCollisions << endl;;
    }

    auto node1 = mrmlScene()->GetFirstNodeByName("BrainParcellationSegmentation");
    vtkMRMLSegmentationNode* node = vtkMRMLSegmentationNode::SafeDownCast(node1);
    if (!node) {
        return;
    }
    vtkSegmentation* segmentation = node->GetSegmentation();
    vtkMRMLSegmentationDisplayNode* displayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(node->GetDisplayNode());
    for (int i = 0; i < segmentation->GetNumberOfSegments(); i++) {
        vtkSegment* segment = segmentation->GetNthSegment(i);
        std::string name = segment->GetName();
        
        auto collisionDetection = vtkCollisionDetectionFilter::New();
        vtkPolyData* polyData = vtkPolyData::SafeDownCast(segment->GetRepresentation(vtkSegmentationConverter::GetClosedSurfaceRepresentationName()));
        collisionDetection->SetInputData(0, polyData);
        vtkPolyData* polyData2 = fiber->m_FiberData->m_ModelNode->GetPolyData();
        collisionDetection->SetInputData(1, polyData2);
        auto matrix = vtkMatrix4x4::New();
        collisionDetection->SetMatrix(0, matrix);
        collisionDetection->SetMatrix(1, matirx1);
        collisionDetection->SetBoxTolerance(0.0);
        collisionDetection->SetCellTolerance(0.0);
        collisionDetection->SetNumberOfCellsPerNode(1);
        collisionDetection->SetCollisionModeToFirstContact();
        collisionDetection->SetOpacity(0.3);
        collisionDetection->Update();
        int numberOfCollisions = collisionDetection->GetNumberOfContacts();
        auto id = node->GetSegmentation()->GetSegmentIdBySegment(segment);
        //cout << "Number of Collision:" << numberOfCollisions << endl;;
        if (name.find("none") != -1) {
            segmentation->RemoveSegment(segment);
            continue;
        }
        if (numberOfCollisions > 0) {
            cout << "in type of Collision:" << segment->GetName() << endl;
            displayNode->SetSegmentVisibility(id, true);
            displayNode->SetSegmentOpacity3D(id, 0.5);
        }
        else {
            //segmentation->RemoveSegment(segment);
            cout << "not any:" << segment->GetName() << endl;
            
            displayNode->SetSegmentVisibility(id, false);
        }
    }
}

void qSlicerNaviModuleWidget::on_fiber_depth(NaviWidgetFiber* fiber, int index) {
    Q_D(qSlicerNaviModuleWidget);

    auto sliceController = qSlicerApplication::application()->layoutManager()->sliceWidget("Red")->sliceController();
    auto slider = sliceController->sliceOffsetSlider();
    auto max = slider->maximum();
    auto min = slider->minimum();
    auto val = min+(max - min) / 400 * index;
    slider->setValue(val);
    //vtkMRMLTransformNode* linearTransformNode = fiber->m_FiberData->m_ModelNode->GetParentTransformNode();
    //vtkSmartPointer<vtkMatrix4x4> matirx1  = fiber->m_FiberData->m_ModelNode->GetParentTransformNode()->GetMatrixTransformFromParent();

    //double vector1[3];
    //vector1[0] = fiber->m_FiberData->m_PointInput[0] - fiber->m_FiberData->m_PointTarget[0];
    //vector1[1] = fiber->m_FiberData->m_PointInput[1] - fiber->m_FiberData->m_PointTarget[1];
    //vector1[2] = fiber->m_FiberData->m_PointInput[2] - fiber->m_FiberData->m_PointTarget[2];
    //double newp[3];
    //newp[0] = fiber->m_FiberData->m_PointTarget[0] + vector1[0] * index / 400;
    //newp[1] = fiber->m_FiberData->m_PointTarget[1] + vector1[1] * index / 400;
    //newp[2] = fiber->m_FiberData->m_PointTarget[2] + vector1[2] * index / 400;


    //matirx1->SetElement(0,3, newp[0]);
    //matirx1->SetElement(1, 3, newp[1]);
    //matirx1->SetElement(2, 3, newp[2]);

    ////玄学,不清楚原理,下面的会自动变为负值
    //matirx1->SetElement(0, 2, -matirx1->GetElement(0, 2));
    //matirx1->SetElement(1, 2, -matirx1->GetElement(1, 2));
    //matirx1->SetElement(2, 0, -matirx1->GetElement(2, 0));
    //matirx1->SetElement(2, 1, -matirx1->GetElement(2, 1));
    //linearTransformNode->SetMatrixTransformToParent(matirx1.GetPointer());


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
    for (cit = ControllerMap.begin(); cit != ControllerMap.end(); ++cit)
    {
        qMRMLSliceControllerWidget* controll = dynamic_cast<qMRMLSliceControllerWidget*>(cit->second);
        if (controll) {
            vtkMRMLSliceNode* sliceNode = controll->mrmlSliceNode();
            vtkNew<vtkMatrix4x4> ijkToRAS;
            sliceNode->SetOrientationToDefault();
            
            vtkMatrix4x4* sliceToRAS = sliceNode->GetSliceToRAS();

            sliceToRAS->Multiply4x4(modelToParentTransform,sliceToRAS, sliceToRAS);
            sliceNode->GetSliceToRAS()->DeepCopy(sliceToRAS);
            sliceNode->UpdateMatrices();
        }
    }
    Reinit();
    vtkMRMLSliceNode::JumpAllSlices(mrmlScene(), dx, dy, dz, 1);
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
        auto model_name = modelNode->GetAttribute(NaviWidgetFiber::fiber_name.c_str());
        auto input_id = modelNode->GetAttribute(NaviWidgetFiber::input_node_id.c_str());
        auto target_id = modelNode->GetAttribute(NaviWidgetFiber::output_node_id.c_str());
        auto color_red = modelNode->GetAttribute(NaviWidgetFiber::color_red.c_str());
        auto color_green = modelNode->GetAttribute(NaviWidgetFiber::color_green.c_str());
        auto color_blue = modelNode->GetAttribute(NaviWidgetFiber::color_blue.c_str());
        auto fiber_visible = modelNode->GetAttribute(NaviWidgetFiber::fiber_visible.c_str());
        auto fiber_ablation_visible = modelNode->GetAttribute(NaviWidgetFiber::fiber_ablation_visible.c_str());
        auto fiber_save_depth = modelNode->GetAttribute(NaviWidgetFiber::fiber_save_depth.c_str());
        auto ablation_node_id = modelNode->GetAttribute(NaviWidgetFiber::ablation_node_id.c_str());
        auto fiber_ptr = new FiberData();
        NaviWidgetFiber* itemWidget = new NaviWidgetFiber(fiber_ptr, true, d->listWidgetOpticalFiber, this->mrmlScene(), d->logic());
        TestFiberWidgetVector.append(itemWidget);
        itemWidget->SetDeleteable(false);
        itemWidget->ui.textEdit->setText(QString::fromLocal8Bit(model_name));
        QListWidgetItem* item = new QListWidgetItem(d->listWidgetOpticalFiber);
        d->listWidgetOpticalFiber->setItemWidget(item, itemWidget);
        d->listWidgetOpticalFiber->addItem(item);
        itemWidget->ui.widgetSliderBkg->show();

        int width = d->listWidgetOpticalFiber->geometry().width();
        if (!itemWidget->m_FiberData->m_IsShuqian) {
            
            item->setSizeHint(QSize(width, 140));
        }
        else {
            item->setSizeHint(QSize(width, 412));
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

        connect(itemWidget, SIGNAL(delete_fiber(NaviWidgetFiber*)), this, SLOT(OnDeleteFiberWidget(NaviWidgetFiber*)));
        //connect(itemWidget, SIGNAL(need_update()), this, SLOT(update_show()));
        connect(itemWidget, SIGNAL(show_fiber(NaviWidgetFiber*, QString)), this, SLOT(on_show_fiber(NaviWidgetFiber*, QString)));
        connect(itemWidget, SIGNAL(move_fiber(NaviWidgetFiber*)), this, SLOT(on_move_fiber(NaviWidgetFiber*)));

        connect(itemWidget, SIGNAL(clicked(NaviWidgetFiber*)), this, SLOT(UpdateFiberList(NaviWidgetFiber*)));

        connect(itemWidget, &NaviWidgetFiber::rotate_to_vector, this, &qSlicerNaviModuleWidget::on_rotate_to_vector);
        connect(itemWidget, &NaviWidgetFiber::fiber_depth, this, &qSlicerNaviModuleWidget::on_fiber_depth);
        connect(itemWidget, &NaviWidgetFiber::estimate, this, &qSlicerNaviModuleWidget::on_estimate);
        int dp = QString::fromLocal8Bit(fiber_save_depth).toInt();
        itemWidget->ui.horizontalSlider->setValue(dp);
        
    }
}

void qSlicerNaviModuleWidget::OnDeleteFiberWidget(NaviWidgetFiber* fiber) {
    Q_D(qSlicerNaviModuleWidget);
    int n = d->listWidgetOpticalFiber->count();
    for (int i = 0; i < n; i++) {
        QListWidgetItem* item = d->listWidgetOpticalFiber->item(i);
        QWidget* widget = d->listWidgetOpticalFiber->itemWidget(item);
        NaviWidgetFiber* fiberWidget = dynamic_cast<NaviWidgetFiber*>(widget);
        if (fiberWidget && fiberWidget == fiber) {
            fiberWidget->DeleteFromScene();
            d->listWidgetOpticalFiber->takeItem(i);
        }
    }

    
}



void qSlicerNaviModuleWidget::exit() {
    this->Superclass::exit();
    Q_D(qSlicerNaviModuleWidget);
    cout << "Exit Reigster Widget" << endl;
    SetGlobalSaveValue("navi_set", "1");
    Vector2D.clear();
    d->listWidget2DMode->clear();

    

    ViewWidgetVector3D.clear();
    d->listWidget3DMode->clear();
}

//新建光纤
void qSlicerNaviModuleWidget::onSegment3DCancel()
{
    Q_D(qSlicerNaviModuleWidget);
    cout<<"cancel segment"<<endl;
    d->logic()->DeleteSegment(d->m_Temp3DWidget->m_Data->segmentID);
    d->m_Temp3DWidget->m_Data->segment = nullptr;
    m_State = qSlicerNaviModuleWidget::None;
    d->tabWidgetModel->setCurrentIndex(0);
    DoInitTemp3DWidget();
    d->EffectsWidget->setActiveEffect(nullptr);
}

void qSlicerNaviModuleWidget::onRemoveSegment(NaviWidget3D* widget3d) {
    Q_D(qSlicerNaviModuleWidget);
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
    
   
    DoFresh2DListView();
    DoFresh3DListView();
}

//确认分割
void qSlicerNaviModuleWidget::onSegment3DConfirm()
{
    Q_D(qSlicerNaviModuleWidget);
    //d->m_Temp3DWidget->SetAttribute(NaviWidget3D::structure_name.c_str(),)
    ViewWidgetVector3D.push_back(d->m_Temp3DWidget->m_Data);
    //d->m_Temp3DWidget->m_Data->m_SegmentationNode->SetMasterRepresentationToClosedSurface();
    d->tabWidgetModel->setCurrentIndex(0);
    DoInitTemp3DWidget();
    DoFresh3DListView();
    d->EffectsWidget->setActiveEffect(nullptr);
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