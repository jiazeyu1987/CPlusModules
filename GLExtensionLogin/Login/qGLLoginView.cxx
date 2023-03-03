/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/
#if _DEBUG
//#pragma comment(lib, "AuxiliaryLib/lib_usbKey/libhasp_cpp_windows_x64_mtd_msc10_d.lib")
#pragma comment(lib, "./Debug/libhasp_cpp_windows_x64_mtd_msc10_d.lib")
#pragma comment(lib, "./Debug/libhasp_windows_x64_33652.lib") // ������ʽ����ÿ������һ�����֣�Ҳ����ÿ��������������һ����
#else
//#pragma comment(lib, "E:\\S\\GLModule\\RRBuild_GLExtensionLogin\\Login\\Release/libhasp_cpp_windows_x64_mtd_msc10.lib")
#pragma comment(lib, "./Release/libhasp_cpp_windows_x64_mtd_msc10.lib")
#pragma comment(lib, "./Release/libhasp_windows_x64_33652.lib") // ������ʽ����ÿ������һ�����֣�Ҳ����ÿ��������������һ����
#endif // _DEBUG

//#pragma comment�ǹ������ڵ�Ŀ¼Ϊ��ǰ��ַ����ν����Ŀ¼���򵥵������vs���Ҽ�����ù��̣�Ȼ������Դ������д򿪣��򿪵��ļ��о��ǵ�ǰĿ¼
//#pragma comment(lib, "E:/MK/abc/Plugins/AuxiliaryLib/lib_usbKey/libhasp_windows_x64_demo.lib") // demo������


#include "qGLLoginView.h"
#include <qGLlog.h>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QMouseEvent>
#include <qsettings.h>
#include <qformlayout.h>
#include <QDate>
#include <QKeyEvent>
#include <qtextcodec.h>
#include <qtextcodec.h>
#include <qitemdelegate.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qGLTool.h>
#include <qGLDb.h>
#include <qGLOperateRecord.h>
#include <qGLChangePassword.h>
#include "qGLSoftwareUserAgreement.h"
#include "qGLSoftwareUserAgreementLogin.h"
#include <qdir.h>
#include "qGLChangePassword.h"
#include "qGLPrivacyPolicy.h"
#include "qSlicerLoginModuleWidget.h"
#include "GLPopWindows.h"
#include "qGLMessageDialog.h"
#include "qJMainWindow.h"
//USB Key(Sentinel)
#include "hasp_api.h"
#include "hasp_api_cpp.h"
#include <PluginConst.h>
//#include <GLPopWindows.cpp>

//#include <vld.h> //�ڴ�й©���

HistoryNameWidget::HistoryNameWidget(QWidget *parent)
  :QDialog(parent)
{
  layoutMain = new QVBoxLayout;
  setWindowFlag(Qt::FramelessWindowHint);
  setLayout(layoutMain);
  layoutMain->setSpacing(0);
  layoutMain->setMargin(0);
  //layoutMain->addStretch();

  //���ñ���Ϊ��ɫ
  QString qss
  (
    "background-color: rgb(255, 255, 255);"
    "border-style: none;"
    "color: rgb(0, 0, 0);"
    "border-radius: 0px;"
  );
  setStyleSheet(qss);

  //����λ�ú͹̶����
  QLabel* line_userName = parent->findChild<QLabel*>("label_3");
  if (line_userName)
  {
      int y = line_userName->y();
      int x = line_userName->x();
      setGeometry(x, y, 360, 1); //��֪��Ϊʲôx��y�����ԣ���Ҫ����29px
      raise();
  }
    
  setFixedWidth(360);

  hide();
}

void HistoryNameWidget::addRow(QString doctorName, QString username)
{
	QWidget* widgetRow = new QWidget; //һ��
	widgetRow->setFixedSize(360, 42); //�̶���͸ߣ���˼����ÿһ���ܵĿ���Ѿ�����

	QPushButton* btnHistroyName = new QPushButton; //��ʷ�û���
	btnHistroyName->setObjectName("btnRow");
	btnHistroyName->setFixedHeight(42);

	QPushButton* btnDel = new QPushButton; //ɾ���û���
	btnDel->setFixedSize(13, 13);
	btnDel->setIcon(QIcon(":/GLExtensionLogin/Icons/ZZLoginViewControls_delHistoryName.png"));

	//������ʷ�û�����ť
	QString qssLabel
	(
		"border-color: rgb(255, 0, 0);"
	);

    //ҽ������
	QLabel* labelDoctorName = new QLabel(doctorName);
	labelDoctorName->setFixedHeight(42);
	labelDoctorName->hide(); //ҽ���������أ�Ŀǰû��ʹ��
	labelDoctorName->setStyleSheet(qssLabel);
	labelDoctorName->setObjectName("labelDoctorName");

    //�û���
	QLabel* labelUsername = new QLabel(username);
	labelUsername->setFixedHeight(42);
	labelUsername->setStyleSheet(qssLabel);
	labelUsername->setObjectName("labelUsername");

	//��ǩ����
	QFormLayout* layout = new QFormLayout;
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addRow(labelDoctorName, labelUsername);
	//��ť����
	QHBoxLayout* layoutBtn = new QHBoxLayout;
	layoutBtn->setMargin(0);
	layoutBtn->addSpacing(30);
	layoutBtn->addLayout(layout);
	layoutBtn->addSpacing(60);
	//���ò��ָ���ť
	btnHistroyName->setLayout(layoutBtn);

	//�����ܲ���
	QHBoxLayout* layoutRow = new QHBoxLayout;
	layoutRow->setMargin(0);
	layoutRow->addWidget(btnHistroyName);
	layoutRow->addWidget(btnDel);
	layoutRow->addSpacing(15);

	//���õ�����
	widgetRow->setLayout(layoutRow);

	//��ӵ�������
	layoutMain->addWidget(widgetRow);

	connect(btnHistroyName, &QPushButton::clicked, this, [=]()
		{
			QPushButton* btn = qobject_cast<QPushButton*>(sender());
			if (btn)
			{
				QLabel* labelUsername = btn->findChild<QLabel*>("labelUsername");
				if (labelUsername)
				{
					QString username = labelUsername->text();
					emit this->sigUsername(username);
				}
			}
		});

	connect(btnDel, &QPushButton::clicked, this, [=]() //ɾ����ʷ�û���
		{
			QPushButton* btnDel_ = qobject_cast<QPushButton*>(sender());
			if (btnDel_)
			{
				QWidget* parent = btnDel_->parentWidget();
				QLabel* labelUsername = parent->findChild<QLabel*>("labelUsername");
				if (labelUsername)
				{
					QString username = labelUsername->text();
					emit sigDelUsername(username);
					layoutMain->removeWidget(parent);
					delete parent;
					QSize size_ = size();
					size_.rheight() -= 42;
					resize(size_);
				}
			}
		});
}

