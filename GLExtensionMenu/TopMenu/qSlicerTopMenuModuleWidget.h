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

#ifndef __qSlicerTopMenuModuleWidget_h
#define __qSlicerTopMenuModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerGLModuleWidget.h"

#include "qSlicerTopMenuModuleExport.h"
#include "qGLTool.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDViewControllerWidget.h"
class qSlicerTopMenuModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
//建议这个里面的东西从#include "qGLTool.cpp"，mitk顶部菜单栏就是GLTool.cpp
typedef struct
{
	QString versionRelase; //发行版本号
	QString versionComplete; //完整版本号
	QString installationDateTime; //安装时间
	QString registerName = QStringLiteral("杭州佳量医疗科技有限公司"); //注册人名称
	QString address = QStringLiteral("浙江省杭州市余杭区仓前街道仓兴街1390号9幢B座801室"); //地址
	QString contact = "(86)0571-88730192"; //联系方式
	QString copyright = QStringLiteral("Copyright &#169; 2021杭州佳量医疗科技有限公司. All Rights Reserved");
	QString licenseKey; //许可证号
} SoftwareInformation;

class Q_SLICER_QTMODULES_TOPMENU_EXPORT qSlicerTopMenuModuleWidget :
  public qSlicerGLModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerGLModuleWidget Superclass;
  qSlicerTopMenuModuleWidget(QWidget *parent=0);
  virtual ~qSlicerTopMenuModuleWidget();

public slots:
	void OnSaveBtn();
	void OnSettingBtn();
	void OnHelpBtn();
	void OnExitAnalyseBtn();
	void OnExitBtn();
	void on_controller_visible_triggered();
	void on_action_reinit_triggered();
	void Ontimeout();
protected:
	//void CreateQtPartControl(QWidget* parent) override;
	//void NodeAdded(const mitk::DataNode* node) override;
	//void OnTimerOutFunction(QString) override;
	bool AutoTestBreak(int i);
	void ChangeNavigationVisible(bool b);
private:
	//clear all info in scene
	void ClearAllInfo();

	void CloseWindows();
	//隐藏进度条
	void HideProgressBar();
	//save project and jump to patient manager ui
	void JumpToPatientManager();
	void ReturnToPatientManager();
	//save project
	//typedef void FuncType();
	void SaveScene(int type = 0);
	//save patient name
	void SetPatientInfo();
	//显示进度条
	void ShowProgressBar(QString info);
	//读取注册表，获取版本，安装时间等
	static SoftwareInformation GetSoftwareInfo();
	bool m_IsVisible = true;
public:
	void createHelpMenu();
	void createSetMenu();
	void SetLabelSheet(int state, QLabel* lab);
private slots:
	void exitWithSave();
	void ChooseVolume();
	void ChooseVolume2();
	void DoFreshState();
	void RegistrateVolume();
	void PlanPath();
	void GotoOutputPage();
	void OutResult();
	void onLogicModified(vtkObject* caller, vtkObject* obj);
protected:
  QScopedPointer<qSlicerTopMenuModuleWidgetPrivate> d_ptr;

  void setup() override;
  void enter() override;
  void exit() override;
private:
  Q_DECLARE_PRIVATE(qSlicerTopMenuModuleWidget);
  Q_DISABLE_COPY(qSlicerTopMenuModuleWidget);

private:
	QTimer* m_timer;
	QString m_SheetFirstUnselected = "border-image: url(:/Resources/Icons/next.png)";
	QString m_SheetFirstSelected   = "border-image: url(:/Resources/Icons/current.png)";
	QString m_SheetFirstFinished   = "border-image: url(:/Resources/Icons/last.png)";

	QString m_SheetUnselected = "border-image: url(:/Resources/Icons/next.png)";
	QString m_SheetSelected   = "border-image: url(:/Resources/Icons/current.png)";
	QString m_SheetFinished   = "border-image: url(:/Resources/Icons/last.png)";
};

#endif
