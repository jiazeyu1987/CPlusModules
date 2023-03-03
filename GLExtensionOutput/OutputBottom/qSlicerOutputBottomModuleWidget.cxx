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
#include "qSlicerOutputBottomModuleWidget.h"
#include "ui_qSlicerOutputBottomModuleWidget.h"
#include "qGLTool.h"
#include "vtkMRMLSegmentationNode.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerOutputBottomModuleWidgetPrivate: public Ui_qSlicerOutputBottomModuleWidget
{
public:
  qSlicerOutputBottomModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerOutputBottomModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerOutputBottomModuleWidgetPrivate::qSlicerOutputBottomModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerOutputBottomModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerOutputBottomModuleWidget::qSlicerOutputBottomModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerOutputBottomModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerOutputBottomModuleWidget::~qSlicerOutputBottomModuleWidget()
{
}


void qSlicerOutputBottomModuleWidget::DoFreshFiberList() {
	Q_D(qSlicerOutputBottomModuleWidget);
	d->listWidget->clear();
	std::vector<vtkMRMLNode*> procNodes;
	GLTool::opticalVec.clear();
	int numProcNodes = qSlicerApplication::application()->mrmlScene()->GetNodesByClass("vtkMRMLModelNode", procNodes);
	for (int i = 0; i < numProcNodes; i++)
	{
		vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(procNodes[i]);
		auto model_type = modelNode->GetAttribute("fiber_type");
		if (!model_type) {
			continue;
		}
		auto stl_head = modelNode->GetAttribute("stl_head");
		if (!stl_head) {
			continue;
		}
		/*ResultFiberItem* itemWidget = new ResultFiberItem();
		QListWidgetItem* item = new QListWidgetItem(d->listWidget);
		d->listWidget->setItemWidget(item, itemWidget);
		d->listWidget->addItem(item);*/
		auto fiber_name = modelNode->GetAttribute("fiber_name");
		auto fiber_type = modelNode->GetAttribute("fiber_type");
		auto input_id = modelNode->GetAttribute("input_node_id");
		auto target_id = modelNode->GetAttribute("output_node_id");
		auto ablation_node_id = modelNode->GetAttribute("ablation_node_id");
		int segment_number = 0;
		if (ablation_node_id) {
			auto segment_node = vtkMRMLSegmentationNode::SafeDownCast(mrmlScene()->GetNodeByID(ablation_node_id));
			if(segment_node){
				segment_number = segment_node->GetSegmentation()->GetNumberOfSegments();
			}
		}

		auto leksell_x = modelNode->GetAttribute("leksell_frame_position_x");
		auto leksell_y = modelNode->GetAttribute("leksell_frame_position_y");
		auto leksell_z = modelNode->GetAttribute("leksell_frame_position_z");
		auto leksell_arc = modelNode->GetAttribute("leksell_frame_position_arc");
		auto leksell_ran = modelNode->GetAttribute("leksell_frame_position_ring");
		auto id1 = target_id;
		FiberOutPutInfo1* info = new FiberOutPutInfo1();
		double x1 = 0, y1 = 0, z1 = 0;
		if(id1){
			vtkMRMLMarkupsFiducialNode* m_TargetFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(
				mrmlScene()->GetNodeByID(id1));
			if(m_TargetFiducialNode){
				vtkMRMLMarkupsNode::ControlPoint* cp2 = m_TargetFiducialNode->GetNthControlPoint(0);
				x1 = cp2->Position[0];
				y1 = cp2->Position[1];
				z1 = cp2->Position[2];
			}
		}
		info->X = leksell_x;
		info->Y = leksell_y;
		info->Z = leksell_z;
		info->arcAngle = leksell_arc;
		info->ringAngle = leksell_ran;
		info->fiberName = QString::fromLocal8Bit(fiber_name);
		info->fiberType = QString::fromLocal8Bit(fiber_type);
		
		AddFiberItemToList(info);

		Optical optical;
		optical.X = leksell_x;
		optical.Y = leksell_y;
		optical.Z = leksell_z;
		optical.ARC = leksell_arc;
		optical.RAN = leksell_ran;
		optical.headstockType = "Leksell";
		optical.name = fiber_name;
		optical.model = fiber_type;
		optical.ablationCount = QString("%1").arg(segment_number);
		GLTool::opticalVec.push_back(optical);
	}


}

void qSlicerOutputBottomModuleWidget::onGotoPreviewPage() {
	auto cn = mrmlScene()->GetFirstNodeByClass("vtkMRMLColorNode");
	cn->SetAttribute("value", "5");
	mrmlScene()->InvokeEvent(1000005, cn);
}
void qSlicerOutputBottomModuleWidget::onGotoNextPage() {
	AddSignalNode("OnExitAnalyseBtn", "1");
}


//-----------------------------------------------------------------------------
void qSlicerOutputBottomModuleWidget::setup()
{
  Q_D(qSlicerOutputBottomModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  connect(d_ptr->btn_preview, SIGNAL(clicked()), this, SLOT(onGotoPreviewPage()));
  connect(d_ptr->btn_next, SIGNAL(clicked()), this, SLOT(onGotoNextPage()));
  InitFiberInfo();
}

void qSlicerOutputBottomModuleWidget::AddFiberItemToList(FiberOutPutInfo1* fiber)
{
	Q_D(qSlicerOutputBottomModuleWidget);
	ResultFiberItem* fiberItem = new ResultFiberItem();
	fiberItem->SetFiberInfo(fiber);
	QListWidgetItem* item = new QListWidgetItem(d->listWidget);
	item->setFlags(Qt::ItemIsSelectable);
	item->setSizeHint(QSize(371, 186));
	d->listWidget->setItemWidget(item, fiberItem);
	d->listWidget->addItem(item);
}

void qSlicerOutputBottomModuleWidget::InitFiberInfo()
{
	Q_D(qSlicerOutputBottomModuleWidget);
	d->listWidget->clear();

	/*FiberOutPutInfo* fiber = new FiberOutPutInfo();
	fiber->fiberName = QString(QStringLiteral("╧Бок1"));
	fiber->fiberType = QStringLiteral("FD04C01A-4mm-1.55mm");
	fiber->X = 0.0;
	fiber->Y = 0.0;
	fiber->Z = 0.0;
	fiber->arcAngle = 0.0;
	fiber->ringAngle = 0.0;
	AddFiberItemToList(fiber);*/

}

void qSlicerOutputBottomModuleWidget::exit() {
	this->Superclass::exit();
	Q_D(qSlicerOutputBottomModuleWidget);
}

void qSlicerOutputBottomModuleWidget::enter() {
	this->Superclass::enter();
	Q_D(qSlicerOutputBottomModuleWidget);
	SetGlobalSaveValue("output_set", "1");
	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByClass("vtkMRMLLayoutNode");
	vtkMRMLLayoutNode* lnode = vtkMRMLLayoutNode::SafeDownCast(node);
	if (lnode) {
		lnode->SetViewArrangement(vtkMRMLLayoutNode::SlicerLayoutOneUpYellowSliceView);
	}
	Reinit();
	DoFreshFiberList();
}