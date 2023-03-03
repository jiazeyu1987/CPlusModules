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
#include <QMenu>
#include <QMessageBox>
#include "qGLMessageDialog.h"
#include <qfile.h>
#include <qdir.h>
#include <qdesktopservices.h>

// Slicer includes
#include "qSlicerTopMenuModuleWidget.h"
#include "ui_qSlicerTopMenuModuleWidget.h"
#include "qGLChangePassword.h"
#include "qGLDb.h"
#include "qGLRegisterAccount.h"
#include "qSlicerIOManager.h"
#include "qSlicerIO.h"
#include "qSlicerCoreIOManager.h"
#include "qSlicerMainWindow.h"
#include "itksys/SystemTools.hxx"

#include "vtkSlicerTopMenuLogic.h"
#include "qapplication.h"
#include "itkDOMNodeXMLReader.h"
#include "qJMainWindow.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerTopMenuModuleWidgetPrivate : public Ui_qSlicerTopMenuModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerTopMenuModuleWidget);
protected:
	qSlicerTopMenuModuleWidget* const q_ptr;
public:
	qSlicerTopMenuModuleWidgetPrivate(qSlicerTopMenuModuleWidget& object);
	~qSlicerTopMenuModuleWidgetPrivate();
	void Init();
	void InitToolBar();
	void InitToolButton(QToolButton* btn, QString text, QSize size, QString iconStr);

};

//-----------------------------------------------------------------------------
// qSlicerTopMenuModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerTopMenuModuleWidgetPrivate::qSlicerTopMenuModuleWidgetPrivate(qSlicerTopMenuModuleWidget& object) :q_ptr(&object)
{
}
qSlicerTopMenuModuleWidgetPrivate::~qSlicerTopMenuModuleWidgetPrivate()
{

}
void qSlicerTopMenuModuleWidgetPrivate::Init()
{
	Q_Q(qSlicerTopMenuModuleWidget);
	this->setupUi(q);
	InitToolBar();
	QObject::connect(saveBtn, &QToolButton::clicked, q, &qSlicerTopMenuModuleWidget::OnSaveBtn);
	//QObject::connect(settingBtn, &QToolButton::clicked, q, &qSlicerTopMenuModuleWidget::OnSettingBtn);
	//QObject::connect(helpBtn, &QToolButton::clicked, q, &qSlicerTopMenuModuleWidget::OnHelpBtn);
	QObject::connect(exitAnalyseBtn, &QToolButton::clicked, q, &qSlicerTopMenuModuleWidget::OnExitAnalyseBtn);
	/*action_exit->setShortcut(QKeySequence("Alt+F4"));
	exitBtn->setDefaultAction(action_exit);
	QObject::connect(this->action_exit, SIGNAL(triggered()),
		q, SLOT(OnExitBtn()));*/
	QObject::connect(exitBtn, &QToolButton::clicked, q, &qSlicerTopMenuModuleWidget::OnExitBtn);
	label->setVisible(false);
	lblPatientIcon->setVisible(false);
	lblPatientInfo->setVisible(false);
}
void qSlicerTopMenuModuleWidgetPrivate::InitToolButton(QToolButton* btn, QString text, QSize size, QString iconStr)
{
	QString qss("QToolButton::menu-indicator{image: none;}; ");
	btn->setStyleSheet(qss);
	btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	QIcon icon(QString(":/Resources/Icons/%1").arg(iconStr));
	btn->setIcon(icon);
	btn->setText(text);
	btn->setIconSize(size);
}
void qSlicerTopMenuModuleWidgetPrivate::InitToolBar()
{
	Q_Q(qSlicerTopMenuModuleWidget);
	toolBar->setFixedSize(680, 50);

	InitToolButton(saveBtn, QStringLiteral("保存"), QSize(26, 30), "topmenu_save.png");
	InitToolButton(settingBtn, QStringLiteral("设置"), QSize(26, 30), "topmenu_setting.png");
	InitToolButton(helpBtn, QStringLiteral("帮助"), QSize(26, 26), "topmenu_help.png");
	InitToolButton(exitAnalyseBtn, QStringLiteral("退出分析"), QSize(24, 24), "topmenu_exit.png");
	InitToolButton(exitBtn, QStringLiteral("退出"), QSize(24, 24), "topmenu_quit.png");
}
//-----------------------------------------------------------------------------
// qSlicerTopMenuModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerTopMenuModuleWidget::qSlicerTopMenuModuleWidget(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSlicerTopMenuModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerTopMenuModuleWidget::~qSlicerTopMenuModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerTopMenuModuleWidget::setup()
{
	Q_D(qSlicerTopMenuModuleWidget);
	this->Superclass::setup();

	d->Init();
	SetPatientInfo();
	createSetMenu();
	createHelpMenu();

	m_timer = new QTimer(this);
	m_timer->stop();
	m_timer->setInterval(500);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(Ontimeout()));

	/*auto settings = qSlicerApplication::application()->settings;
	QString auto_test = settings->value("boot_config/auto_test").toString();
	int show_data = settings->value("boot_config/show_data").toInt();
	if (show_data == 1) {
		qSlicerApplication::application()->layoutManager()->ShowData = true;
	}
	else {
		qSlicerApplication::application()->layoutManager()->ShowData = false;
	}*/

	//d->btnChoose->setStyleSheet(m_SheetFirstUnselected);
	//d->btnRegistration->setStyleSheet(m_SheetUnselected);
	//d->btnPlanning->setStyleSheet(m_SheetUnselected);
	//d->btnOutput->setStyleSheet(m_SheetUnselected);
	SetLabelSheet(0, d->label_2);
	SetLabelSheet(0, d->label_3);
	SetLabelSheet(0, d->label_4);
	SetLabelSheet(0, d->label_5);
	connect(d_ptr->btnChoose, SIGNAL(clicked()), this, SLOT(ChooseVolume2()));
	connect(d_ptr->btnRegistration, SIGNAL(clicked()), SLOT(RegistrateVolume()));
	connect(d_ptr->btnPlanning, SIGNAL(clicked()), this, SLOT(PlanPath()));
	connect(d_ptr->btnOutput, SIGNAL(clicked()), this, SLOT(GotoOutputPage()));
	ChangeNavigationVisible(false);
	

}