//setCursor(Qt::PointingHandCursor);
ZZLoginView::ZZLoginView(Ui_qSlicerLoginModuleWidget& m_Controls, qSlicerLoginModuleWidget* loginModuleWidget)
    : QObject(nullptr), m_Controls(m_Controls), loginModuleWidget(loginModuleWidget), parentWidget(loginModuleWidget)
{
    CreateQtPartControl(loginModuleWidget);
}

ZZLoginView::~ZZLoginView() {}

void ZZLoginView::CreateQtPartControl(QWidget* parent)
{
    //GetVarConfig();
    //Log* instance = Log::instance();
    //connect(this, &ZZLoginView::sig_infoLog, instance, &Log::appendInfoLog);
	//connect(this, &ZZLoginView::sig_customLog, instance, &Log::appendInfoLog);

    //����û�Э�����
    if (getSoftwareUserAgreementShow())
    {
        new SoftwareUserAgreement(nullptr);
        setSoftwareUserAgreementShow(false);
    }


 /*   QLabel* labelBG = new QLabel(parent);
    labelBG->setObjectName("labelBG");
    labelBG->setFixedSize(1920, 1080);

    labelBG->installEventFilter(this);*/

    // create GUI widgets from the Qt Designer's .ui file
    //��֪��Ϊʲô�����������ƶ����Ǹ�������������
    m_Controls.setupUi(parent);

 /*   QString fileName = QString("%1/%2/%3/qss/glbluestyle.qss");
    setQss(fileName);*/

   // m_Controls.checkUserAgreement->setEnabled(false);

    m_Controls.lineEdit_USER->setFocusPolicy(Qt::StrongFocus);
    m_Controls.lineEdit_USER->installEventFilter(this);

    //��������ģʽ
    m_Controls.lineEdit_PASSWORD->setEchoMode(QLineEdit::Password);
    m_Controls.btn_CLOSE->setCursor(QCursor(Qt::PointingHandCursor));
    m_Controls.btnUserAgreement->setCursor(QCursor(Qt::PointingHandCursor));

    //��¼״̬
    labelLoginStatus = new QLabel(QStringLiteral("<font color='black'>��¼״̬</font>"), parent);
    labelLoginStatus->setGeometry(1111, 380, 360, 48); //����λ��
    //labelLogStatus->setStyleSheet("background:opaque"); //��ʾ��͸��
    labelLoginStatus->setStyleSheet("background:white");
    labelLoginStatus->hide();

    //����û�Э��
    connect(m_Controls.btnUserAgreement, &QPushButton::clicked, this, &ZZLoginView::slot_btnUserAgreementClicked);
    //��˽����
    connect(m_Controls.btnPrivacyPolicy, &QPushButton::clicked, this, &ZZLoginView::onBtnPrivacyPolicyClicked);
    //��ʱ�����ܹ�
    timer.callOnTimeout(this, &ZZLoginView::slot_timerTimeOut, Qt::QueuedConnection);
    timer.start(1000);
    slot_timerTimeOut(); //���̿�ʼ���USBKey״̬
    //��¼
    connect(m_Controls.btn_OK, SIGNAL(clicked()), this, SLOT(confirm()));
    //�˳�
    connect(m_Controls.btn_CLOSE, &QPushButton::clicked, this, [=]()
    {
        emit sig_infoLog("�û�����˳�");
        GLTool tool;
        tool.Exit(QStringLiteral("�˳���¼"), false);
    });

    //SetDelayFunction(500, ""); //���յ����麯��OnTimerOutFunction��500ms֮�����

    //����Tab˳��
    QWidget::setTabOrder(m_Controls.lineEdit_USER, m_Controls.lineEdit_PASSWORD);
    QWidget::setTabOrder(m_Controls.lineEdit_PASSWORD, m_Controls.btn_OK);

    menu = new QMenu();
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowFlags(Qt::FramelessWindowHint | menu->windowFlags());
    QString menuStyle
    (
        "background-color: #F00000"
    );

    menu->setStyleSheet(menuStyle);
    //menu->setWindowOpacity(0.7);
    usernameList = getPreviousLoginUsername();

    for (int i = 0; i < usernameList.size(); i++)
    {
        QString username = usernameList.at(i);
        menu->addAction(username, this, [=]()
        {
            QAction* action = qobject_cast<QAction*>(sender());
            QString username = action->text();
            m_Controls.lineEdit_USER->setText(username);
        });
    }

    connect(m_Controls.lineEdit_USER, &QLineEdit::textEdited, this, [=]()
    {
        menu->hide();
        AutoCheckAgreement(m_Controls.lineEdit_USER->text());
    });
    m_Controls.lblUser->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputUsernameNone.png"));
    connect(m_Controls.lineEdit_USER, &QLineEdit::editingFinished, this, [=]()
    {
        emit sig_infoLog("�����û���" + m_Controls.lineEdit_USER->text());
        //�༭��ɣ�����ͼƬ
        if (!m_Controls.lineEdit_USER->text().isEmpty())
        {
            m_Controls.lblUser->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputUsername.png"));
            AutoCheckAgreement(m_Controls.lineEdit_USER->text());
        }   
        else
            m_Controls.lblUser->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputUsernameNone.png"));
    });
    m_Controls.lblPassword->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputPasswordNone.png"));
    connect(m_Controls.lineEdit_PASSWORD, &QLineEdit::editingFinished, this, [=]()
    {
		emit sig_infoLog("�����û���" + m_Controls.lineEdit_PASSWORD->text());
        //�༭��ɣ�����ͼƬ
        if (!m_Controls.lineEdit_PASSWORD->text().isEmpty())
            m_Controls.lblPassword->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputPassword.png"));
        else
            m_Controls.lblPassword->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputPasswordNone.png"));
    });
    //��ʷ�û���
    historyNameWidget = new HistoryNameWidget(parent);
    connect(historyNameWidget, &HistoryNameWidget::sigUsername, this, [=](QString username)
    {
		emit sig_infoLog("ѡ����ʷ�û���" + username);
        m_Controls.lineEdit_USER->setText(username);
        if (!m_Controls.lineEdit_USER->text().isEmpty())
            m_Controls.lblUser->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputUsername.png"));
        historyNameWidget->hide();
        AutoCheckAgreement(m_Controls.lineEdit_USER->text());
    });
    connect(historyNameWidget, &HistoryNameWidget::sigDelUsername, this, [=](QString username)
    {
		emit sig_infoLog("ɾ����ʷ�û���" + username);
        usernameList.removeAll(username);
        setCurrentLoginUsername("");
    });

    usernameList = getPreviousLoginUsername();
    for (int i = 0; i < usernameList.size(); i++)
    {
        QString username = usernameList.at(i);
        //�����ݿ��ȡ��Ӧ���û�����ʵ��Ϣ
        GLDb db;
        if (!db.createDbConnect())
        {
            emit sig_infoLog("���ݿ�����ʧ�ܣ����ܻ�ȡ�û�������ʵ����");
        }
        else
        {
            //Ŀǰҽ��������û����ʾ����ʷ�û��У����Բ���ȡ
        }
        historyNameWidget->addRow(QStringLiteral("��ҽ��"), username);
    }

    connect(m_Controls.btnHistoryName, &QPushButton::clicked, this, [=](bool checked)
    {
        m_Controls.btnHistoryName->setIcon(QIcon(":/GLExtensionLogin/Icons/drowDownHistoryName.png"));
        emit sig_customLog("�����ʷ�û���");
        if (checked)
        {
            historyNameWidget->show();
        }
        if (!checked)
        {
            historyNameWidget->hide();
        }
    });
}

