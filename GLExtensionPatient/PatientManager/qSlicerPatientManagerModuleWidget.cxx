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
 
// Qt includes
#include <QDebug>
#include <QEvent>
#include <QDateTime>
#include <QScreen>
#include <QDir>
 
// Slicer includes
#include "qSlicerPatientManagerModuleWidget.h"
#include "ui_qSlicerPatientManagerModuleWidget.h"

#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerIOManager.h"
#include "QSettings.h"
#include  "qJMainWindow.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPatientManagerModuleWidgetPrivate: public Ui_patientMange
{
public:
  qSlicerPatientManagerModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerPatientManagerModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerPatientManagerModuleWidgetPrivate::qSlicerPatientManagerModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerPatientManagerModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerPatientManagerModuleWidget::qSlicerPatientManagerModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerPatientManagerModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerPatientManagerModuleWidget::~qSlicerPatientManagerModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerPatientManagerModuleWidget::setup()
{
  Q_D(qSlicerPatientManagerModuleWidget);
  d->setupUi(this);
  pmv = new patientMangeView(*d, this);
  this->Superclass::setup();
  connect(pmv, &patientMangeView::sig_nextPage, this, &qSlicerPatientManagerModuleWidget::onGotoNextPage);
}

void qSlicerPatientManagerModuleWidget::enter() {
	this->Superclass::enter();
	Q_D(qSlicerPatientManagerModuleWidget);
	QTimer::singleShot(300, [=]() {
		const char* clear = GetGlobalSaveValue("clear");
		if (clear && strcmp(clear, "true") == 0)
		{
			DoReinitScene();
			SetGlobalSaveValue("filter_clear", "true");
			SetGlobalSaveValue("filter_top_clear", "true");
			SetGlobalSaveValue("register_clear", "true");
			SetGlobalSaveValue("navi_clear", "true");
			SetGlobalSaveValue("navigation_clear", "true");
		}
		qJMainWindow* mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
		mw->HideProgressBar();

		QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
		bool JTest = set.value("PatientManager/skip_patient_manager").toString() == "2";
		if (JTest) {
			mw->ShowProgressBar(QStringLiteral("���ڴ򿪷���"));

			SetGlobalSaveValue("init_analyse", "2");
			QStringList sl;
			PatientsAnalyse pa;
			pa.patientAnalyseId = QString::fromStdString("207").toInt();
			pa.patientName = QStringLiteral("�żҽ�");
			pa.createTime = QStringLiteral("2022-01-01");
			pa.age = QStringLiteral("32");
			pa.birthday = QStringLiteral("1999-01-02");
			pa.sex = QStringLiteral("Ů");
			pa.is_package = true;
			pa.remark = QStringLiteral("������ͨ�ϰ�����Ҫ�����籣�ǹ�������Ľӽ�40%����˾�͸��˲���һ����������һ����������ֻ���ø�����ǧ��ĳЩ�ȵ�����Ⱥ���Լ����ý����籣�����ݹ��ʴ�ǧ�����������Թ����ϰ��ն����ޣ��Ժ�����˽����籣�����ǿ����ø߶����ݽ����ϡ������ֹ����Լ������Ͻ��ˣ������ϰ����Լ�����һ���ӣ����ﻹ����޶��ë�����ڽ����籣���ǽ�ƶ�ø���");
			//pmv->load_package_file(198);
			pmv->finished_success(sl, pa);
			return true;
		}

		});

}

void qSlicerPatientManagerModuleWidget::DoReinitScene() {
	mrmlScene()->Clear();
	
}

void qSlicerPatientManagerModuleWidget::exit() {
	this->Superclass::exit();
	Q_D(qSlicerPatientManagerModuleWidget);
}

void qSlicerPatientManagerModuleWidget::onGotoNextPage() {
	AddSignalNode("fresh_top_name", "1");
	auto cn = mrmlScene()->AddNewNodeByClass("vtkMRMLColorNode");
	cn->SetAttribute("value", "3");
	mrmlScene()->InvokeEvent(1000005, cn);
	mrmlScene()->RemoveNode(cn);
}

void qSlicerPatientManagerModuleWidget::onGotoPreviewPage() {
	//qSlicerApplication::application()->layoutManager()->setCurrentPage(0);
}

