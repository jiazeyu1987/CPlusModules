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

// .NAME vtkSlicerFilterRightLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerFilterRightLogic_h
#define __vtkSlicerFilterRightLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include <vtkMRMLScalarVolumeNode.h>
#include "vtkSlicerFilterRightModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_FILTERRIGHT_MODULE_LOGIC_EXPORT vtkSlicerFilterRightLogic :
  public vtkSlicerModuleLogic
{
public:
	enum {
		NodeAddEvent = 100001,
		NodeRemoveEvent,
	};
  static vtkSlicerFilterRightLogic *New();
  vtkTypeMacro(vtkSlicerFilterRightLogic, vtkSlicerModuleLogic);

  void HideAllVolumeNode();

  void PrintSelf(ostream& os, vtkIndent indent);

  void RemoveVolumeNode(vtkMRMLScalarVolumeNode* node);

  void SetMRMLApplicationLogic(vtkMRMLApplicationLogic* logic) override;
protected:
  vtkSlicerFilterRightLogic();
  virtual ~vtkSlicerFilterRightLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerFilterRightLogic(const vtkSlicerFilterRightLogic&); // Not implemented
  void operator=(const vtkSlicerFilterRightLogic&); // Not implemented
public:
	std::vector<std::string> m_NameList;
	std::vector<vtkMRMLScalarVolumeNode*> m_NodeList;//store all vtkMRMLScalarVolumeNode
	void clear();
	void DoAddNode(vtkMRMLScalarVolumeNode*  volumeNode);
};

#endif
