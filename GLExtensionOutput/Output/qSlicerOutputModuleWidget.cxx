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
#include<QDateTime>

// Slicer includes
#include "qSlicerOutputModuleWidget.h"
#include "ui_qSlicerOutputModuleWidget.h"

#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkMRMLModelNode.h"
#include "qSurgeryPlanReportWidget.h"
#include "vtkMRMLSegmentationNode.h"
#include "QFileDialog.h"
#include "qSlicerIO.h"
#include "qSlicerCoreIOManager.h"
#include "qSlicerIOManager.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerOutputModuleWidgetPrivate: public Ui_qSlicerOutputModuleWidget
{
public:
  qSlicerOutputModuleWidgetPrivate();
  void Init();
};

//-----------------------------------------------------------------------------
// qSlicerOutputModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerOutputModuleWidgetPrivate::qSlicerOutputModuleWidgetPrivate()
{
}

void qSlicerOutputModuleWidgetPrivate::Init()
{

}
//-----------------------------------------------------------------------------
// qSlicerOutputModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerOutputModuleWidget::qSlicerOutputModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerOutputModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerOutputModuleWidget::~qSlicerOutputModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerOutputModuleWidget::setup()
{
  Q_D(qSlicerOutputModuleWidget);
  d->setupUi(this);
  InitInfo();
  //d->Init();
  //connect(d_ptr->btnPreview, SIGNAL(clicked()), this, SLOT(onGotoPreviewPage()));
  connect(d_ptr->btnExport, &QPushButton::clicked, this, &qSlicerOutputModuleWidget::OnResultExport);
  connect(d_ptr->btnExportPDF, &QPushButton::clicked, this, &qSlicerOutputModuleWidget::OnPDFExport);
  //connect(d_ptr->btnIdentify, &QPushButton::clicked, this, &qSlicerOutputModuleWidget::OnIdentifyHeadFramePositionNew);
  //connect(d_ptr->btnModel, &QPushButton::toggled, this, &qSlicerOutputModuleWidget::OnDisplayModelChange);
  //connect(d_ptr->cmbSeries, SIGNAL(currentIndexChanged(int)), this, SLOT(ShowSelectSeries(int)));
  //connect(d_ptr->cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(SetIdentifyType(int)));
  d_ptr->btnExport->setStyleSheet("QPushButton {background-color: #525F7B;}"
	  "QPushButton:hover {background-color: #3377FF;}");
  d_ptr->btnExportPDF->setStyleSheet("QPushButton {background-color: #525F7B;}"
	  "QPushButton:hover {background-color: #3377FF;}");

}

void qSlicerOutputModuleWidget::onGotoPreviewPage() {
	
}



void qSlicerOutputModuleWidget::OnIdentifyHeadFramePositionNew()
{

}

void qSlicerOutputModuleWidget::enter() {
	this->Superclass::enter();
	Q_D(qSlicerOutputModuleWidget);
    //d->cmbSeries->clear();
    std::vector<vtkMRMLNode*> procNodes;
    int numProcNodes = qSlicerApplication::application()->mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", procNodes);
    for (int i = 0; i < numProcNodes; i++)
    {
        vtkMRMLScalarVolumeNode* procNode = vtkMRMLScalarVolumeNode::SafeDownCast(procNodes[i]);
        char* name = procNode->GetName();
        auto tName = QString::fromStdString(std::string(name));
        const char* is_registed_node = procNode->GetAttribute("is_registed");
        const char* alias_name = procNode->GetAttribute("alias_name");
        if (is_registed_node && std::string(is_registed_node) == "true") {

        }
        else {
            if (alias_name && std::string(alias_name) == "basic") {
            }
            else {
            }
        }
        //d->cmbSeries->addItem(QString::fromLocal8Bit(alias_name));
    }
    auto patientBirthday = GetGlobalSaveValue("patientBirthday");
    auto patientAge = GetGlobalSaveValue("patientAge");
    auto patientSex = GetGlobalSaveValue("patientSex");
    auto createTime = GetGlobalSaveValue("createTime");

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    auto patientName = GetGlobalSaveValue("patientName");
    auto remark = GetGlobalSaveValue("remark");
    d->lblBirthday->setText(patientBirthday);
    d->lblAge->setText(patientAge);
    d->lblSex->setText(patientSex);
    d->lblDate->setText(current_date);
    d->lblName->setText(patientName);
    DoFreshFiberInfo();
    DoFreshTumorInfo();
    SetGlobalSaveValue("output_set", "1");
    SetGlobalSaveValue("current_page", "4");
}

