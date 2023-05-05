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
#include "qdebug.h"

// Slicer includes
#include "qSlicerFilterLeftModuleWidget.h"
#include "ui_qSlicerFilterLeftModuleWidget.h"

#include "vtkSlicerFilterLeftLogic.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "dicom.h"
#include "qGLMessageDialog.h"
#include "qSlicerDataDialog.h"
#include "PythonQt.h"
#include "QFileDialog.h"
#include "qfileinfo.h"
#include "qSlicerIOManager.h"
#include "qJMainWindow.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFilterLeftModuleWidgetPrivate : public Ui_qSlicerFilterLeftModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerFilterLeftModuleWidget);
protected:
	qSlicerFilterLeftModuleWidget* const q_ptr;
public:
	qSlicerFilterLeftModuleWidgetPrivate(qSlicerFilterLeftModuleWidget& object);
	~qSlicerFilterLeftModuleWidgetPrivate();
	vtkSlicerFilterLeftLogic* logic() const;

	void Init();
	//set info of the selected node
	void SetNodeInfo();
	void CreatPicTextButton(QPushButton* btn, QString picPath, QString text);
};

//-----------------------------------------------------------------------------
// qSlicerFilterLeftModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerFilterLeftModuleWidgetPrivate::qSlicerFilterLeftModuleWidgetPrivate(qSlicerFilterLeftModuleWidget& object) : q_ptr(&object)
{
}
qSlicerFilterLeftModuleWidgetPrivate::~qSlicerFilterLeftModuleWidgetPrivate() {}

vtkSlicerFilterLeftLogic* qSlicerFilterLeftModuleWidgetPrivate::logic() const {
	Q_Q(const qSlicerFilterLeftModuleWidget);
	return vtkSlicerFilterLeftLogic::SafeDownCast(q->logic());
}

void qSlicerFilterLeftModuleWidgetPrivate::Init()
{
	Q_Q(qSlicerFilterLeftModuleWidget);
	this->setupUi(q);
	CreatPicTextButton(this->btn_delete, ":/Icons/remove.png", QStringLiteral("�Ƴ�ͼ��"));
	CreatPicTextButton(this->btn_import, ":/Icons/add.png", QStringLiteral("����ͼ��"));
	CreatPicTextButton(this->btn_dti, ":/Icons/add.png", QStringLiteral("����DTI"));
}

void qSlicerFilterLeftModuleWidgetPrivate::SetNodeInfo()
{
	/*lblLayerNumber->setText(QString::fromStdString(logic()->layerNumber));
	lblScanType->setText(QString::fromStdString(logic()->scanType));
	lblSerialNo->setText(QString::fromStdString(logic()->serialNumber));
	lblSerialDes->setText(QString::fromStdString(logic()->serialDes));
	lblTestDate->setText(QString::fromStdString(logic()->testDate));*/
}

void qSlicerFilterLeftModuleWidgetPrivate::CreatPicTextButton(QPushButton* btn, QString picPath, QString text)
{
	QLabel* labelPic = new QLabel;
	QLabel* labelText = new QLabel;
	labelPic->setPixmap(QPixmap(picPath));
	labelPic->setStyleSheet("background: #525F7B;border-radius: 4px 4px 4px 4px;");
	labelText->setText(text);
	labelText->setStyleSheet("font: 20px 'Source Han Sans CN-Regular, Source Han Sans CN';");
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addSpacing(2);
	layout->addWidget(labelPic);
	layout->addSpacing(2);
	layout->addWidget(labelText);
	layout->addStretch();
	btn->setLayout(layout);
	//btn->setStyleSheet("background: #525F7B;");
}
//-----------------------------------------------------------------------------
// qSlicerFilterLeftModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerFilterLeftModuleWidget::qSlicerFilterLeftModuleWidget(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSlicerFilterLeftModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerFilterLeftModuleWidget::~qSlicerFilterLeftModuleWidget()
{
	Q_D(qSlicerFilterLeftModuleWidget);
}

