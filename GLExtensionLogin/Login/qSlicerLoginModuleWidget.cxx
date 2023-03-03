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
#include "qSlicerLoginModuleWidget.h"
#include "ui_qSlicerLoginModuleWidget.h"
#include <QSettings>
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "QFile.h"
#include "itkDOMNodeXMLReader.h"
#include "itkDOMNode.h"

#include "qSlicerModuleManager.h"
#include "qSlicerModuleFactoryManager.h"
#include "qSlicerIOManager.h"
#include "QThread.h"
#include "qJMainWindow.h"
#include <PythonQt.h>
#include "ThreadWorker.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLoginModuleWidgetPrivate: public Ui_qSlicerLoginModuleWidget
{
public:
  qSlicerLoginModuleWidgetPrivate();

private:
	
};

//-----------------------------------------------------------------------------
// qSlicerLoginModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerLoginModuleWidgetPrivate::qSlicerLoginModuleWidgetPrivate()
{
	
}

//-----------------------------------------------------------------------------
// qSlicerLoginModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerLoginModuleWidget::qSlicerLoginModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerLoginModuleWidgetPrivate )
{
	
}

//-----------------------------------------------------------------------------
qSlicerLoginModuleWidget::~qSlicerLoginModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLoginModuleWidget::setup()
{
  Q_D(qSlicerLoginModuleWidget);
  //d->setupUi(this);
  login = new ZZLoginView(*d, this);
  this->Superclass::setup();


  connect(login, &ZZLoginView::sig_nextPage , this, &qSlicerLoginModuleWidget::onGotoNextPage);

  d->pushButton_2->hide();
  d->pushButton_2->animateClick(10);
  connect(d->pushButton_2, &QPushButton::clicked, this, [=]() {
	  QString path;
	  QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	  bool JTest = set.value("Login/skip_login").toString() == "2";
	  if (JTest) {
		  onGotoNextPage();
	  }
	  });
  
}

void qSlicerLoginModuleWidget::LoadVolumeAsync(QString path, int type) {
	/*std::cout << "LoadVolumeAsync" << std::endl;
	auto qthread1 = new QThread(this);
	PythonQt::init();
	PythonQtObjectPtr context = PythonQt::self()->getMainModule();
	this->moveToThread(qthread1);
	context.evalScript(QString(
		"import slicer.util as util;widget = util.getModuleWidget('%1'); widget.SetPage(%2);").arg(ProjectName).arg(i))*/
	QThread* workerThread = new QThread(this);

		auto* worker = new ThreadWorker;
		
	// 调用 moveToThread 将该任务交给 workThread
	worker->moveToThread(workerThread);

	// operate 信号发射后启动线程工作
	connect(this, SIGNAL(operate(QString,int)), worker, SLOT(doWork(QString,int)));

	// 该线程结束时销毁
	connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);

	// 线程结束后发送信号，对结果进行处理
	connect(worker, SIGNAL(resultReady(int)), this, SLOT(handleResults(int)));
	
	// 启动线程
	workerThread->start();

	// 发射信号，开始执行
	qDebug() << "emit the signal to execute!";
	qDebug() << "\tCurrent thread ID:" << QThread::currentThreadId() << '\n';
	
	emit operate("D:/CT2.nrrd",1);
}

void qSlicerLoginModuleWidget::handleResults(const int result)
{
	qDebug() << "receive the resultReady signal";
	qDebug() << "\tCurrent thread ID: " << QThread::currentThreadId() << '\n';
	qDebug() << "\tThe last result is: " << result;
}


void qSlicerLoginModuleWidget::enter() {
	this->Superclass::enter();
	
	Q_D(qSlicerLoginModuleWidget);

	d->lineEdit_USER->setText("administrator");

	d->lineEdit_PASSWORD->setText("1234567890");
}

void qSlicerLoginModuleWidget::exit() {
	this->Superclass::exit();
	
}


void qSlicerLoginModuleWidget::onGotoNextPage() {
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	bool JTest = set.value("Login/skip_login").toString() == "2";
	if (JTest) {
		auto location = set.value("Login/skip_login_location").toString();
		auto cn = mrmlScene()->GetFirstNodeByClass("vtkMRMLColorNode");
		cn->SetAttribute("value", location.toStdString().c_str());
		mrmlScene()->InvokeEvent(1000005, cn);
		return;
	}
	auto cn = mrmlScene()->GetFirstNodeByClass("vtkMRMLColorNode");
	cn->SetAttribute("value", "2");
	mrmlScene()->InvokeEvent(1000005, cn);
} 


void qSlicerLoginModuleWidget::AutoClearUiData()
{
	Q_D(qSlicerLoginModuleWidget);

	d->lineEdit_USER->setText("");

	d->lineEdit_PASSWORD->setText("");

    d->checkUserAgreement->setChecked(false);
	
	login->hideLoginStatusLabel();
}
