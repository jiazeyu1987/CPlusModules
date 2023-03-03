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

// .NAME vtkSlicerFilterLeftLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerFilterLeftLogic_h
#define __vtkSlicerFilterLeftLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerFilterLeftModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_FILTERLEFT_MODULE_LOGIC_EXPORT vtkSlicerFilterLeftLogic :
  public vtkSlicerModuleLogic
{
public:
	enum {
		Select_Node_Event = 100001,
	};
  static vtkSlicerFilterLeftLogic *New();
  vtkTypeMacro(vtkSlicerFilterLeftLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetMRMLApplicationLogic(vtkMRMLApplicationLogic* logic) override;
protected:
  vtkSlicerFilterLeftLogic();
  virtual ~vtkSlicerFilterLeftLogic();

  //parse help node info 
  void ParseHelpNodeInfo(vtkMRMLNode* node);

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
  void ProcessMRMLLogicsEvents(vtkObject*, long unsigned int, void*) override;
private:

  vtkSlicerFilterLeftLogic(const vtkSlicerFilterLeftLogic&); // Not implemented
  void operator=(const vtkSlicerFilterLeftLogic&); // Not implemented
  std::string itkDicomSaveToPng(std::string source, std::string target);
  void GetMaxAndMinOfDicom(const itk::MetaDataDictionary& dictionary, int& max, int& min);
public:
	std::string patientName = "";
	std::string scanType = "";
	std::string serialNumber = "";
	std::string layerNumber = "";
	std::string AcquisitionDateTime = "";
};

#endif
