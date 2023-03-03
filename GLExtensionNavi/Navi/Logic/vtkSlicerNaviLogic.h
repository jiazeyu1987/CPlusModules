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

// .NAME vtkSlicerNaviLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerNaviLogic_h
#define __vtkSlicerNaviLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerNaviModuleLogicExport.h"
#include "vtkMRMLSegmentationNode.h"
#include "vtkSlicerMeasureLogicAbstract.h"
#include "vtkMatrix4x4.h"
#include "vtkMitkThickSlicesFilter.h"
#include "DoubleSlider.h"
#include "itkMatrix.h"
/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_NAVI_MODULE_LOGIC_EXPORT vtkSlicerNaviLogic :
  public vtkSlicerMeasureLogicAbstract
{
public:
   
    enum EventType
    {
        OpenAblationEstimatePanel = 10001,
        CloseNaviSettingPanel = 10002,
        OpenDoctorViewPanel = 10003,
        Fresh2DView = 19000,
        Fresh3DView = 19001,
    };
  static vtkSlicerNaviLogic *New();
  void DeleteSegment(std::string id);
  vtkTypeMacro(vtkSlicerNaviLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetAndObserveNode(vtkMRMLNode* node) { vtkObserveMRMLNodeMacro(node) };
  void RotateToVector(double* m_PointInput, double* m_PointTarget);
  void Multi4x4ForPython(vtkMatrix4x4* v1, vtkMatrix4x4* v2);
  vtkMatrix4x4* matrixForPython = nullptr;
  vtkMatrix4x4* getMatrixForPython();
  vtkMitkThickSlicesFilter* m_TSFilter = nullptr;
  vtkMitkThickSlicesFilter* GetTSFilter();
  itk::Image<short, 3>::Pointer ChangeVolumeToItkImage(vtkMRMLScalarVolumeNode* volume);
  vtkMRMLSegmentationNode* ProcessZFrame(vtkMRMLScalarVolumeNode* volume, const char* transform_id);
protected: 
  vtkSlicerNaviLogic();
  virtual ~vtkSlicerNaviLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
  virtual void OnMRMLNodeModified(vtkMRMLNode* node);
  void RegisterNodes() override;
  void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) override;
  
private:
	vtkMRMLSegmentationNode* m_SegmentationNode = nullptr;
  vtkSlicerNaviLogic(const vtkSlicerNaviLogic&); // Not implemented
  void operator=(const vtkSlicerNaviLogic&); // Not implemented
};

#endif
