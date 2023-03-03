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

// Navi Logic includes
#include "vtkSlicerNaviLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>
#include "vtkMRMLSegmentEditorNode.h"
#include "vtkSegmentationConverterFactory.h"
#include <vtkSegmentationModifier.h>
#include "vtkBinaryLabelmapToClosedSurfaceConversionRule.h"
#include "vtkClosedSurfaceToBinaryLabelmapConversionRule.h"
#include "vtkClosedSurfaceToFractionalLabelmapConversionRule.h"
#include "vtkFractionalLabelmapToClosedSurfaceConversionRule.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkTransform.h"
#include "itkImageFileReader.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkITKImageWriter.h"
#include "QCoreApplication.h"
#include "itksys/SystemTools.hxx"
#include "vtkSlicerSegmentationsModuleLogic.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerNaviLogic);

//----------------------------------------------------------------------------
vtkSlicerNaviLogic::vtkSlicerNaviLogic()
{
    m_TSFilter = vtkMitkThickSlicesFilter::New();
}

//----------------------------------------------------------------------------
vtkSlicerNaviLogic::~vtkSlicerNaviLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerNaviLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkSlicerNaviLogic::RegisterNodes()
{
	if (!this->GetMRMLScene())
	{
		vtkErrorMacro("RegisterNodes: Invalid MRML scene");
		return;
	}

}

//---------------------------------------------------------------------------
void vtkSlicerNaviLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());

  
}

vtkMitkThickSlicesFilter* vtkSlicerNaviLogic::GetTSFilter(){
    return vtkMitkThickSlicesFilter::New();
}

itk::Image<short, 3>::Pointer vtkSlicerNaviLogic::ChangeVolumeToItkImage(vtkMRMLScalarVolumeNode* volume) {
    typedef typename itk::Image<short, 3> InputImageType;
    typedef typename itk::ImageFileReader<InputImageType>                     ReaderType;
    auto base = QCoreApplication::applicationDirPath();
    auto tmp_qt = base + "/tmp/navi/";
    itksys::SystemTools::MakeDirectory(tmp_qt.toStdString().c_str());
    auto tmp_file = tmp_qt + "ChangeVolumeToItkImage.nrrd";
    std::string file_path = tmp_file.toStdString();
    vtkNew<vtkMatrix4x4> matrix; // initialized to identity by default
    volume->GetRASToIJKMatrix(matrix);
    try {
        vtkITKImageWriter* writer = vtkITKImageWriter::New();
        writer->SetFileName(file_path.c_str());
        writer->SetInputConnection(volume->GetImageDataConnection());
        writer->SetRasToIJKMatrix(matrix);
        writer->Update();
        writer->Write();
    }
    catch (...) {
        cout << "catch...." << endl;
    }

    ReaderType::Pointer reader1 = ReaderType::New();
    reader1->SetFileName(file_path);
    reader1->Update();
    InputImageType::Pointer image = reader1->GetOutput();

    image->SetOrigin(volume->GetOrigin());

    double slicer_direction[3][3];
    volume->GetIJKToRASDirections(slicer_direction);
    itk::Matrix<double, 3, 3> matrix1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix1[i][j] = slicer_direction[i][j];
        }
    }
    image->SetDirection(matrix1);
    image->SetSpacing(volume->GetSpacing());
    return image;
}

