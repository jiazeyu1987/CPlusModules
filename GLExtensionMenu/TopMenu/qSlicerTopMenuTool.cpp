#include "qSlicerTopMenuTool.h"
#include <qfile.h>
#include <qdir.h>
#include <qcoreapplication.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
//#include <berryPlatformUI.h>
//#include "internal/berryWorkbench.h"
#include <qmenu.h>
#include <QDesktopWidget>
#include <qtextedit.h>
#include <qapplication.h>
//#include <berryIWorkbenchPage.h>
#include <qtoolbutton.h>
#include <qsettings.h>
#include <QHBoxLayout>
//#include "GLDb.h"
//#include "GLChangePassword.h"
//#include "GLRegisterAccount.h"
#include <qdatetime.h>
#include <qtextcodec.h>
//#include "GLOperateRecord.h"
#include <qdesktopservices.h>
//#include "internal/GLMessageDialog.h"
QString qSlicerTopMenuTool::curDir = QCoreApplication::applicationDirPath();
QString qSlicerTopMenuTool::patientName = QStringLiteral("lin");
QString qSlicerTopMenuTool::imagesDir = qSlicerTopMenuTool::curDir + "/images";
QString qSlicerTopMenuTool::patientDataDir = qSlicerTopMenuTool::curDir + "/patientData/" + qSlicerTopMenuTool::patientName;
QString qSlicerTopMenuTool::fiberDir = qSlicerTopMenuTool::curDir + "/fiber";

QWidget *qSlicerTopMenuTool::toolBar = nullptr;
QToolButton *qSlicerTopMenuTool::saveBtn = nullptr; //����
QToolButton *qSlicerTopMenuTool::setUpBtn = nullptr; //����-����
QToolButton *qSlicerTopMenuTool::helpBtn = nullptr; //����-�ʺ�
QToolButton *qSlicerTopMenuTool::exitBtn1 = nullptr; //�˳�����
QToolButton *qSlicerTopMenuTool::exitBtn = nullptr; //�˳���ť

QProcess qSlicerTopMenuTool::myProcess;
//GLMessageBox qSlicerTopMenuTool::msg;
QFileInfoList qSlicerTopMenuTool::fileInfoList;

bool qSlicerTopMenuTool::createPath(QString path, ::PathType type)
{
	if (type == ::PathType::File)
	{
		//�ж�·���Ƿ����
		QFile file(path);
		if (file.exists())
			return true;

		int first = path.lastIndexOf("/");

		//����Ŀ¼
		QString dirpath = path.mid(0, first);
		QDir dir(dirpath);
		if (!dir.exists())
			dir.mkdir(dirpath);

		//�����ļ�
		if (dir.exists())
		{
			QString filename = path.mid(first + 1);
			if (file.open(QIODevice::WriteOnly))
			{
				file.close();
				return true;
			}
		}
		return false;
	}

	if (type == ::PathType::Dir)
	{
		return QDir().mkpath(path);
	}

	return false;
}

void qSlicerTopMenuTool::setPatientName(QString patientName)
{
	qSlicerTopMenuTool::patientName = patientName;
	qSlicerTopMenuTool::patientDataDir = qSlicerTopMenuTool::curDir + "/patientData/" + qSlicerTopMenuTool::patientName;
}

//void qSlicerTopMenuTool::setSoftwareoperationrecord(SoftwareOperationRecord & record)
//{
//
//}


QToolButton * qSlicerTopMenuTool::createBtn(QString iconStr, QString label, QSize size)
{
	QString qss("QToolButton::menu-indicator{image: none;}; ");
	QToolButton* btn = new QToolButton;
	btn->setStyleSheet(qss);
	btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	QIcon icon;
	QString iconFmt = ":/Resources/Icons/%1";
	icon.addFile(iconFmt.arg(iconStr));
	btn->setIcon(icon);
	btn->setIconSize(size);
	btn->setText(label);
	return btn;
}

void qSlicerTopMenuTool::createHelpMenu()
{
	QMenu *menu = new QMenu(toolBar);
	helpBtn->setMenu(menu);
	helpBtn->setPopupMode(QToolButton::InstantPopup);
	//-----------��ϸ��Ϣ--------------
	menu->addAction(QStringLiteral("��ϸ��Ϣ"), []()
		{
			/*MITK_CUSTOM << "������-��ϸ��Ϣ";*/
			//���ݷŵ������ļ������ݿ����ע���
			SoftwareInfo info = getSoftwareInfo();
            QString c = "";
			QString txt = QStringLiteral
			(
				"������ƣ����嶨�������ƻ����<br/>"
				"�����汾��%1<br/>" //1
				"�����汾��%2<br/>" //1.0.0
				"��װʱ�䣺%3<br/>"
				"ע�������ƣ�%4<br/>"
				"��ַ��%5<br/>"
				"��ϵ��ʽ��%6<br/>"
				"��Ȩ������%7"
				//"���֤�ţ�%5"
			).arg(info.versionRelase).arg(info.versionComplete).arg(info.installationDateTime)
				.arg(info.registerName).arg(info.address).arg(info.contact).arg(info.copyright);
			//GLMessageDialog::ShowInformationWithoutIcon(QStringLiteral("��ϸ��Ϣ"), txt);
		});
	//-----------�����ֲ�----------------------
	menu->addAction(QStringLiteral("�����ֲ�"), []()
		{
			//MITK_CUSTOM << "������-�����ֲ�";
			QString manual = curDir + "/configure/manual.pdf";
			QDesktopServices::openUrl(QUrl::fromLocalFile(manual));
		});
}