void ZZLoginView::OnTimerOutFunction(QString)
{
    usernameList = getPreviousLoginUsername();
    for (int i = 0; i < usernameList.size(); i++)
    {
        QString username = usernameList.at(i);
        //�����ݿ��ȡ��Ӧ���û�����ʵ��Ϣ
        GLDb db;
        if (!db.createDbConnect())
        {
            emit sig_infoLog("���ݿ�����ʧ�ܣ����ܻ�ȡ�û�������ʵ����");
        }
        else
        {

        }
        historyNameWidget->addRow(QStringLiteral("��ҽ��"), username);
    }

    connect(m_Controls.btnHistoryName, &QPushButton::clicked, this, [=](bool checked)
    {
        emit sig_customLog("�����ʷ�û���");
        if (checked)
        {
            historyNameWidget->show();
        }
        if (!checked)
        {
            historyNameWidget->hide();
        }
    });
    setSelfDefToolBar(); //����������Ϊ��������Щ�ؼ��ڳ�ʼ�����

    emit sig_nextPage();
    //�Ƿ�����ָ�
    ResetLastOperate();
}

void ZZLoginView::slot_btnUserAgreementClicked()
{
    int i=GLPopWindows::ShowInformation(QStringLiteral("����Э��"), PluginConst::ProtocolInfo,true);
}

void ZZLoginView::onHistoryUsername()
{

  QPoint point = m_Controls.label_3->pos();
  int height = m_Controls.label_3->height();
  point.ry() += height;
  point.setX(m_Controls.lineEdit_USER->x());
  
  menu->popup(point);
}

