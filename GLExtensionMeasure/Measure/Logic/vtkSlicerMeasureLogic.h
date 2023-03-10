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

// .NAME vtkSlicerMeasureLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerMeasureLogic_h
#define __vtkSlicerMeasureLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerMeasureModuleLogicExport.h"
#include "vtkMRMLMarkupsDisplayNode.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLMarkupsDisplayNode.h"
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLSliceCompositeNode.h"
#include "vtkMRMLSliceNode.h"
/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_MEASURE_MODULE_LOGIC_EXPORT vtkSlicerMeasureLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerMeasureLogic *New();
  vtkTypeMacro(vtkSlicerMeasureLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetActiveListID(vtkMRMLMarkupsNode* markupsNode);
  std::string AddNewDisplayNodeForMarkupsNode(vtkMRMLNode* mrmlNode);
  void SetDisplayNodeToDefaults(vtkMRMLMarkupsDisplayNode* displayNode);
  std::string GetSelectionNodeID();
  void StartSliceNodeInteraction(unsigned int parameters);
  vtkMRMLMarkupsDisplayNode* GetDefaultMarkupsDisplayNode();
  void ParseHelpNodeInfo(vtkMRMLNode* node);
  void CopyBasicDisplayProperties(vtkMRMLMarkupsDisplayNode* sourceDisplayNode, vtkMRMLMarkupsDisplayNode* targetDisplayNode);
  vtkMRMLSliceNode* SliceNode;
  vtkMRMLSliceCompositeNode* SliceCompositeNode;
protected:
  vtkSlicerMeasureLogic();
  virtual ~vtkSlicerMeasureLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerMeasureLogic(const vtkSlicerMeasureLogic&); // Not implemented
  void operator=(const vtkSlicerMeasureLogic&); // Not implemented
};

#endif