void qSlicerTopMenuModuleWidget::enter() {
	Q_D(qSlicerTopMenuModuleWidget);
	this->Superclass::enter();
	auto logic1 = vtkSlicerTopMenuLogic::SafeDownCast(logic());
	qvtkReconnect(logic1, vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
	
	
	
	
	qSlicerApplication* app = qSlicerApplication::application();
	if (!app) return;
	qSlicerLayoutManager* layoutManager = app->layoutManager();
	if (!layoutManager) return;
	std::vector<vtkMRMLNode*> viewNodes;
	mrmlScene()->GetNodesByClass("vtkMRMLAbstractViewNode", viewNodes);
	int count = layoutManager->threeDViewCount();
	for (int i = 0; i < count; i++) {
		auto widget = layoutManager->threeDWidget(i);
		auto controller = widget->threeDController();
		controller->setBlackBackground();
	}
	for (std::vector< vtkMRMLNode* >::iterator sliceNodeIt = viewNodes.begin(); sliceNodeIt != viewNodes.end(); ++sliceNodeIt)
	{
		vtkMRMLViewNode* vnode = vtkMRMLViewNode::SafeDownCast(*sliceNodeIt);
		if (!vnode)continue;
		vnode->SetLinkedControl(1);
		vnode->SetAxisLabelsVisible(false);
		vnode->SetBoxVisible(false);
	}

}

void qSlicerTopMenuModuleWidget::exit() {
	this->Superclass::exit();
	Q_D(qSlicerTopMenuModuleWidget);
	auto logic1 = vtkSlicerTopMenuLogic::SafeDownCast(logic());
	qvtkDisconnect(logic1, vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
}

void qSlicerTopMenuModuleWidget::ChooseVolume()
{
	if (true) {
		auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
		mw->SetPage(2);
		//DoFreshState();
//		SetLabelSheet(1, d_ptr->label_2);
	}

}

void qSlicerTopMenuModuleWidget::ChooseVolume2() {
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	mw->SetPage(2);
	int i = 1;
	DoFreshState();
	int j = 1;
	SetLabelSheet(1, d_ptr->label_2);
	int k = 1;
}

void qSlicerTopMenuModuleWidget::RegistrateVolume()
{
	if (true) {
		auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
		mw->SetPage(3);
		DoFreshState();
		SetLabelSheet(1, d_ptr->label_3);
	}
}


void qSlicerTopMenuModuleWidget::GotoOutputPage()
{
	if (true) {
		auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
		mw->SetPage(5);
		DoFreshState();
		SetLabelSheet(1, d_ptr->label_5);
	}
}

void qSlicerTopMenuModuleWidget::PlanPath()
{
	if (true) {
		auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
		mw->SetPage(4);
		DoFreshState();
		SetLabelSheet(1, d_ptr->label_4);
	}
}

void qSlicerTopMenuModuleWidget::OutResult()
{
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	mw->SetPage(5);
}

void qSlicerTopMenuModuleWidget::exitWithSave() {
	SaveScene(2);
	qSlicerApplication::application()->closeAllWindows();
}

void qSlicerTopMenuModuleWidget::onLogicModified(vtkObject* vtkNotUsed(caller), vtkObject* callData)
{
	Q_D(qSlicerTopMenuModuleWidget);

	// the call data should be the annotation hierarchy node that was just
	// added, passed along from the logic
	if (callData == nullptr)
	{
		return;
	}
	vtkMRMLColorNode* node = nullptr;
	node = reinterpret_cast<vtkMRMLColorNode*>(callData);
	if (node == nullptr)
	{
		return;
	}
	auto help_node = node->GetAttribute("Help_Node");
	auto type = node->GetAttribute("Help_Name");
	auto value = node->GetAttribute("Help_Info_0");
	SafeRemove(node);
	if (help_node && strcmp(help_node, "true") == 0) {

		if (type && value) {
			if (strcmp(type, "OnExitAnalyseBtn") == 0) {
				OnExitAnalyseBtn();
				return;
			}
			if (strcmp(type, "OnExitBtn") == 0) {
				OnExitBtn();
				return;
			}
			if (strcmp(type, "exit_with_save") == 0) {
				exitWithSave();
				return;
			}
			if (strcmp(type, "setMainModule") == 0) {
				//auto window = dynamic_cast<qSlicerMainWindow*>(qSlicerApplication::application()->mainWindow());
				//window->setMainModule(value);
				return;
			}
			if (strcmp(type, "fresh_top_name") == 0) {
				auto titlename = GetGlobalSaveValue("analyseName");
				if (titlename == nullptr || std::string(titlename).length() == 0) {
					auto patientName = GetGlobalSaveValue("patientName");
					QString q_name = QString::fromLocal8Bit(patientName);
					auto fname = q_name.split(":")[0];
					d->lblPatientInfo->setText(fname);
				}
				else {
					d->lblPatientInfo->setText(QString::fromLocal8Bit(titlename));
				}
				return;
			}
			if (strcmp(type, "goto_page") == 0) {
				qInfo() << "GoToPage:" << value;
				if (value && strcmp(value, "2") == 0) {
					ChangeNavigationVisible(true);
					ChooseVolume();
				}
				if (value && strcmp(value, "3") == 0) {
					ChangeNavigationVisible(true);
					RegistrateVolume();
				}
				if (value && strcmp(value, "4") == 0) {
					ChangeNavigationVisible(true);
					PlanPath();
				}
				if (value && strcmp(value, "5") == 0) {
					ChangeNavigationVisible(true);
					GotoOutputPage();
				}
			}
			if (strcmp(type, "fresh_navigation") == 0) {
				DoFreshState();
			}
			if (strcmp(type, "Reinit") == 0) {
				Reinit();
			}
		}
	}
}

void qSlicerTopMenuModuleWidget::DoFreshState() {
	Q_D(qSlicerTopMenuModuleWidget);
	auto filter_set = GetGlobalSaveValue("filter_set");
	auto register_set = GetGlobalSaveValue("register_set");
	auto navi_set = GetGlobalSaveValue("navi_set");
	auto output_set = GetGlobalSaveValue("output_set");
	if (filter_set && strcmp(filter_set, "1") == 0) {
		SetLabelSheet(2, d_ptr->label_2);
	}
	if (register_set && strcmp(register_set, "1") == 0) {
		SetLabelSheet(2, d_ptr->label_3);
	}
	if (navi_set && strcmp(navi_set, "1") == 0) {
		SetLabelSheet(2, d_ptr->label_4);
	}
	if (output_set && strcmp(output_set, "1") == 0) {
		SetLabelSheet(2, d_ptr->label_5);
	}
	if (!filter_set) {
		SetLabelSheet(0, d_ptr->label_2);
	}
	if (!register_set) {
		SetLabelSheet(0, d_ptr->label_3);
	}
	if (!navi_set) {
		SetLabelSheet(0, d_ptr->label_4);
	}
	if (!output_set) {
		SetLabelSheet(0, d_ptr->label_5);
	}
}

void qSlicerTopMenuModuleWidget::SetLabelSheet(int state, QLabel* lab)
{
	Q_D(qSlicerTopMenuModuleWidget);
	// 0 初始状态    1 选中状态   2 完成状态

	switch (state)
	{
	case 0:
	{
		lab->setStyleSheet(m_SheetUnselected);
		break;
	}
	case 1:
	{
		lab->setStyleSheet(m_SheetSelected);
		break;
	}
	case 2:
	{
		lab->setStyleSheet(m_SheetFinished);
		break;
	}
	default:
		break;
	}
}

//void qSlicerTopMenuModuleWidget::CreateQtPartControl(QWidget* parent)
//{
//	Q_D(qSlicerTopMenuModuleWidget);
//	d->setupUi(parent);
//	//SetDelayFunction(500, "");
//	qSlicerTopMenuTool::topToolBar(1200, 3, 500, 64, false, parent);
//	connect(qSlicerTopMenuTool::saveBtn, &QToolButton::clicked, [=]() {	 SaveScene(); });
//	connect(qSlicerTopMenuTool::exitBtn1, &QToolButton::clicked, [=]() { JumpToPatientManager(); });
//	connect(qSlicerTopMenuTool::exitBtn, &QToolButton::clicked, this, &qSlicerTopMenuModuleWidget::OnCloseClick);
//	SetPatientInfo();
//	//progressDialog = new GLCustomDialog();
//}
//void qSlicerTopMenuModuleWidget::NodeAdded(const mitk::DataNode* node)
//{
//	std::string nodeName = node->GetName();
//	if (nodeName == "set_basic_info") {
//		GetDataStorage()->Remove(node);
//		std::string tmpName;
//		node->GetStringProperty("value", tmpName);
//
//		QString tmpQName = QString::fromStdString(tmpName);
//		QStringList list = tmpQName.split(":");
//		SetGlobalValue("Patient_Name", list[0].toStdString());
//		m_Controls.lblPatientInfo->setText(list[0]);
//		return;
//	}
//	if (nodeName == "show_progress_bar") {
//		std::string info = "";
//		node->GetStringProperty("value", info);
//		GetDataStorage()->Remove(node);
//
//		ShowProgressBar(QString::fromStdString(info));
//		return;
//	}
//	if (nodeName == "hide_progress_bar") {
//		GetDataStorage()->Remove(node);
//		HideProgressBar();
//		return;
//	}
//}

//void qSlicerTopMenuModuleWidget::OnTimerOutFunction(QString)
//{
//}

void qSlicerTopMenuModuleWidget::ClearAllInfo()
{
	/*AddSignal("clearToolView");
	AddSignal("clearBeforeOperateRegisterInfo");
	SetGlobalValue("Patient_Name", "");
	m_Controls.lblPatientInfo->setText("");
	auto list = GetDataStorage()->GetAll();
	auto iter = list->begin();
	for (auto iter = list->begin(); iter != list->end(); ++iter) {
		GetDataStorage()->Remove(*(iter));
	}*/

	vtkMRMLNode* node = mrmlScene()->GetFirstNodeByName("saveconfig");
	if (!node) {
		return;
	}
	mrmlScene()->RemoveNode(node);

	SetGlobalSaveValue("clear", "true");
}

void qSlicerTopMenuModuleWidget::CloseWindows()
{
}

void qSlicerTopMenuModuleWidget::HideProgressBar()
{
	//mitk::ProgressBar::GetInstance()->Progress(2);
	//progressDialog->setText("");
	//progressDialog->hide();
}


void qSlicerTopMenuModuleWidget::ReturnToPatientManager()
{
	ClearAllInfo();
	/*berry::PlatformUI::GetWorkbench()->ShowPerspective("org.gl.zz_patientMange_perspective",
		berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow());*/
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	mw->SetPage(1);
	ChangeNavigationVisible(false);
}

void qSlicerTopMenuModuleWidget::ChangeNavigationVisible(bool b) {
	Q_D(qSlicerTopMenuModuleWidget);
	d->btnChoose->setVisible(b);
	d->btnRegistration->setVisible(b);
	d->btnPlanning->setVisible(b);
	d->btnOutput->setVisible(b);
	d->label_2->setVisible(b);
	d->label_3->setVisible(b);
	d->label_4->setVisible(b);
	d->label_5->setVisible(b);
}

void qSlicerTopMenuModuleWidget::OnExitAnalyseBtn()
{
	qDebug() << "OnExitAnalyseBtn";
	QStringList tmpList = { QStringLiteral("保存"), QStringLiteral("取消") , QStringLiteral("不保存") };
	MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("退出分析"), QStringLiteral("是否退出分析, 并保存此次分析?"), MessageButtonType::BUTTON_OK_AND_ACCEPT, tmpList);
	if (res == MessageResult::OK) {
		SaveScene(2);
		ReturnToPatientManager();
	}
	else if (res == MessageResult::ACCEPT) {
		ReturnToPatientManager();
	}
	else {
		int i = 1;
	}

}

void qSlicerTopMenuModuleWidget::Ontimeout()
{
	Q_D(qSlicerTopMenuModuleWidget);
	/*int curValue = GLTool::progressBar->getProgressValue();
	if (curValue >99)
	{
		GLTool::progressBar->hide();
		m_timer->stop();
		return;
	}
	GLTool::progressBar->setProgressValue(curValue+18);*/
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	mw->HideProgressBar();
	m_timer->stop();
}

void qSlicerTopMenuModuleWidget::SaveScene(int type)
{
	const char* c_id = GetGlobalSaveValue("patientAnalyseId");
	if (!c_id) {
		c_id = "tmp";
	}
	auto nodes = mrmlScene()->GetNodes();
	for (int n = 0; n < nodes->GetNumberOfItems(); n++)
	{
		vtkMRMLNode* node = (vtkMRMLNode*)nodes->GetItemAsObject(n);
		auto at1 = node->GetAttribute("Help_Node");
		auto at2 = node->GetAttribute("Help_Name");
		if (at1 && at2) {
			mrmlScene()->RemoveNode(node);
		}
	}
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	mw->ShowProgressBar(QStringLiteral("正在保存"));

	int index = QString::fromLatin1(c_id).toInt();
	QString curDirPath = QCoreApplication::applicationDirPath();
	QString backupPath = QStringLiteral("%1/%2").arg(curDirPath).arg("bak");
	itksys::SystemTools::MakeDirectory(backupPath.toLatin1());
	QString fileName = QStringLiteral("%1/%2.mrb").arg(backupPath).arg(index);

	mw->SetProgress(28);
	qApp->processEvents();
	if (fileName != "")
	{
		if (type == 2) {
			qSlicerCoreIOManager* coreIOManager = qSlicerCoreApplication::application()->coreIOManager();
			qSlicerIO::IOProperties fileParameters;
			fileParameters["fileName"] = fileName;
			QWidget* widget = qSlicerApplication::application()->layoutManager()->viewport();
			coreIOManager->saveNodes("SceneFile", fileParameters); //此函数比较耗时
			m_timer->start();
		}
		else {
			mw->m_GLFullPathFileName = fileName;
			qSlicerApplication::application()->ioManager()->openSaveDataDialog();
			return;
		}

	}
	else
	{
		std::cout << "Error Saving Scene : no filename" << std::endl;
		return;
	}

	//更新打开的新建分析的修改时间
	GLDb db;
	if (!db.createDbConnect())
	{
		GLMessageBox msg;
		msg.queRenWarn(QStringLiteral("保存分析"), QStringLiteral("打开数据库失败，将不能更新当前分析的时间！"));
	}
	else
	{
		PatientsAnalyse pa;
		const char* id1 = GetGlobalSaveValue("patientAnalyseId");
		if (id1) {
			pa.patientAnalyseId = QString::fromLatin1(id1).toInt();
		}
		db.setPatientAnalyseChangedTime(pa);
	}
	//GLTool::progressBar->setProgressValue(100);
	//GLTool::progressBar->hide();
	
}

void qSlicerTopMenuModuleWidget::SetPatientInfo()
{
	Q_D(qSlicerTopMenuModuleWidget);
	//std::string tmpName = GetGlobalValue("Patient_Name");
	//QString tmpQName = QString::fromStdString(tmpName);
	d->lblPatientInfo->setText(QString("PatientName"));
}

void qSlicerTopMenuModuleWidget::ShowProgressBar(QString info)
{
	//progressDialog->show();
	//progressDialog->showFullScreen();
	//progressDialog->setText(info);
	//progressDialog->GetProgressBar()->show();
	//progressDialog->GetProgressBar()->AddStepsToDo(1);
}

void qSlicerTopMenuModuleWidget::OnExitBtn()
{
	QStringList tmpList = { QStringLiteral("保存"), QStringLiteral("取消") , QStringLiteral("不保存") };
	MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("退出应用"), QStringLiteral("是否退出应用, 并保存此次分析?"), MessageButtonType::BUTTON_OK_AND_ACCEPT, tmpList);
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	if (res == MessageResult::OK) {
		mw->close_flag = true;
		SaveScene(2);
		qSlicerApplication::application()->closeAllWindows();
	}
	else if (res == MessageResult::ACCEPT) { 
		mw->close_flag = true;
		QTimer::singleShot(0, qSlicerApplication::application(), SLOT(closeAllWindows()));
	}

}