void ZZLoginView::setQss(QString fileName)
{
    if (!fileName.isEmpty())
    {
        QFile styleFile(fileName);
        if (styleFile.open(QIODevice::ReadOnly)) {
            QString styleSheet(styleFile.readAll());
            parentWidget->setStyleSheet(styleSheet);
            styleFile.close();
            return;
        }
    }
    int width = parentWidget->width();
    int height = parentWidget->height();
    parentWidget->setFixedSize(width, height);
    QWidget *parent = parentWidget->parentWidget();
    if (parent)
    {

        parent->dumpObjectTree();
        parent->dumpObjectInfo();
    }
    else
    {

        parentWidget->dumpObjectTree();
        parentWidget->dumpObjectInfo();

    }
   
}

void ZZLoginView::AutoCheckAgreement(QString usrname)
{
	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //��ֹ��������
	QStringList usernameList = set.value("Login/rememberUsername").toStringList();
	usernameList.removeDuplicates();
    for (auto userItem : usernameList)
    {
        if (userItem==usrname)
        {
            m_Controls.checkUserAgreement->setChecked(true);
            return;
        }
    }
    m_Controls.checkUserAgreement->setChecked(false);
}

QStringList ZZLoginView::getPreviousLoginUsername()
{
	//MITK_INFO << "�������ļ���ȡ��һ���û���";
	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //��ֹ��������
	QStringList usernameList = set.value("Login/rememberUsername").toStringList();
	usernameList.removeDuplicates();
	return usernameList;
}

void ZZLoginView::setCurrentLoginUsername(QString username)
{
	emit sig_infoLog("����ǰ�û������뵽�����ļ�����");
  if (!username.isEmpty())
  {
  usernameList.prepend(username);
  usernameList.removeDuplicates();
  int size = usernameList.size();
  while (usernameList.size() > 5)
  {
    usernameList.removeLast();
  } 
  }

	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //��ֹ��������
	set.setValue("Login/rememberUsername", usernameList);
}

bool ZZLoginView::getSoftwareUserAgreementShow()
{
	emit sig_infoLog("�������ļ��л�ȡ�Ƿ���Ҫչʾ��������û�Э���־");
	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //��ֹ��������
	bool isShow = set.value("Login/softwareUserAgreementShow").toBool();
	
	return isShow;
}

void ZZLoginView::setSoftwareUserAgreementShow(bool isShow)
{
	emit sig_infoLog("�����Ƿ���Ҫչʾ��������û�Э���־�������ļ�");
	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //��ֹ��������
	set.setValue("Login/softwareUserAgreementShow", isShow);
	
}