void qSlicerTopMenuTool::createSetMenu()
{
	QMenu *menu = new QMenu(toolBar);
	setUpBtn->setMenu(menu);
	setUpBtn->setPopupMode(QToolButton::InstantPopup);
	menu->addAction(QStringLiteral("�޸�����"), []()
	{
		/*MITK_INFO << "�޸�����";
		MITK_CUSTOM << "�޸�����";
		GLChangePassword *changPassword = new GLChangePassword;
		AccountType type = GLDb::glAccount.userName == ADMINISTRATOR_USERNAME ? AccountType::Administrator : AccountType::User;
		changPassword->SetAccountType(type);
		changPassword->show();
		changPassword->exec();
		if (changPassword) {
			delete changPassword;
			changPassword = 0;
		}*/
	});

	/*if (GLDb::glAccount.userName == ADMINISTRATOR_USERNAME)
	{
		menu->addAction(QStringLiteral("ע���û�"), []()
		{
			MITK_INFO << "ע���û�";
			MITK_CUSTOM << "ע���û�";
			GLRegisterAccount *uiRegister = new GLRegisterAccount();
			uiRegister->show();
			uiRegister->exec();
			if (uiRegister) {
				delete uiRegister;
				uiRegister = 0;
			}
		});
	}*/
}

QWidget * qSlicerTopMenuTool::topToolBar(int x, int y, int width, int height, bool autotest, QWidget *parent)
{
	if (!toolBar)
	{
		toolBar = new QWidget();
	}

	toolBar->setStyleSheet(QString::fromUtf8(""));
	toolBar->setFixedSize(QSize(680, 54));
	QString qss
	(
		"QToolButton::menu-indicator{image: none;}; "
	);
	toolBar->setStyleSheet("background-color: transparent;");
	if (!saveBtn) {
		saveBtn = createBtn("topmenu_save.png", QStringLiteral("����"), QSize(26, 30));
	}

	if (!setUpBtn)
	{
		setUpBtn = createBtn("topmenu_setting.png", QStringLiteral("����"), QSize(26, 30));
		createSetMenu();
	}


	if (!helpBtn)
	{
		helpBtn = createBtn("topmenu_help.png", QStringLiteral("����"), QSize(26, 26));
		createHelpMenu();
	}

	if (!exitBtn1) {
		exitBtn1 = createBtn("topmenu_exit.png", QStringLiteral("�˳�����"), QSize(24, 24));
	}

	if (!exitBtn)
	{
		exitBtn = createBtn("topmenu_quit.png", QStringLiteral("�˳�"), QSize(24, 24));
	}

	//����
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addStretch(6); //6/12
	layout->addWidget(saveBtn, 1);
	layout->addWidget(setUpBtn, 1);
	layout->addWidget(helpBtn, 1);
	layout->addWidget(exitBtn1, 1);
	layout->addWidget(exitBtn, 1);
	
	//toolBar->setStyleSheet("border: 3px solid red");
	toolBar->setParent(parent);
	toolBar->setGeometry(x, y, width, height);
	toolBar->setLayout(layout);

	return toolBar;
}

SoftwareInfo qSlicerTopMenuTool::getSoftwareInfo()
{
	SoftwareInfo info;
	QSettings reg("HKEY_LOCAL_MACHINE\\Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\SurgeryPlan", QSettings::NativeFormat); //WOW6432Node����Ϊ32Ϊ��װ����дע���ʱ���ض�����
                                                                                                                                           //QString version = reg.value("DisplayVersion").toString();
	QStringList keyList = reg.childKeys();
	info.versionRelase = reg.value("DisplayVersion").toString();
	info.versionComplete = "1"; //Ŀǰ����д��
	info.installationDateTime = reg.value("InstallationDate").toString();

	return info;
}