void qSlicerOutputModuleWidget::DoFreshTumorInfo() {
    Q_D(qSlicerOutputModuleWidget);
    std::vector<vtkMRMLNode*> procNodes;
    
    int numProcNodes = qSlicerApplication::application()->mrmlScene()->GetNodesByClass("vtkMRMLSegmentationNode", procNodes);
    int num = 0;
    for (int i = 0; i < numProcNodes; i++)
    {
        vtkMRMLSegmentationNode* modelNode = vtkMRMLSegmentationNode::SafeDownCast(procNodes[i]);
        auto model_type = modelNode->GetAttribute("structure_name");
        if (!model_type) {
            continue;
        }
        if (model_type == QStringLiteral("肿瘤")) {
            num++;
        }
        
    }
    d->lblFocalZoneNum->setText(QString("%1").arg(num));
}

void qSlicerOutputModuleWidget::DoFreshFiberInfo() {
    Q_D(qSlicerOutputModuleWidget);
    std::vector<vtkMRMLNode*> procNodes;
    int numProcNodes = qSlicerApplication::application()->mrmlScene()->GetNodesByClass("vtkMRMLModelNode", procNodes);
    int num = 0;
    for (int i = 0; i < numProcNodes; i++)
    {
        vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(procNodes[i]);
        auto model_type = modelNode->GetAttribute("fiber_type");
        if (!model_type) {
            continue;
        }
        auto stl_head = modelNode->GetAttribute("stl_head");
        if (stl_head) {
            continue;
        }
        num++;
    }
    d->lblFiberNum->setText(QString("%1").arg(num));
}

void qSlicerOutputModuleWidget::exit() {
	this->Superclass::exit();
	Q_D(qSlicerOutputModuleWidget);
}

void qSlicerOutputModuleWidget::OnPDFExport()
{
    SurgeryPlanReportWidget sprw;
}

void qSlicerOutputModuleWidget::OnResultExport()
{
    QString filename = QStringLiteral("%1.mrb")
        .arg(GLTool::patientAnalyseId)
        .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    filename = QFileDialog::getSaveFileName(nullptr, QStringLiteral("选择输出文件路径"), filename, tr("(*.mrb)"));
    if (filename.isEmpty())
    {
        return ;
    }

    auto im = qSlicerApplication::application()->ioManager();

    qSlicerCoreIOManager* coreIOManager = qSlicerCoreApplication::application()->coreIOManager();
    qSlicerIO::IOProperties fileParameters; 
    fileParameters["fileName"] = filename;
    QWidget* widget = qSlicerApplication::application()->layoutManager()->viewport();
    coreIOManager->saveNodes("SceneFile", fileParameters); //此函数比较耗时
}

void qSlicerOutputModuleWidget::OnDisplayModelChange(bool value) 
{

}

void qSlicerOutputModuleWidget::SetIdentifyType(int num)
{

}

void qSlicerOutputModuleWidget::ShowSelectSeries(int num)
{

}

void qSlicerOutputModuleWidget::InitInfo()
{
	Q_D(qSlicerOutputModuleWidget);
	d->lblDate->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
	d->lblName->setText("D0G496");
	d->lblBirthday->setText("2001/12/05");
	d->lblAge->setText("20");
	d->lblSex->setText("M");

	//auto predicateName = mitk::NodePredicateProperty::New("stl_name");
	//auto predicateType = mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("stl_head"));
	//auto fiberNodes = GetDataStorage()->GetSubset(mitk::NodePredicateAnd::New(predicateName, predicateType));
	//int fiberNum = fiberNodes->size();
    d->lblFiberNum->setText("1");

	//auto tumorNodes = GetDataStorage()->GetSubset(mitk::NodePredicateProperty::New("structure_alias", mitk::StringProperty::New("Tumor")));
	//int tumorNum = tumorNodes->size();
    d->lblFocalZoneNum->setText("0");
}