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

// .NAME vtkSlicerLoginLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerLoginLogic_h
#define __vtkSlicerLoginLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerLoginModuleLogicExport.h"
#include "vtkMitkLevelWindowFilter.h"

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_LOGIN_MODULE_LOGIC_EXPORT vtkSlicerLoginLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerLoginLogic *New();
  vtkTypeMacro(vtkSlicerLoginLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkMitkLevelWindowFilter* levelWindowFilter() {
	  return vtkMitkLevelWindowFilter
		  ::New();
  }
  void LoadVolumeAsync(std::string path, int type);

protected:
  vtkSlicerLoginLogic();
  virtual ~vtkSlicerLoginLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerLoginLogic(const vtkSlicerLoginLogic&); // Not implemented
  void operator=(const vtkSlicerLoginLogic&); // Not implemented
};

#endif
