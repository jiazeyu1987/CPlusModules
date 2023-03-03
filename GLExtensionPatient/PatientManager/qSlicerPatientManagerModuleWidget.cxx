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
			mw->ShowProgressBar(QStringLiteral("正在打开分析"));

			SetGlobalSaveValue("init_analyse", "2");
			QStringList sl;
			PatientsAnalyse pa;
			pa.patientAnalyseId = QString::fromStdString("207").toInt();
			pa.patientName = QStringLiteral("张家界");
			pa.createTime = QStringLiteral("2022-01-01");
			pa.age = QStringLiteral("32");
			pa.birthday = QStringLiteral("1999-01-02");
			pa.sex = QStringLiteral("女");
			pa.is_package = true;
			pa.remark = QStringLiteral("我们普通老百姓需要缴纳社保是工资收入的接近40%（公司和个人部分一共），缴了一辈子退休了只能拿个两三千。某些既得利益群体自己不用缴纳社保，退休工资大几千甚至上万！所以鼓励老百姓多生娃，以后才有人缴纳社保让他们可以拿高额退休金养老。现在又鼓励自己交养老金了，可怜老百姓自己苦了一辈子，子孙还被人薅羊毛！现在交的社保就是劫贫济富啊");
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

