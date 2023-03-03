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

// TopMenu Logic includes
#include "vtkSlicerTopMenuLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

#include "vtkMRMLColorNode.h"

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerTopMenuLogic);

//----------------------------------------------------------------------------
vtkSlicerTopMenuLogic::vtkSlicerTopMenuLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerTopMenuLogic::~vtkSlicerTopMenuLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerTopMenuLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerTopMenuLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerTopMenuLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerTopMenuLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

void vtkSlicerTopMenuLogic::ParseHelpNodeInfo(vtkMRMLNode* node) {
    const char* helpType = node->GetAttribute("Help_Name");
    const char* helpVal = node->GetAttribute("Help_Info_0");
    if (helpType && strcmp(helpType, "exit_with_save") == 0) {
        GetMRMLScene()->RemoveNode(node);     
        this->InvokeEvent(9797);
        
    }
}

//---------------------------------------------------------------------------
void vtkSlicerTopMenuLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
    if (!node)
    {
        return;
    }
    vtkMRMLColorNode* colorNode = vtkMRMLColorNode::SafeDownCast(node);
    if (colorNode) {
        const char* isHelpNode = node->GetAttribute("Help_Node");
        if (isHelpNode && strcmp(isHelpNode, "true") == 0) {
            //ParseHelpNodeInfo(node);
            this->InvokeEvent(vtkCommand::ModifiedEvent, colorNode); 
            return;
        }
        return;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerTopMenuLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

void vtkSlicerTopMenuLogic
::OnMRMLNodeModified(vtkMRMLNode* node) {
    int j = 1;
}