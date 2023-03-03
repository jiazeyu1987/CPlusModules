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

// FilterRight Logic includes
#include "vtkSlicerFilterTopLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

#include"vtkEventBroker.h"
#include "vtkMRMLColorNode.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerFilterTopLogic);

//----------------------------------------------------------------------------
vtkSlicerFilterTopLogic::vtkSlicerFilterTopLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerFilterTopLogic::~vtkSlicerFilterTopLogic()
{
}


void vtkSlicerFilterTopLogic::HideAllVolumeNode()
{
}

//----------------------------------------------------------------------------
void vtkSlicerFilterTopLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

void vtkSlicerFilterTopLogic::RemoveVolumeNode(vtkMRMLScalarVolumeNode* node)
{
    this->GetMRMLScene()->RemoveNode(node);
}

//---------------------------------------------------------------------------
void vtkSlicerFilterTopLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
    this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}


void vtkSlicerFilterTopLogic
::clear() {
    m_TopItemDataList.clear();
    m_SelectedItemData = nullptr;   
}

//----------------------------------------------------------------------------
void vtkSlicerFilterTopLogic
::SetMRMLApplicationLogic(vtkMRMLApplicationLogic* logic)
{
    vtkMRMLApplicationLogic* oldLogic = this->GetMRMLApplicationLogic();
    if (logic == oldLogic)
    {
        return;
    }

    bool wasModifying = this->StartModify();
    if (oldLogic)
    {
        vtkEventBroker::GetInstance()->RemoveObservations(
            oldLogic, vtkSlicerApplicationLogic::RequestProcessedEvent,
            this, this->GetMRMLLogicsCallbackCommand());
    }

    Superclass::SetMRMLApplicationLogic(logic);
    assert(logic == this->GetMRMLApplicationLogic());

    // Observe application logic to know when the CLI is completed and the
    // associated data loaded.
    if (logic)
    {
        vtkEventBroker::GetInstance()->AddObservation(
            logic, vtkSlicerApplicationLogic::RequestProcessedEvent,
            this, this->GetMRMLLogicsCallbackCommand());
    }
    this->EndModify(wasModifying);
}

//-----------------------------------------------------------------------------
void vtkSlicerFilterTopLogic::RegisterNodes()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFilterTopLogic::UpdateFromMRMLScene()
{
    assert(this->GetMRMLScene() != 0);
}

void vtkSlicerFilterTopLogic
::DoAnalyseHelpNode(vtkMRMLNode* node) {
    auto type = node->GetAttribute("Help_Name");
    auto value = node->GetAttribute("Help_Info_0");
    if (type && strcmp(type, "filter_add_node") == 0) {
        if (value) {
            
        }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerFilterTopLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
    /*vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
    if (!volumeNode) {
        return;
    }
    std::string filename = "";
    if (volumeNode->filename)
        filename = std::string(volumeNode->filename);
    m_NameList.push_back(volumeNode->GetName());
    m_NodeList.push_back(volumeNode);
    this->InvokeEvent(NodeAddEvent);*/
    vtkMRMLColorNode* node1 = vtkMRMLColorNode::SafeDownCast(node);
    if(node1){
        {
            this->InvokeEvent(vtkCommand::ModifiedEvent,node1);
        }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerFilterTopLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
    /*vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
    if (volumeNode) {
        int pos = 0;
        std::string tname = volumeNode->GetName();
        for (auto iter = m_NameList.begin(); iter != m_NameList.end(); iter++, pos++) {
            if (*iter == tname) {
                m_NameList.erase(m_NameList.begin() + pos);
                m_NodeList.erase(m_NodeList.begin() + pos);
                break;
            }
        }
    }
    this->InvokeEvent(vtkCommand::ModifiedEvent);*/
}