void qSlicerTopMenuModuleWidget::OnSaveBtn()
{
	SaveScene(2);
}

void qSlicerTopMenuModuleWidget::OnSettingBtn()
{

}

void qSlicerTopMenuModuleWidget::OnHelpBtn()
{

}

void qSlicerTopMenuModuleWidget::createHelpMenu()
{
	Q_D(qSlicerTopMenuModuleWidget);
	QMenu* menu = new QMenu(d->toolBar);
	d->helpBtn->setMenu(menu);
	d->helpBtn->setPopupMode(QToolButton::InstantPopup);
	//-----------详细信息--------------
	menu->addAction(QStringLiteral("详细信息"), []()
		{
			/*MITK_CUSTOM << "工具栏-详细信息";*/
			//数据放到配置文件，数据库或者注册表
			SoftwareInformation info = GetSoftwareInfo();
			QString c = "";
			QString txt = QStringLiteral
			(
				"软件名称：立体定向手术计划软件<br/>"
				"发布版本：%1<br/>" //1
				"完整版本：%2<br/>" //1.0.0
				"安装时间：%3<br/>"
				"注册人名称：%4<br/>"
				"地址：%5<br/>"
				"联系方式：%6<br/>"
				"版权声明：%7"
				//"许可证号：%5"
			).arg(info.versionRelase).arg(info.versionComplete).arg(info.installationDateTime)
				.arg(info.registerName).arg(info.address).arg(info.contact).arg(info.copyright);
			qGLMessageDialog::ShowInformationWithoutIcon(QStringLiteral("详细信息"), txt);
		});
	//-----------操作手册----------------------
	menu->addAction(QStringLiteral("操作手册"), []()
		{
			//MITK_CUSTOM << "工具栏-操作手册";
			QString curDir = QCoreApplication::applicationDirPath();
			QString manual = curDir + "/configure/manual.pdf";
			QDesktopServices::openUrl(QUrl::fromLocalFile(manual));
		});

	menu->addAction(QStringLiteral("工程师模式"), []()
		{
			auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());

			mw->SetPage(1000);
		});
	menu->addAction(QStringLiteral("设置"), []()
		{
			ctkSettingsDialog* const settingsDialog =
				qSlicerApplication::application()->settingsDialog();

			// Reload settings to apply any changes that have been made outside of the
			// dialog (e.g. changes to module paths due to installing extensions). See
			// https://github.com/Slicer/Slicer/issues/3658.
			settingsDialog->reloadSettings();

			// Now show the dialog
			settingsDialog->exec();
		});
}

