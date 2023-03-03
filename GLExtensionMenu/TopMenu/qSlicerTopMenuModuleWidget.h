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
//�����������Ķ�����#include "qGLTool.cpp"��mitk�����˵�������GLTool.cpp
typedef struct
{
	QString versionRelase; //���а汾��
	QString versionComplete; //�����汾��
	QString installationDateTime; //��װʱ��
	QString registerName = QStringLiteral("���ݼ���ҽ�ƿƼ����޹�˾"); //ע��������
	QString address = QStringLiteral("�㽭ʡ�������ຼ����ǰ�ֵ����˽�1390��9��B��801��"); //��ַ
	QString contact = "(86)0571-88730192"; //��ϵ��ʽ
	QString copyright = QStringLiteral("Copyright &#169; 2021���ݼ���ҽ�ƿƼ����޹�˾. All Rights Reserved");
	QString licenseKey; //���֤��
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
	//���ؽ�����
	void HideProgressBar();
	//save project and jump to patient manager ui
	void JumpToPatientManager();
	void ReturnToPatientManager();
	//save project
	//typedef void FuncType();
	void SaveScene(int type = 0);
	//save patient name
	void SetPatientInfo();
	//��ʾ������
	void ShowProgressBar(QString info);
	//��ȡע�����ȡ�汾����װʱ���
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
