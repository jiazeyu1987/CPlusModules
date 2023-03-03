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

#ifndef __qSlicerFilterRightModuleWidget_h
#define __qSlicerFilterRightModuleWidget_h

// GL includes
#include "qlistwidget.h"
// Slicer includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerFilterRightModuleExport.h"
#include "qSlicerGLModuleWidget.h"

#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerFilterRightLogic.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLLayoutNode.h"
#include "qGLChooseDialog.h"
#include "qGLChooseItem.h"
#include "vtkSlicerFilterTopLogic.h"
#include "qvector.h"
#include "qSlicerMainWindow.h"
#include "qGLTool.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#define MaxProVal 100.0
#define MinProVal 0.0
class qSlicerFilterRightModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_FILTERRIGHT_EXPORT qSlicerFilterRightModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerFilterRightModuleWidget(QWidget *parent=0);
  virtual ~qSlicerFilterRightModuleWidget();
  void DoLoadFolder(QString dir);
  Q_INVOKABLE virtual void enter() override;
  Q_INVOKABLE virtual void exit() override;
  qGLChooseItem* GetChooseItem(RightItemData* itemData);
  void DoResetUI();
  bool AutoTestBreak(int i);
  QString GetMRBFileName();
  void DoAnalyseParasFromPatientModule();
  void DoSetInitMultiWidgetState();
  void DoLoadPath(QString dir);
  void fresh_right_ui_global_config();
  void HideAllMarkupiducialNodes();
public slots:
	void OnAddClick();
	void onGotoPreviewPage();
	void onLoadPatientFiles();
	void onLogicModified(vtkObject* caller, vtkObject* obj);
	void OnNodeAdd();
	void OnGotoPreviewPage();
	void OnGotoNextPage();
protected:
  QScopedPointer<qSlicerFilterRightModuleWidgetPrivate> d_ptr;

  virtual void setup();

  QString pathOfNodeAdded;

public:
	int LastMarkLabel=-1;
	QVector< qGLChooseItem*> m_ItemListForTest;
	qGLChooseItem* GetChooseItem(std::string alias);
	TopItemData* GetTopItemData(std::string alias);
	TopItemData* GetTopItemData(vtkMRMLScalarVolumeNode* node);
	void sendAddSignalNode(const char* name, const char* data);
	qGLChooseDialog* m_choose=nullptr;

private:
  Q_DECLARE_PRIVATE(qSlicerFilterRightModuleWidget);
  Q_DISABLE_COPY(qSlicerFilterRightModuleWidget);
};

#endif
