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
#include "vtkSlicerFilterRightLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include "vtkMRMLColorNode.h"
// STD includes
#include <cassert>
#include "vtkMRMLDiffusionWeightedVolumeNode.h"
#include"vtkEventBroker.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerFilterRightLogic);

//----------------------------------------------------------------------------
vtkSlicerFilterRightLogic::vtkSlicerFilterRightLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerFilterRightLogic::~vtkSlicerFilterRightLogic()
{
}

void vtkSlicerFilterRightLogic::clear() {
    m_NameList.clear();
    m_NodeList.clear();
}

void vtkSlicerFilterRightLogic::HideAllVolumeNode()
{
    for (auto iter = m_NodeList.begin(); iter != m_NodeList.end(); iter++)
    {
        vtkMRMLScalarVolumeNode* node = (*iter);
        if(node)
        node->SetDisplayVisibility(false);
    }
}

//----------------------------------------------------------------------------
void vtkSlicerFilterRightLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

void vtkSlicerFilterRightLogic::RemoveVolumeNode(vtkMRMLScalarVolumeNode* node)
{
    this->GetMRMLScene()->RemoveNode(node);
}

//---------------------------------------------------------------------------
void vtkSlicerFilterRightLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
    this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}




//----------------------------------------------------------------------------
void vtkSlicerFilterRightLogic
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
void vtkSlicerFilterRightLogic::RegisterNodes()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFilterRightLogic::UpdateFromMRMLScene()
{
    assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFilterRightLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
    vtkMRMLColorNode* node1 = vtkMRMLColorNode::SafeDownCast(node);
    if (node1) {
        auto help_node = node1->GetAttribute("Help_Node");
        if (help_node && strcmp(help_node, "true") == 0) {
            this->InvokeEvent(vtkCommand::ModifiedEvent, node1);
        }
    }
    
    vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
    if (!volumeNode) {
        return;
    }
    std::cout << "detect volume node in vtkSlicerFilterRightLogic:" << volumeNode->GetID()<<std::endl;
    vtkMRMLDiffusionWeightedVolumeNode* node_dwi = vtkMRMLDiffusionWeightedVolumeNode::SafeDownCast(node);
    if (node_dwi) {
        return;
    }

    auto is_dwi = node->GetAttribute("dwi");
    if (is_dwi) {
        return;
    }
    DoAddNode(volumeNode);
    
}

void vtkSlicerFilterRightLogic
::DoAddNode(vtkMRMLScalarVolumeNode* volumeNode) {
    std::string filename = "";
    /*if (volumeNode->filename)
        filename = std::string(volumeNode->filename);*/
    m_NameList.push_back(volumeNode->GetName());
    m_NodeList.push_back(volumeNode);
    this->InvokeEvent(NodeAddEvent);
}


//---------------------------------------------------------------------------
void vtkSlicerFilterRightLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
    vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
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
    this->InvokeEvent(vtkCommand::ModifiedEvent);
}