void qSlicerTopMenuModuleWidget::createSetMenu()
{
	Q_D(qSlicerTopMenuModuleWidget);
	QMenu* menu = new QMenu(d->toolBar);
	d->settingBtn->setMenu(menu);
	d->settingBtn->setPopupMode(QToolButton::InstantPopup);
	menu->addAction(QStringLiteral("修改密码"), []()
		{
			/*MITK_INFO << "修改密码";
			MITK_CUSTOM << "修改密码";*/
			GLChangePassword* changPassword = new GLChangePassword;
			AccountType type = GLDb::glAccount.userName == ADMINISTRATOR_USERNAME ? AccountType::Administrator : AccountType::User;
			changPassword->SetAccountType(type);
			changPassword->exec();
			if (changPassword) {
				delete changPassword;
				changPassword = 0;
			}
		});

	/*menu->addAction(QStringLiteral("角标设置"), []()
		{
			qSlicerApplication::application()->layoutManager()->setCurrentPage(12);
		});

	menu->addAction(QStringLiteral("二维设置"), []()
		{
			qSlicerApplication::application()->layoutManager()->setCurrentPage(qSlicerLayoutManager::PageType::SliceControllerSetting);
		});*/

	menu->addAction(d->controller_visible);
	menu->addAction(d->action_reinit);
	if (GLDb::glAccount.userName == ADMINISTRATOR_USERNAME)
	{
		menu->addAction(QStringLiteral("注册用户"), []()
			{
				//M/*ITK_INFO << "注册用户";
				//MITK_CUSTOM << "注册用户";*/
				GLRegisterAccount* uiRegister = new GLRegisterAccount();
				uiRegister->show();
				uiRegister->exec();
				if (uiRegister) {
					delete uiRegister;
					uiRegister = 0;
				}
			});
	}
}