vtkMRMLSegmentationNode* vtkSlicerNaviLogic::ProcessZFrame(vtkMRMLScalarVolumeNode* volume,const char* transform_id) {
    
    auto image = ChangeVolumeToItkImage(volume);
    DoubleSlider::GetInstance()->SetLogic(GetApplicationLogic()->GetModuleLogic("Models"));
    auto model =  DoubleSlider::GetInstance()->OnIdentifyHeadFramePositionNew(GetMRMLScene(),image, transform_id);
    if (model == nullptr) {
        return nullptr;
    }
    auto segmentation_leksell = vtkMRMLSegmentationNode::SafeDownCast(GetMRMLScene()->GetFirstNodeByName("segmentation_leksell"));
    if (!segmentation_leksell) {
        segmentation_leksell = vtkMRMLSegmentationNode::SafeDownCast(GetMRMLScene()->AddNewNodeByClass("vtkMRMLSegmentationNode", "segmentation_leksell"));
    }
    auto segment_logic = vtkSlicerSegmentationsModuleLogic::SafeDownCast(GetApplicationLogic()->GetModuleLogic("Segmentations"));
    segment_logic->ImportModelToSegmentationNode(model, segmentation_leksell);
    segmentation_leksell->RemoveAllDisplayNodeIDs();
    segmentation_leksell->CreateDefaultDisplayNodes();

    auto display_model_node = model->GetDisplayNode();
    if (!display_model_node) {
        model->CreateDefaultDisplayNodes();
        display_model_node = model->GetDisplayNode();
    }
    display_model_node->SetVisibility(false);
    segmentation_leksell->SetAttribute("polydata_model_transform_to_segmentation_save_bug", model->GetID());
    //GetMRMLScene()->RemoveNode(model);
    return segmentation_leksell;
}

//---------------------------------------------------------------------------
void vtkSlicerNaviLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerNaviLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
	vtkMRMLSegmentationNode* SegmentationNode = vtkMRMLSegmentationNode::SafeDownCast(node);
	if (SegmentationNode) {
		vtkNew<vtkIntArray> events;
		events->InsertNextValue(vtkSegmentation::SegmentAdded);
		events->InsertNextValue(vtkSegmentation::SegmentRemoved);
		events->InsertNextValue(vtkSegmentation::SegmentModified);
		vtkSetAndObserveMRMLNodeEventsMacro(m_SegmentationNode,SegmentationNode, events.GetPointer());
	}

	vtkMRMLMarkupsFiducialNode* FiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(node);
	if (FiducialNode) {
		this->InvokeEvent(9999, FiducialNode);
	}

	vtkMRMLModelNode* ModelNode = vtkMRMLModelNode::SafeDownCast(node);
	if (ModelNode) {
		this->InvokeEvent(9998, ModelNode);
	}

    if (ModelNode || FiducialNode || SegmentationNode) {
        return;
    }
    vtkMRMLColorNode* node1 = vtkMRMLColorNode::SafeDownCast(node);
    if (node1) {
        auto help_node = node1->GetAttribute("Help_Node");
        if (help_node && strcmp(help_node, "true") == 0) {
            this->InvokeEvent(vtkCommand::ModifiedEvent, node1);
        }
    }
}

void vtkSlicerNaviLogic::RotateToVector(double* m_PointInput, double* m_PointTarget) {
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
    matrixForPython = modelToParentTransform;
    this->InvokeEvent(9990, modelToParentTransform.GetPointer());
    vtkMRMLSliceNode::JumpAllSlices(GetMRMLScene(), dx, dy, dz, 1);
}

vtkMatrix4x4* vtkSlicerNaviLogic::getMatrixForPython() {
    return matrixForPython;
}

void vtkSlicerNaviLogic::Multi4x4ForPython(vtkMatrix4x4* v1, vtkMatrix4x4* v2) {
    v1->Multiply4x4(v2, v1, v1);
}

void vtkSlicerNaviLogic::OnMRMLNodeModified(vtkMRMLNode* node) {
	int i = 1;
}

//---------------------------------------------------------------------------
void vtkSlicerNaviLogic::ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData)
{
    
	this->InvokeEvent(event, callData);
}

void vtkSlicerNaviLogic::DeleteSegment(std::string id) {
	if (m_SegmentationNode) {
		cout << "remove segment by id" << id<< endl;
		m_SegmentationNode->RemoveSegment(id);
	}
}

//---------------------------------------------------------------------------
void vtkSlicerNaviLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
