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

#ifndef __qSlicerFilterTopModuleWidget_h
#define __qSlicerFilterTopModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"
#include "qlistwidget.h"
#include "qSlicerFilterTopModuleExport.h"
#include "qSlicerGLModuleWidget.h"
#include <qGLFilterListItem.h>
#include "qcollator.h"
#include "qdir.h"
#include "qfile.h"
#include "qGLFilterListItem.h"
#include <qSlicerApplication.h>
#include "vtkSlicerFilterTopLogic.h"
#include "qSlicerApplication.h"
#include <QSettings>
#include "qSlicerLayoutManager.h"
#include <QtCore/qdir.h>
#include "qSlicerIOManager.h"
#include "dicom.h"
#include "vtkMRMLColorNode.h"
#include <itkImageSeriesReader.h>
#include "qvector.h"
class qSlicerFilterTopModuleWidgetPrivate;
class vtkMRMLNode;
class qGLFilterListItem;
/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_FILTERTOP_EXPORT qSlicerFilterTopModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerFilterTopModuleWidget(QWidget *parent=0);
  void DoLoadFolder(QString dir);
  virtual ~qSlicerFilterTopModuleWidget();
  void OnNodeAdd();
  void onFreshState();
  void InitWidgetListFromDicomCollection(bool isBackup);
  Q_INVOKABLE virtual void enter() override;
  Q_INVOKABLE virtual void exit() override;
  void onChangePageToRegister();
  void LoadDcm(QString filePath);

  QStringList filenameInDir(const QString& path, QString tail);
  qGLFilterListItem* GetWidgetByItem(TopItemData*);
public slots:
	void OnRemoveClick();
	void OnFreshItem(const char*);
	void onLogicModified(vtkObject* caller, vtkObject* obj);
	void OnClickWidget(vtkMRMLScalarVolumeNode* volumeNode);
	void OnRightClickWidget(qGLFilterListItem* filterItem);
protected:
  QScopedPointer<qSlicerFilterTopModuleWidgetPrivate> d_ptr;
  virtual void setup();
  void RemoveDataNodeFromList(QListWidgetItem* item);

private:
  Q_DECLARE_PRIVATE(qSlicerFilterTopModuleWidget);
  Q_DISABLE_COPY(qSlicerFilterTopModuleWidget);
  QStringList GetFileNameInDir(const QString& path, QString tail, bool isNeedTotalPath = false);
  void SplitFileFold(QString path, QString toPath);
public:
	QListWidgetItem* IsExistMarkLabel();

private:
	qGLFilterListItem* m_currentItem=nullptr;

private:
	qGLFilterListItem* m_LastFilterItem = nullptr;
};

#endif