void qSlicerTopMenuModuleWidget::on_action_reinit_triggered() {
	Reinit();
}

void qSlicerTopMenuModuleWidget::on_controller_visible_triggered() {
	m_IsVisible = !m_IsVisible;
	qSlicerLayoutManager* lm = qSlicerApplication::application()->layoutManager();
	for (int i = 0; i < lm->threeDViewCount(); i++) {
		qMRMLThreeDWidget* widget = lm->threeDWidget(i);
		widget->threeDController()->setVisible(m_IsVisible);
	}
	auto names = lm->sliceViewNames();
	for (int i = 0; i < names.length(); i++) {
		lm->sliceWidget(names[i])->sliceController()->setVisible(m_IsVisible);
	}
}

SoftwareInformation qSlicerTopMenuModuleWidget::GetSoftwareInfo()
{
	SoftwareInformation info;
	//QSettings reg("HKEY_LOCAL_MACHINE\\Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\SurgeryPlan", QSettings::NativeFormat); //WOW6432Node是因为32为安装程序写注册表时被重定向了
	//																																	   //QString version = reg.value("DisplayVersion").toString();
	//QStringList keyList = reg.childKeys();
	//info.versionRelase = reg.value("DisplayVersion").toString();
	//info.versionComplete = "1"; //目前程序写死
	//info.installationDateTime = reg.value("InstallationDate").toString();

	return info;
}