void qSlicerFilterLeftModuleWidget::OnNodeSelected(vtkObject* appLogic, void* data, unsigned long event)
{
	Q_D(qSlicerFilterLeftModuleWidget);
	d->SetNodeInfo();
}


//-----------------------------------------------------------------------------
void qSlicerFilterLeftModuleWidget::setup()
{
	Q_D(qSlicerFilterLeftModuleWidget);
	this->Superclass::setup();
	d->Init();

	qvtkConnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified()));
	QObject::connect(d->btn_import, &QPushButton::clicked, this, &qSlicerFilterLeftModuleWidget::onLoadPatientFiles);
	QObject::connect(d->btn_delete, &QPushButton::clicked, this, &qSlicerFilterLeftModuleWidget::OnRemoveClick);
	QObject::connect(d->btn_dti, &QPushButton::clicked, this, &qSlicerFilterLeftModuleWidget::onLoadDTI);
	d->btn_dti->hide();
}

void qSlicerFilterLeftModuleWidget::onLoadDTI() {
	Q_D(qSlicerFilterLeftModuleWidget);
	auto node = mrmlScene()->GetFirstNodeByName("dti_node");
	if (node) {
		qGLMessageDialog::ShowWarnning(QStringLiteral("����ɾ�����е�DWIͼ��"));
		return;
	}

	QString filePath;
	/*bool JTest = qSlicerApplication::application()->settings->value("boot_config/FixedPathImportTest").toString() == "2";
	if (JTest) {
		filePath = QStringLiteral("D:/��������/error/DTI");
	}
	else {
		filePath = QFileDialog::getExistingDirectory();
	}*/
	filePath = QStringLiteral("F:/DTI");
	if (filePath.isEmpty())
		return;
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	mw->ShowProgressBar(QStringLiteral("���ڵ���DTI"));
	PythonQt::init();
	PythonQtObjectPtr context = PythonQt::self()->getMainModule();
	mw->SetProgress(30);
	context.evalScript(QString("import slicer.util as util; dti_logic = util.getModuleLogic('GLPyDTI'); dti_logic.load_dti_folder('%1')").arg(filePath));
	
	mw->SetProgress(100);
}

void qSlicerFilterLeftModuleWidget::onLoadPatientFiles()
{
	QString filePath;
	/*bool JTest = qSlicerApplication::application()->settings->value("boot_config/FixedPathImportTest").toString() == "2";
	if (JTest) {
		filePath = QStringLiteral("D:/��������/cyh");
	}
	else {
		filePath = QFileDialog::getExistingDirectory();
	}*/
	filePath = QStringLiteral("D:/cyh");
	if (filePath.isEmpty())
		return;
	AddSignalNode("load_data_from_filter", filePath.toStdString().c_str());

	
	
}

void qSlicerFilterLeftModuleWidget::OnRemoveClick()
{
	AddSignalNode("remove_left_filter_node", "1");
}


void qSlicerFilterLeftModuleWidget::onLogicModified()
{
	Q_D(qSlicerFilterLeftModuleWidget);

	vtkSlicerFilterLeftLogic* logic = d->logic();
	QString info = "";
	info = info + QStringLiteral("ɨ������:") + QString::fromStdString(logic->scanType);
	info = info + "\t";
	info = info + QStringLiteral("����:") + QString::fromStdString(logic->layerNumber);
	info = info + "\t";
	info = info + QStringLiteral("���к�:") + QString::fromStdString(logic->serialNumber);
	info = info + "\t";
	info = info + QStringLiteral("ɨ��ʱ��:") + QString::fromStdString(logic->AcquisitionDateTime);
	info = info + "\t";
	info = info + QStringLiteral("ɨ������:") + QString::fromStdString(logic->patientName);
	d->label_info->setText(info);
	
}