LoginStatus ZZLoginView::usbkeyVerification()
{
	//���µ�USB Key����ʱ��������µ�vendorCode���滻ԭ���ļ���
	//unsigned char vendorCode[] = //����Code
	//  "AzIceaqfA1hX5wS+M8cGnYh5ceevUnOZIzJBbXFD6dgf3tBkb9cvUF/Tkd/iKu2fsg9wAysYKw7RMA"
	//  "sVvIp4KcXle/v1RaXrLVnNBJ2H2DmrbUMOZbQUFXe698qmJsqNpLXRA367xpZ54i8kC5DTXwDhfxWT"
	//  "OZrBrh5sRKHcoVLumztIQjgWh37AzmSd1bLOfUGI0xjAL9zJWO3fRaeB0NS2KlmoKaVT5Y04zZEc06"
	//  "waU2r6AU2Dc4uipJqJmObqKM+tfNKAS0rZr5IudRiC7pUwnmtaHRe5fgSI8M7yvypvm+13Wm4Gwd4V"
	//  "nYiZvSxf8ImN3ZOG9wEzfyMIlH2+rKPUVHI+igsqla0Wd9m7ZUR9vFotj1uYV0OzG7hX0+huN2E/Id"
	//  "gLDjbiapj1e2fKHrMmGFaIvI6xzzJIQJF9GiRZ7+0jNFLKSyzX/K3JAyFrIPObfwM+y+zAgE1sWcZ1"
	//  "YnuBhICyRHBhaJDKIZL8MywrEfB2yF+R3k9wFG1oN48gSLyfrfEKuB/qgNp+BeTruWUk0AwRE9XVMU"
	//  "uRbjpxa4YA67SKunFEgFGgUfHBeHJTivvUl0u4Dki1UKAT973P+nXy2O0u239If/kRpNUVhMg8kpk7"
	//  "s8i6Arp7l/705/bLCx4kN5hHHSXIqkiG9tHdeNV8VYo5+72hgaCx3/uVoVLmtvxbOIvo120uTJbuLV"
	//  "TvT8KtsOlb3DxwUrwLzaEMoAQAFk6Q9bNipHxfkRQER4kR7IYTMzSoW5mxh3H9O8Ge5BqVeYMEW36q"
	//  "9wnOYfxOLNw6yQMf8f9sJN4KhZty02xm707S7VEfJJ1KNq7b5pP/3RjE0IKtB2gE6vAPRvRLzEohu0"
	//  "m7q1aUp8wAvSiqjZy7FLaTtLEApXYvLvz6PEJdj4TegCZugj7c8bIOEqLXmloZ6EgVnjQ7/ttys7VF"
	//  "ITB3mazzFiyQuKf4J6+b/a/Y";

	unsigned char vendorCode[] =
		"yfVhlDvndoqHdycGVilnL14HoBA254RHwF0Zzm0yZg2ho9Fjlzqx6jBQ0gr+Wy5YSZzQwvkdFfvgweUD"
		"IMhNTnBbEtr4grBBHt0EJO8Mz8ujCh6Sn9Iw17dOomRVjs6MPaYntW1trPHvCUU0eSaQca4If0S4yT9t"
		"JgbvVCyjuVJtxdAt1SbeAh3llHqGbSjS+tl+N0LUMqoLCnpNMqXuogQWiPmBOkohrgX8U539bqDCe6Rj"
		"PWL6X7KnPpvkBI6JnyfUETPglMeGEkBtubzpKDlKNw1rsasDBNFa8a4CTEG3+MfOafVtp0m8kwSaVbMj"
		"fN6FwizrKumwIuzZTpXVNfWk3O+eU1euop6r0AOIjUnaTWWXnuEdgDwkPaJs2qa/CO/4ifUlapEpEF4g"
		"9Oazj3l5evz06inMHCbVr5TiUparyzaarvX4G2zxRKrYbNO+nYHNz1q2CbMUPI0lL3mDANhtoAxlykGC"
		"jo87VpL2oe1DMtH9Wk88tx8yAsOkjCptPMAR5lNMebfyG1X0taRjsQKlI4+YLlGKEBgZJ1d2Z+rN/7Wl"
		"XbiLEJ1vSZ9Cy7m+Inhz1ALSi4OAV5CL5AXps7/6Q0iAomrxdsTJ9CjMjPuF0W4SU3R9CLDRK+7OwM4N"
		"p9Q1Js1BdgU15IHEwRklkAsmgdwWWS1KXhiuQj9bHVSgAU56Eni8xBtS8vH/TgLkSnEAtGpad3InMC5D"
		"g8wigilyF23pF9YgYm/y+fxLdYOse++I8tKKW1l+UrSNtbXB4gr6tjTGHjvsDl1Eyzbo0+H3rI01INHn"
		"NgEXVdTf7A6Z6NER1Ur6o20bOgHLDNkWHUkHNMLrWIEHldvQlWcGT10h89R8uIiVXJnQGmvxnnWza9jW"
		"O4GJt/NamsT9mO9Ckc6StuFNhTUU5wVoh7Y4W/eiqKMcnu0I0s2aVrJuwWiWUubhEG/pd2iRUafO1jXQ"
		"0tva7rOzY3m9BBi//TfjHw==";

	haspStatus status; //״̬��û�м�⵽USB Key������0x0007����������USB Key������0x0031����ȷ����0

	//MITK_INFO << "��¼��feature";
	//42��USB Key��featureId��ÿ��USB Key���Դ洢���ɸ�id��id����ͨ��ems��¼��ȥ��ͨ����������Բ鿴id
	//USB Keyͨ��vendorCode����Ƿ������ȷ��USB Key����ͨ��feature Id��飬�����Ƿ�ƥ����ȷ��id
	//ChaspFeature feature4 = ChaspFeature::fromFeature(42); 
	ChaspFeature feature = ChaspFeature::fromFeature(1);
	//��xml�ַ������ǻ�ȡ��ͬ���ܵĽӿ�
	//const char* localScope =
	//  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	//  "<haspscope>"
	//  "    <license_manager hostname =\"localhost\" />"
	//  "</haspscope>";
	Chasp hasp(feature);
	//status = hasp.login(vendorCode, localScope); //�ƶ���ͬ�ĻỰ
	status = hasp.login(vendorCode);

	//MITK_INFO << "��ȡ��״̬��" << status;

	if (HASP_SUCCEEDED(status))
		hasp.logout();

	LoginStatus usbKeyStatus;
	switch (status)
	{
	case HASP_STATUS_OK:
		usbKeyStatus = USBKeySuccess;
		break;
	case HASP_HASP_NOT_FOUND:
		usbKeyStatus = USBKeyNone;
		break;
	case HASP_FEATURE_NOT_FOUND:
		usbKeyStatus = USBKeyVerificationFailed;
	default:
		usbKeyStatus = USBKeyVerificationFailed;
		break;
	}
	//MITK_INFO << "���usbKey��״̬:" << usbKeyStatus;
    return USBKeySuccess;
	//return usbKeyStatus;
}

void ZZLoginView::setLabelLogStatusText(QString status, QString icon, bool success, bool clearpassword, bool changedPassword)
{
    QTextCodec* codec = QTextCodec::codecForName("GBK");
    QByteArray baStatus = codec->fromUnicode(status);
    char* str = baStatus.data();

    if (success)
    {
        //��־���
        GLDb db;
        if (!db.createDbConnect())
        {
            QAbstractButton* btn = new QPushButton(QStringLiteral("ȷ��"));
            QPair<QAbstractButton*, QMessageBox::ButtonRole> btnPair(btn, QMessageBox::ButtonRole::AcceptRole);
            QVector<QPair<QAbstractButton*, QMessageBox::ButtonRole>> btnVec(1, btnPair);
            QString title = QStringLiteral("��־���");
            QString txt = QStringLiteral("���ݿ�����ʧ�ܣ�");
            //GLMessageBox msg(title, txt, btnVec, QMessageBox::Warning);
        }
        else
        {
            SoftwareOperationRecord record;
            record.content = QStringLiteral("��¼�ɹ�");
            record.classes = "_User";
            record.module = "Login";
            db.setSoftwareOperationRecord(record);
        }

        if (!changedPassword)
        {
            QString userName = m_Controls.lineEdit_USER->text();
            QString password = m_Controls.lineEdit_PASSWORD->text();
            QMap<QString, QString> map;
            map.insert("Account", userName);
            map.insert("Password", password);
            //GLORecord->RecordData(map);

            emit sig_nextPage();

        }
    }
    else
    {
        if (clearpassword)
            m_Controls.lineEdit_PASSWORD->clear();
        labelLoginStatus->setText(QStringLiteral("<font color='red'>%1</font>").arg(status));
        QPixmap map(icon);
        //labelLoginStatus->setPixmap(map);
        labelLoginStatus->show();
    }
}