void qSlicerTopMenuTool::Exit(QString cause, bool autotest)
{
	//GLDb db;
	//if (!db.createDbConnect())
	//{
	//	QAbstractButton *btn = new QPushButton(QStringLiteral("ȷ��"));
	//	QPair<QAbstractButton *, QMessageBox::ButtonRole> btnPair(btn, QMessageBox::ButtonRole::AcceptRole);
	//	QVector<QPair<QAbstractButton *, QMessageBox::ButtonRole>> btnVec(1, btnPair);
	//	QString title = QStringLiteral("��־���");
	//	QString txt = QStringLiteral("���ݿ�����ʧ�ܣ�");
	//	//GLMessageBox msg(title, txt, btnVec, QMessageBox::Warning);
	//}
	//else
	//{
	//	SoftwareOperationRecord record;
	//	record.content = cause;
	//	record.classes = "_User";
	//	record.module = "Login";
	//	db.setSoftwareOperationRecord(record);
	//}

	////�������ݿ�
	//if (!autotest)
	//	databackup();

	//berry::Workbench *workbench = dynamic_cast<berry::Workbench *>(berry::PlatformUI::GetWorkbench());
	//workbench->Close();
}

void qSlicerTopMenuTool::ShowChangePassword()
{
	/*GLChangePassword *changPassword = new GLChangePassword;
	AccountType type = GLDb::glAccount.userName == ADMINISTRATOR_USERNAME ? AccountType::Administrator : AccountType::User;
	changPassword->SetAccountType(type);
	changPassword->show();
	changPassword->exec();
	if (changPassword) {
		delete changPassword;
		changPassword = 0;
	}*/
}


QFileInfoList qSlicerTopMenuTool::getFileInfoFromDir(QString dir)
{
    QDir _dir(dir);
    QFileInfoList _fileInfoList = _dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    fileInfoList << _fileInfoList;
    for (int i = 0; i < _fileInfoList.size(); i++)
    {
        getFileInfoFromDir(_fileInfoList.at(i).filePath());
    }

    return fileInfoList;
}

//���ݿⱸ��
void qSlicerTopMenuTool::databackup()
{
	////GLMessageBox msg1;
	////QMessageBox::ButtonRole role = msg1.YesOrNoError(QStringLiteral("�˳�����"), QStringLiteral("�Ƿ񱸷����ݿ�����"));
	////if (role == QMessageBox::NoRole)
	////  return;
	//QString path = curDir + "/dbbackup/";
	//createPath(path, Dir);
	//QString filename = path + QString("db_jl_bak%1.sql").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));

	////QString filename = curDir + "/" + QString("db_jl_bak.sql");
	//QSettings set(curDir + "/configure/cfg.ini", QSettings::IniFormat);
	//set.setIniCodec(QTextCodec::codecForName("GBK"));
	//QString mysqldump = set.value("MySQL/mysqldump").toString();
	//QString program = mysqldump + "mysqldump.exe";
	//QStringList arguments;
	//arguments << "-uroot" << "-p123456" << "db_jl";
	////QProcess myProcess;
	////myProcess.setWorkingDirectory("C:/Program Files/MySQL/MySQL Server 8.0/bin");
	////myProcess.setProgram("cmd");
	////myProcess.setArguments(arguments);
	//myProcess.setStandardOutputFile(filename); //�����Զ������ļ�������·����Ҫ�Ѿ�����
	//myProcess.start(program, arguments);

	////ͬ����д
	////if (!myProcess.waitForStarted())
	////  QString error = myProcess.errorString();

	////if (!myProcess.waitForFinished(60 * 1000))
	////  QString error = myProcess.errorString();

	////QByteArray result = myProcess.readAll();
	////myProcess.close();

	////�첽��д 
	//connect(&myProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus)
	//	{
	//		QByteArray result = myProcess.readAll();
	//		myProcess.close();
	//		myProcess.kill();
	//		msg.accept();

	//		
	//		GLMessageDialog::ShowInformation(QStringLiteral("�˳�����"), QStringLiteral("���ݿ����ݱ�����ɣ�"));
	//	});

	//QMessageBox::ButtonRole role1 = msg.queRenInfo(QStringLiteral("�˳�����"), QStringLiteral("���ڱ��ݡ���\n�����Ҫ��ֹ���ݣ�������ֹ����"), QStringLiteral("��ֹ����"));
	//if (role1 == QMessageBox::AcceptRole)
	//{
	//	myProcess.close();
	//	myProcess.kill();
	//}
}

int qSlicerTopMenuTool::calAge(QString date)
{
	//MITK_INFO << "���㻼�����䣬�������գ�" << date.toStdString();
	int year = date.section('/', 0, 0).toInt();
	int month = date.section('/', 1, 1).toInt();
	int day = date.section('/', 2, 2).toInt();

	int currentYear = QDate::currentDate().year();
	int currentMonth = QDate::currentDate().month();
	int currentDay = QDate::currentDate().day();

	int yearDiff = currentYear - year;
	if (month > currentMonth)
		yearDiff--;
	if (month == currentDay && day > currentDay)
		yearDiff--;

	return yearDiff;
}
