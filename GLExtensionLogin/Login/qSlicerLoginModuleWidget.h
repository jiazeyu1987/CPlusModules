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

#ifndef __qSlicerLoginModuleWidget_h
#define __qSlicerLoginModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerLoginModuleExport.h"
#include "qGLLoginView.h"


class qSlicerLoginModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_LOGIN_EXPORT qSlicerLoginModuleWidget :
  public qSlicerGLModuleWidget //只能这个继承，否则scense为空
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerLoginModuleWidget(QWidget *parent=0);
  virtual ~qSlicerLoginModuleWidget();
  Q_INVOKABLE void LoadVolumeAsync(QString path, int type);
public slots:
	void onGotoNextPage();
	void handleResults(const int result);
public:
signals:
	void operate(QString, int);  // 发送信号，触发线程
protected:
  QScopedPointer<qSlicerLoginModuleWidgetPrivate> d_ptr;
  Q_INVOKABLE virtual void enter() override;
  Q_INVOKABLE virtual void exit() override;
  virtual void setup();
  bool AutoTestBreak(int i);

  void AutoClearUiData();
private:
  ZZLoginView* login;
  Q_DECLARE_PRIVATE(qSlicerLoginModuleWidget);
  Q_DISABLE_COPY(qSlicerLoginModuleWidget);

  friend class ZZLoginView;
};

#endif