LoginStatus ZZLoginView::verify(Account& account)
{
    emit sig_infoLog("��֤�û���������,�û�����" + account.userName + "�Ƿ����ʹ��:" + QString::number(account.isFirstUse));

    emit sig_infoLog("���USB Key");
    if (usbkeyStatus == USBKeyNone || usbkeyStatus == USBKeyVerificationFailed) //������֤USB��Կ��״̬
        return usbkeyStatus;

    emit sig_infoLog("USB Key���ɹ����������û�Э��");
    if (!m_Controls.checkUserAgreement->isChecked()) //δͬ������û�Э��
        return SoftwareUserAgreementUnCheck;

    //����֤����Ա���룬����ԱҲ��Ҫ����usbKey��ͬ��Э��
    if (account.userName == ADMINISTRATOR_USERNAME)
    {
        if (account._password == ADMINISTRATOR_PASSWORD)
        {
            GLDb::glAccount.userName = account.userName;
            GLDb::glAccount._password = account._password;
            return VerificationSuccess;
        }
        else
            return PasswordError;
    }

    emit sig_infoLog("����û�Э����ɹ�������û����Ƿ�Ϊ��");
    if (account.userName.isEmpty())
        return UsernameEmpty;

    emit sig_infoLog("�û�����Ϊ�գ�����û��������Ƿ񳬹�30");
    if (account.userName.length() > 30)
        return UsernameFormatError;

    emit sig_infoLog("�û���������������Ƿ�Ϊ��");
    if (account._password.isEmpty())
        return PasswordEmpty;

    emit sig_infoLog("���벻Ϊ�գ�������볤���Ƿ�С��6���ߴ���12");
    if (account._password.length() < 6 || account._password.length() > 12)
        return PasswordFormatError;

    //�������ݿ⣬������
    emit sig_infoLog("���벻Ϊ�գ�����û����������Ƿ���ȷ");
    GLDb db(this);
    if (!db.createDbConnect())
    {
        emit sig_infoLog("���ݿ�����ʧ�ܣ�ֱ���˳�����");
        //����ʧ�ܣ�ֱ���˳�
        QTimer::singleShot(0, qApp, &QApplication::closeAllWindows);
    }

    return db.login(account);
}

