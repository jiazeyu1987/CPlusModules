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

// .NAME vtkSlicerFilterTopLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerFilterTopLogic_h
#define __vtkSlicerFilterTopLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>
#include <vtkMRMLScalarVolumeNode.h>
#include "vtkSlicerFilterTopModuleLogicExport.h"
#include "vtkMRMLScalarVolumeNode.h"

class VTK_SLICER_FILTERTOP_MODULE_LOGIC_EXPORT RightItemData {
public:
	void* m_Widget = nullptr;
	std::string m_NameRegistration = "";
	std::string m_NameTransmit = "";
	std::string m_NameRegistrationIcon = "";
	std::string m_NameTransmitIcon = "";
};

class VTK_SLICER_FILTERTOP_MODULE_LOGIC_EXPORT TopItemData {
public:
	vtkMRMLScalarVolumeNode* m_DataNode = nullptr;
	vtkMRMLScalarVolumeNode* m_RegisterNode = nullptr;
	RightItemData* m_ItemFriend = nullptr;
	std::string m_AliasName = "";
	std::string m_ImagePath = "";
	bool m_IsTransmit = false;


	void SetMyDataNode(vtkMRMLScalarVolumeNode* dn)
	{
		m_DataNode = dn;
	}

	void SetRegisterNode(vtkMRMLScalarVolumeNode* dn)
	{
		m_RegisterNode = dn;
	} 

	void SetFriendItem(RightItemData* item, bool isTransmit)
	{
		m_ItemFriend = item;
		if (item == nullptr) {
			
			if (m_RegisterNode) {
				m_RegisterNode->RemoveAttribute("register_name");
			}
			if (m_DataNode) {
				m_DataNode->RemoveAttribute("alias_name");
				m_DataNode->RemoveAttribute("is_transmit");
				m_DataNode->RemoveAttribute("alias_transmit");
			}
			m_IsTransmit = false;
			return;
		}
		m_IsTransmit = isTransmit;
		m_AliasName = item->m_NameRegistration;
		if (isTransmit) m_AliasName = item->m_NameTransmit;

		auto info = m_AliasName;
		std::string transmit_alias = item->m_NameTransmit;

		if (m_DataNode) {
			m_DataNode->SetAttribute("alias_name", info.c_str());
			m_DataNode->SetAttribute("is_transmit", m_IsTransmit ? "1" : "0");//is transmit
			if (m_IsTransmit && transmit_alias.c_str()) {
				m_DataNode->SetAttribute("alias_base", item->m_NameRegistration.c_str());// the transmit node name of the registration node
			}
			else {
				m_DataNode->RemoveAttribute("alias_base");
			}
		}
		if (m_RegisterNode) {
			m_RegisterNode->SetAttribute("register_name", info.c_str());
		}
	}
};

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_FILTERTOP_MODULE_LOGIC_EXPORT vtkSlicerFilterTopLogic :
  public vtkSlicerModuleLogic
{
public:

	enum {
		NodeAddEvent = 100001,
		NodeRemoveEvent,
	};
	TopItemData* m_SelectedItemData = nullptr;
	std::vector<TopItemData*> m_TopItemDataList;
	std::vector<TopItemData*> m_Right2TopItemDataList;
  static vtkSlicerFilterTopLogic *New();
  vtkTypeMacro(vtkSlicerFilterTopLogic, vtkSlicerModuleLogic);
  void HideAllVolumeNode();

  void PrintSelf(ostream& os, vtkIndent indent);

  void RemoveVolumeNode(vtkMRMLScalarVolumeNode* node);

  void SetMRMLApplicationLogic(vtkMRMLApplicationLogic* logic) override;

protected:
  vtkSlicerFilterTopLogic();
  virtual ~vtkSlicerFilterTopLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:
	void DoAnalyseHelpNode(vtkMRMLNode* node);
	vtkSlicerFilterTopLogic(const vtkSlicerFilterTopLogic&); // Not implemented
	void operator=(const vtkSlicerFilterTopLogic&); // Not implemented
public:
	void clear();
};

#endif