bool ZZLoginView::eventFilter(QObject * obj, QEvent * event)
{
	//MITK_INFO << "�¼�����" << event->type();
	//�ж��¼��Ǽ���Enter�¼�
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *eventKey = static_cast<QKeyEvent *>(event);
		if (eventKey->key() == Qt::Key_Return || eventKey->key() == Qt::Key_Enter)
		{
            emit sig_customLog("Enter�����¼���ִ�е�¼����");
			confirm();
			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

bool ZZLoginView::createPath(QString filepath)
{
  //�ж�·���Ƿ����
  QFile file(filepath);
  if (file.exists())
    return true;

  int first = filepath.lastIndexOf("/");
  
  //����Ŀ¼
  QString dirpath = filepath.mid(0, first);
  QDir dir(dirpath);
  if (!dir.exists())
    dir.mkdir(dirpath);

  //�����ļ�
  if (dir.exists())
  {
    QString filename = filepath.mid(first+1);
    if (file.open(QIODevice::WriteOnly))
    {
      file.close();
      return true;
    }
  }
  return false;
}

void ZZLoginView::onBtnPrivacyPolicyClicked(bool)
{
    int i = GLPopWindows::ShowInformation(QStringLiteral("��˽����"), PluginConst::PrivacyPolicy, true);
}

void ZZLoginView::setSelfDefToolBar()
{
  //auto windowbench = dynamic_cast<berry::Workbench*>(GetSite()->GetWorkbenchWindow()->GetWorkbench());
  //auto windows = windowbench->GetWorkbenchWindows();
  //for (int i = 0; i < windows.size(); i++)
  //{
  //  auto win = windows.at(i);
  //  berry::IWorkbenchWindow::Pointer iWind = windows.at(i);
  //  berry::WorkbenchWindow * wind = dynamic_cast<berry::WorkbenchWindow *>(iWind.GetPointer());
  //  if (i == 0)
  //  {
  //    toolBar = wind->pSelfDefToolBar;
  //  }
  //}
  //QList<QAction *> actionList = toolBar->actions();

  //berry::Workbench *workbench = dynamic_cast<berry::Workbench *>(berry::PlatformUI::GetWorkbench());
  //QAction *exit = workbench->toolBarExitAction;
  //connect(exit, &QAction::triggered, this, [this]()
  //{
  //  GLTool::Exit(QStringLiteral("�����˳�"), false);
  //});
}

void ZZLoginView::confirm()
{
    emit sig_infoLog("�û������¼");
    Account account;
    account.userName = m_Controls.lineEdit_USER->text();
    account._password = m_Controls.lineEdit_PASSWORD->text();
    LoginStatus _status = verify(account);

    switch (_status)
    {
    case VerificationSuccess://�ɹ�
    {
        emit sig_infoLog("��¼�ɹ�");

        //���˺ź����뱣�浽ȫ�ֵ�DataStorage
        loginModuleWidget->AddSignalNode("userName", account.userName.toStdString().c_str());
        loginModuleWidget->AddSignalNode("password", account._password.toStdString().c_str());

        //���û����������pdf�Ĳ�����
        GLTool::worker = account.userName;

        bool changedPassword = false;
        if (account.isFirstUse == 1)
        {
            //��һ��ʹ�ã������Ƿ��޸�����ĶԻ���
            //QAbstractButton* btnYes = new QPushButton(QStringLiteral("��"));
            //QPair<QAbstractButton*, QMessageBox::ButtonRole> btnYesPair(btnYes, QMessageBox::ButtonRole::YesRole);
            //QAbstractButton* btnNo = new QPushButton(QStringLiteral("��"));
            //QPair<QAbstractButton*, QMessageBox::ButtonRole> btnNoPair(btnNo, QMessageBox::ButtonRole::NoRole);
            //QVector<QPair<QAbstractButton*, QMessageBox::ButtonRole>> btnVec;
            //btnVec << btnYesPair << btnNoPair;
            //QString title = QStringLiteral("�޸�����");
            //QString txt = QStringLiteral("�Ƿ��޸�����");
            //GLMessageBox msg(title, txt, btnVec, QMessageBox::Question);
            MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("�޸�����"), QStringLiteral("�Ƿ��޸�����?"));
            //if (msg.result() == btnYes)
            //{
            //    GLChangePassword* changPassword = new GLChangePassword;
            //    AccountType type = GLDb::glAccount.userName == ADMINISTRATOR_USERNAME ? AccountType::Administrator : AccountType::User;
            //    changPassword->SetAccountType(type);
            //    account.isFirstUse = 2;
            //    changedPassword = true;
            //}
            if (res == MessageResult::OK) {
                //GLTool::ShowChangePassword();
                GLChangePassword* changPassword = new GLChangePassword;
                AccountType type = GLDb::glAccount.userName == ADMINISTRATOR_USERNAME ? AccountType::Administrator : AccountType::User;
                changPassword->SetAccountType(type);
                changPassword->exec();
                if (changPassword) {
                    delete changPassword;
                    changPassword = 0;
                }
                account.isFirstUse = 2;
                changedPassword = true;
            }
            else
            {
                account.isFirstUse = 3;
            }
            //ָisFirstUse��־Ϊ3
            GLDb db(this);
            if (!db.createDbConnect())
            {
                QTimer::singleShot(0, qApp, &QApplication::closeAllWindows);
            }
            db.setAccountIsFirstUse(account);
        }
        setLabelLogStatusText("", "", true, true, changedPassword);
        //���ݹ�ѡ��״̬��ѡ���Ƿ��ס�û���
        //bool isRemember = m_Controls.chkLoginStatus->isChecked();
        if (true)
        {
            setCurrentLoginUsername(account.userName);
        }
        break;
    }
    case USBKeyNone:
        setLabelLogStatusText(QStringLiteral("�����USB��Կ"), "", false, false);
        emit sig_infoLog("�����USB��Կ");
        break;
    case USBKeyVerificationFailed:
        setLabelLogStatusText(QStringLiteral("�������ȷ��USB��Կ"), "", false, false);
        emit sig_infoLog("�������ȷ��USB��Կ");
        break;
    case SoftwareUserAgreementUnCheck:
        setLabelLogStatusText(QStringLiteral("�����Ķ���ͬ�����Э�����˽����"), "", false, false);
        emit sig_infoLog("�����Ķ���ͬ�����Э�����˽����");
        break;
    case UsernameEmpty:
        setLabelLogStatusText(QStringLiteral("�������û���"), "", false, true);
        emit sig_infoLog("�������û���");
        break;
    case UsernameFormatError:
        setLabelLogStatusText(QStringLiteral("�û������Ȳ��ܳ���30��"), "", false, true);
        emit sig_infoLog("�û������Ȳ��ܳ���30��");
        break;
    case UsernameError:
        setLabelLogStatusText(QStringLiteral("�û���������"), "", false, true);
        emit sig_infoLog("�û���������");
        break;
    case DataTampered:
        setLabelLogStatusText(QStringLiteral("���ݱ����"), "", false, true);
        emit sig_infoLog("���ݱ����");
        break;
    case PasswordEmpty:
        setLabelLogStatusText(QStringLiteral("����������"), "", false, true);
        emit sig_infoLog("����������");
        break;
    case PasswordFormatError:
        setLabelLogStatusText(QStringLiteral("���볤�Ȳ�С��6λ�Ҳ�����12λ"), "", false, true);
        emit sig_infoLog("���볤�Ȳ�С��6λ�Ҳ�����12λ");
        break;
    case PasswordError:
        setLabelLogStatusText(QStringLiteral("�û������������"), "", false, true);
        emit sig_infoLog("�û������������");
        break;
    case Undefined:
        break;
    case AdministratorVerificationSuccess:
    {
        //���˺ź����뱣�浽ȫ�ֵ�DataStorage
        loginModuleWidget->AddSignalNode("userName", account.userName.toStdString().c_str());
        loginModuleWidget->AddSignalNode("password", account._password.toStdString().c_str());

        emit sig_nextPage();
    }
    default:
        break;
    }
}

void ZZLoginView::Exit(QString exitLogin)
{
  //��־���
  GLDb db;
  if (!db.createDbConnect())
  {
    QAbstractButton *btn = new QPushButton(QStringLiteral("ȷ��"));
    QPair<QAbstractButton *, QMessageBox::ButtonRole> btnPair(btn, QMessageBox::ButtonRole::AcceptRole);
    QVector<QPair<QAbstractButton *, QMessageBox::ButtonRole>> btnVec(1, btnPair);
    QString title = QStringLiteral("��־���");
    QString txt = QStringLiteral("���ݿ�����ʧ�ܣ�");
    GLMessageBox msg(title, txt, btnVec, QMessageBox::Warning);
}
  else
  {
    SoftwareOperationRecord record;
    record.content = exitLogin; // QStringLiteral("�˳���¼")
    record.classes = "_User";
    record.module = "Login";
    db.setSoftwareOperationRecord(record);
  }

  QTimer::singleShot(0, qApp, &QApplication::closeAllWindows);
}

void ZZLoginView::slot_timerTimeOut()
{
	qApp->processEvents();
	usbkeyStatus = usbkeyVerification();
	QString curPath = qApp->applicationDirPath();
	if (usbkeyStatus == USBKeySuccess)
	{
		//�л�ͼƬ
		QString curPath = qApp->applicationDirPath();
		QPixmap map(QString(":/GLExtensionLogin/Icons/insertUsbKey.png"));
		m_Controls.lblInsertUsbkeyIcon->setPixmap(map);
		m_Controls.labelInsertUsbkey->setText(QStringLiteral("�Ѳ�����Կ"));
		//ֹͣ��ʱ��
		timer.stop();
	}
	else if (usbkeyStatus == USBKeyVerificationFailed) //��֤��ͨ��
	{
		//setLabelLogStatusText(QStringLiteral("�������ȷ����Կ"), "", false, false);
		//�л�ͼƬ
		QString curPath = qApp->applicationDirPath();
		QPixmap map(":/GLExtensionLogin/Icons/noUsbkey.png");
		m_Controls.lblInsertUsbkeyIcon->setPixmap(map);
		m_Controls.labelInsertUsbkey->setText(QStringLiteral("δ������Կ"));
	}
	else if (usbkeyStatus == USBKeyNone) //û�м�⵽��Կ
	{
		//setLabelLogStatusText(QStringLiteral("�������Կ"), "", false);
	//�л�ͼƬ
		QString curPath = qApp->applicationDirPath();
		QPixmap map(QString(":/GLExtensionLogin/Icons/noUsbkey.png"));
		m_Controls.lblInsertUsbkeyIcon->setPixmap(map);
		m_Controls.labelInsertUsbkey->setText(QStringLiteral("δ������Կ"));
	}
}

void ZZLoginView::ResetLastOperate() {
	//return;
	//if (!GLORecord->isHaveRecord) return;
	//QMessageBox messageBox(QMessageBox::NoIcon,
	//	QStringLiteral("�Զ��ָ�"), QStringLiteral("�����ϴ�����ر�, �Ƿ�ָ��ϴβ���?"),
	//	QMessageBox::Yes | QMessageBox::No, NULL); ;
	//messageBox.setButtonText(QMessageBox::Yes, QStringLiteral("��"));
	//messageBox.setButtonText(QMessageBox::No, QStringLiteral("��"));
	//int result = messageBox.exec();


	//switch (result)
	//{
	//case QMessageBox::Yes:
	//	qDebug() << "Yes";
	//	GLORecord->isUseRecord = true;
	//	m_Controls.lineEdit_USER->setText(GLORecord->GetStringByKey("Account"));
	//	m_Controls.lineEdit_PASSWORD->setText(GLORecord->GetStringByKey("Password"));
	//	m_Controls.checkUserAgreement->setChecked(true);
	//	confirm();
	//	break;
	//case QMessageBox::No:
	//	qDebug() << "NO";
	//	GLORecord->isUseRecord = false;
	//	GLORecord->DeleteJsonFile();
	//	break;
	//default:
	//	break;
	//}
}

void ZZLoginView::GetVarConfig()
{
	//QString filename = qApp->applicationDirPath() + BasicConfigData::VarFileName;
	//GLCommonXML *xml = new GLCommonXML(filename);
	//BasicConfigData::AutoTest = xml->GetBoolInfo("AutoTest");
	//BasicConfigData::FiberICCTimeLimited = xml->GetBoolInfo("FiberICCTimeLimited");
	//BasicConfigData::FirstStep = xml->GetIntInfo("FirstStep");
	//BasicConfigData::LaserTempLowerLimit = xml->GetIntInfo("LaserTempLowerLimit");
	//BasicConfigData::LaserTempUpperLimit = xml->GetIntInfo("LaserTempUpperLimit");
	//BasicConfigData::LaserStopTemperature = xml->GetIntInfo("LaserStopTemperature");
	//GetTestCaseConfig(xml->GetIntInfo("TestCase"));
}

void ZZLoginView::GetTestCaseConfig(int testCase)
{
	//if (!BasicConfigData::AutoTest) return;
	//QString tmpStr = QString("/Config/%1.xml").arg(testCase);
	//QString filePath = qApp->applicationDirPath() + tmpStr;
	//QFile file(filePath);
	//if (!file.exists()) return;
	//BasicConfigData::TestCaseXML = new GLCommonXML(filePath);

	//MITK_CUSTOM << QString("test case %1 start").arg(testCase);
}
void ZZLoginView::hideLoginStatusLabel()
{
    labelLoginStatus->hide();
}