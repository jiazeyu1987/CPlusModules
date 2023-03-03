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
#pragma comment(lib, "./Debug/libhasp_windows_x64_33652.lib") // 数字正式锁，每把主锁一个数字，也就是每把主锁都会下载一个库
#else
//#pragma comment(lib, "E:\\S\\GLModule\\RRBuild_GLExtensionLogin\\Login\\Release/libhasp_cpp_windows_x64_mtd_msc10.lib")
#pragma comment(lib, "./Release/libhasp_cpp_windows_x64_mtd_msc10.lib")
#pragma comment(lib, "./Release/libhasp_windows_x64_33652.lib") // 数字正式锁，每把主锁一个数字，也就是每把主锁都会下载一个库
#endif // _DEBUG

//#pragma comment是工程所在的目录为当前地址，所谓工程目录，简单点就是在vs，右键点击该工程，然后在资源浏览器中打开，打开的文件夹就是当前目录
//#pragma comment(lib, "E:/MK/abc/Plugins/AuxiliaryLib/lib_usbKey/libhasp_windows_x64_demo.lib") // demo测试锁


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

//#include <vld.h> //内存泄漏检测

HistoryNameWidget::HistoryNameWidget(QWidget *parent)
  :QDialog(parent)
{
  layoutMain = new QVBoxLayout;
  setWindowFlag(Qt::FramelessWindowHint);
  setLayout(layoutMain);
  layoutMain->setSpacing(0);
  layoutMain->setMargin(0);
  //layoutMain->addStretch();

  //设置背景为白色
  QString qss
  (
    "background-color: rgb(255, 255, 255);"
    "border-style: none;"
    "color: rgb(0, 0, 0);"
    "border-radius: 0px;"
  );
  setStyleSheet(qss);

  //设置位置和固定宽度
  QLabel* line_userName = parent->findChild<QLabel*>("label_3");
  if (line_userName)
  {
      int y = line_userName->y();
      int x = line_userName->x();
      setGeometry(x, y, 360, 1); //不知道为什么x，y不可以，需要向上29px
      raise();
  }
    
  setFixedWidth(360);

  hide();
}

void HistoryNameWidget::addRow(QString doctorName, QString username)
{
	QWidget* widgetRow = new QWidget; //一行
	widgetRow->setFixedSize(360, 42); //固定宽和高，意思就是每一行总的宽高已经限死

	QPushButton* btnHistroyName = new QPushButton; //历史用户名
	btnHistroyName->setObjectName("btnRow");
	btnHistroyName->setFixedHeight(42);

	QPushButton* btnDel = new QPushButton; //删除用户名
	btnDel->setFixedSize(13, 13);
	btnDel->setIcon(QIcon(":/GLExtensionLogin/Icons/ZZLoginViewControls_delHistoryName.png"));

	//布局历史用户名按钮
	QString qssLabel
	(
		"border-color: rgb(255, 0, 0);"
	);

    //医生姓名
	QLabel* labelDoctorName = new QLabel(doctorName);
	labelDoctorName->setFixedHeight(42);
	labelDoctorName->hide(); //医生姓名隐藏，目前没有使用
	labelDoctorName->setStyleSheet(qssLabel);
	labelDoctorName->setObjectName("labelDoctorName");

    //用户名
	QLabel* labelUsername = new QLabel(username);
	labelUsername->setFixedHeight(42);
	labelUsername->setStyleSheet(qssLabel);
	labelUsername->setObjectName("labelUsername");

	//标签布局
	QFormLayout* layout = new QFormLayout;
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addRow(labelDoctorName, labelUsername);
	//按钮布局
	QHBoxLayout* layoutBtn = new QHBoxLayout;
	layoutBtn->setMargin(0);
	layoutBtn->addSpacing(30);
	layoutBtn->addLayout(layout);
	layoutBtn->addSpacing(60);
	//设置布局给按钮
	btnHistroyName->setLayout(layoutBtn);

	//整行总布局
	QHBoxLayout* layoutRow = new QHBoxLayout;
	layoutRow->setMargin(0);
	layoutRow->addWidget(btnHistroyName);
	layoutRow->addWidget(btnDel);
	layoutRow->addSpacing(15);

	//设置到整行
	widgetRow->setLayout(layoutRow);

	//添加到主布局
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

	connect(btnDel, &QPushButton::clicked, this, [=]() //删除历史用户名
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

    //软件用户协议界面
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
    //不知道为什么整体向左下移动，是个必须解决的问题
    m_Controls.setupUi(parent);

 /*   QString fileName = QString("%1/%2/%3/qss/glbluestyle.qss");
    setQss(fileName);*/

   // m_Controls.checkUserAgreement->setEnabled(false);

    m_Controls.lineEdit_USER->setFocusPolicy(Qt::StrongFocus);
    m_Controls.lineEdit_USER->installEventFilter(this);

    //设置密码模式
    m_Controls.lineEdit_PASSWORD->setEchoMode(QLineEdit::Password);
    m_Controls.btn_CLOSE->setCursor(QCursor(Qt::PointingHandCursor));
    m_Controls.btnUserAgreement->setCursor(QCursor(Qt::PointingHandCursor));

    //登录状态
    labelLoginStatus = new QLabel(QStringLiteral("<font color='black'>登录状态</font>"), parent);
    labelLoginStatus->setGeometry(1111, 380, 360, 48); //设置位置
    //labelLogStatus->setStyleSheet("background:opaque"); //显示不透明
    labelLoginStatus->setStyleSheet("background:white");
    labelLoginStatus->hide();

    //软件用户协议
    connect(m_Controls.btnUserAgreement, &QPushButton::clicked, this, &ZZLoginView::slot_btnUserAgreementClicked);
    //隐私策略
    connect(m_Controls.btnPrivacyPolicy, &QPushButton::clicked, this, &ZZLoginView::onBtnPrivacyPolicyClicked);
    //定时检测加密狗
    timer.callOnTimeout(this, &ZZLoginView::slot_timerTimeOut, Qt::QueuedConnection);
    timer.start(1000);
    slot_timerTimeOut(); //立刻开始检查USBKey状态
    //登录
    connect(m_Controls.btn_OK, SIGNAL(clicked()), this, SLOT(confirm()));
    //退出
    connect(m_Controls.btn_CLOSE, &QPushButton::clicked, this, [=]()
    {
        emit sig_infoLog("用户点击退出");
        GLTool tool;
        tool.Exit(QStringLiteral("退出登录"), false);
    });

    //SetDelayFunction(500, ""); //最终调用虚函数OnTimerOutFunction，500ms之后调用

    //设置Tab顺序
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
        emit sig_infoLog("输入用户名" + m_Controls.lineEdit_USER->text());
        //编辑完成，更换图片
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
		emit sig_infoLog("输入用户名" + m_Controls.lineEdit_PASSWORD->text());
        //编辑完成，更换图片
        if (!m_Controls.lineEdit_PASSWORD->text().isEmpty())
            m_Controls.lblPassword->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputPassword.png"));
        else
            m_Controls.lblPassword->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputPasswordNone.png"));
    });
    //历史用户名
    historyNameWidget = new HistoryNameWidget(parent);
    connect(historyNameWidget, &HistoryNameWidget::sigUsername, this, [=](QString username)
    {
		emit sig_infoLog("选择历史用户名" + username);
        m_Controls.lineEdit_USER->setText(username);
        if (!m_Controls.lineEdit_USER->text().isEmpty())
            m_Controls.lblUser->setPixmap(QPixmap(":/GLExtensionLogin/Icons/inputUsername.png"));
        historyNameWidget->hide();
        AutoCheckAgreement(m_Controls.lineEdit_USER->text());
    });
    connect(historyNameWidget, &HistoryNameWidget::sigDelUsername, this, [=](QString username)
    {
		emit sig_infoLog("删除历史用户名" + username);
        usernameList.removeAll(username);
        setCurrentLoginUsername("");
    });

    usernameList = getPreviousLoginUsername();
    for (int i = 0; i < usernameList.size(); i++)
    {
        QString username = usernameList.at(i);
        //从数据库获取对应的用户的真实信息
        GLDb db;
        if (!db.createDbConnect())
        {
            emit sig_infoLog("数据库连接失败，不能获取用户名的真实姓名");
        }
        else
        {
            //目前医生姓名并没有显示到历史用户中，所以不获取
        }
        historyNameWidget->addRow(QStringLiteral("姜医生"), username);
    }

    connect(m_Controls.btnHistoryName, &QPushButton::clicked, this, [=](bool checked)
    {
        m_Controls.btnHistoryName->setIcon(QIcon(":/GLExtensionLogin/Icons/drowDownHistoryName.png"));
        emit sig_customLog("点击历史用户名");
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
        //从数据库获取对应的用户的真实信息
        GLDb db;
        if (!db.createDbConnect())
        {
            emit sig_infoLog("数据库连接失败，不能获取用户名的真实姓名");
        }
        else
        {

        }
        historyNameWidget->addRow(QStringLiteral("姜医生"), username);
    }

    connect(m_Controls.btnHistoryName, &QPushButton::clicked, this, [=](bool checked)
    {
        emit sig_customLog("点击历史用户名");
        if (checked)
        {
            historyNameWidget->show();
        }
        if (!checked)
        {
            historyNameWidget->hide();
        }
    });
    setSelfDefToolBar(); //必须放这里，因为在这里有些控件在初始化完成

    emit sig_nextPage();
    //是否意外恢复
    ResetLastOperate();
}

void ZZLoginView::slot_btnUserAgreementClicked()
{
    int i=GLPopWindows::ShowInformation(QStringLiteral("服务协议"), PluginConst::ProtocolInfo,true);
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
	set.setIniCodec(QTextCodec::codecForName("GBK")); //防止中文乱码
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
	//MITK_INFO << "从配置文件获取上一次用户名";
	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //防止中文乱码
	QStringList usernameList = set.value("Login/rememberUsername").toStringList();
	usernameList.removeDuplicates();
	return usernameList;
}

void ZZLoginView::setCurrentLoginUsername(QString username)
{
	emit sig_infoLog("将当前用户名加入到配置文件保存");
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
	set.setIniCodec(QTextCodec::codecForName("GBK")); //防止中文乱码
	set.setValue("Login/rememberUsername", usernameList);
}

bool ZZLoginView::getSoftwareUserAgreementShow()
{
	emit sig_infoLog("从配置文件中获取是否需要展示初次软件用户协议标志");
	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //防止中文乱码
	bool isShow = set.value("Login/softwareUserAgreementShow").toBool();
	
	return isShow;
}

void ZZLoginView::setSoftwareUserAgreementShow(bool isShow)
{
	emit sig_infoLog("设置是否需要展示初次软件用户协议标志到配置文件");
	QString curPath = qApp->applicationDirPath();
	QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
	set.setIniCodec(QTextCodec::codecForName("GBK")); //防止中文乱码
	set.setValue("Login/softwareUserAgreementShow", isShow);
	
}

LoginStatus ZZLoginView::usbkeyVerification()
{
	//当新的USB Key导入时，会产生新的vendorCode，替换原来的即可
	//unsigned char vendorCode[] = //测试Code
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

	haspStatus status; //状态，没有检测到USB Key，返回0x0007，插入错误的USB Key，返回0x0031，正确返回0

	//MITK_INFO << "登录到feature";
	//42是USB Key的featureId，每个USB Key可以存储若干个id，id可以通过ems烧录进去，通过浏览器可以查看id
	//USB Key通过vendorCode检测是否插入正确的USB Key，有通过feature Id检查，程序是否匹配正确的id
	//ChaspFeature feature4 = ChaspFeature::fromFeature(42); 
	ChaspFeature feature = ChaspFeature::fromFeature(1);
	//此xml字符串就是获取不同功能的接口
	//const char* localScope =
	//  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	//  "<haspscope>"
	//  "    <license_manager hostname =\"localhost\" />"
	//  "</haspscope>";
	Chasp hasp(feature);
	//status = hasp.login(vendorCode, localScope); //制定不同的会话
	status = hasp.login(vendorCode);

	//MITK_INFO << "获取的状态：" << status;

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
	//MITK_INFO << "检测usbKey的状态:" << usbKeyStatus;
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
        //日志审核
        GLDb db;
        if (!db.createDbConnect())
        {
            QAbstractButton* btn = new QPushButton(QStringLiteral("确认"));
            QPair<QAbstractButton*, QMessageBox::ButtonRole> btnPair(btn, QMessageBox::ButtonRole::AcceptRole);
            QVector<QPair<QAbstractButton*, QMessageBox::ButtonRole>> btnVec(1, btnPair);
            QString title = QStringLiteral("日志审核");
            QString txt = QStringLiteral("数据库连接失败！");
            //GLMessageBox msg(title, txt, btnVec, QMessageBox::Warning);
        }
        else
        {
            SoftwareOperationRecord record;
            record.content = QStringLiteral("登录成功");
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
    emit sig_infoLog("验证用户名和密码,用户名：" + account.userName + "是否初次使用:" + QString::number(account.isFirstUse));

    emit sig_infoLog("检测USB Key");
    if (usbkeyStatus == USBKeyNone || usbkeyStatus == USBKeyVerificationFailed) //首先验证USB密钥的状态
        return usbkeyStatus;

    emit sig_infoLog("USB Key检测成功，检测软件用户协议");
    if (!m_Controls.checkUserAgreement->isChecked()) //未同意软件用户协议
        return SoftwareUserAgreementUnCheck;

    //先验证管理员密码，管理员也需要插入usbKey和同意协议
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

    emit sig_infoLog("软件用户协议检测成功，检测用户名是否为空");
    if (account.userName.isEmpty())
        return UsernameEmpty;

    emit sig_infoLog("用户名不为空，检测用户名长度是否超过30");
    if (account.userName.length() > 30)
        return UsernameFormatError;

    emit sig_infoLog("用户名合理，检测密码是否为空");
    if (account._password.isEmpty())
        return PasswordEmpty;

    emit sig_infoLog("密码不为空，检测密码长度是否小于6或者大于12");
    if (account._password.length() < 6 || account._password.length() > 12)
        return PasswordFormatError;

    //连接数据库，短连接
    emit sig_infoLog("密码不为空，检测用户名和密码是否正确");
    GLDb db(this);
    if (!db.createDbConnect())
    {
        emit sig_infoLog("数据库连接失败，直接退出程序");
        //连接失败，直接退出
        QTimer::singleShot(0, qApp, &QApplication::closeAllWindows);
    }

    return db.login(account);
}

bool ZZLoginView::eventFilter(QObject * obj, QEvent * event)
{
	//MITK_INFO << "事件类型" << event->type();
	//判断事件是键盘Enter事件
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *eventKey = static_cast<QKeyEvent *>(event);
		if (eventKey->key() == Qt::Key_Return || eventKey->key() == Qt::Key_Enter)
		{
            emit sig_customLog("Enter键盘事件，执行登录功能");
			confirm();
			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

bool ZZLoginView::createPath(QString filepath)
{
  //判断路径是否存在
  QFile file(filepath);
  if (file.exists())
    return true;

  int first = filepath.lastIndexOf("/");
  
  //创建目录
  QString dirpath = filepath.mid(0, first);
  QDir dir(dirpath);
  if (!dir.exists())
    dir.mkdir(dirpath);

  //创建文件
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
    int i = GLPopWindows::ShowInformation(QStringLiteral("隐私政策"), PluginConst::PrivacyPolicy, true);
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
  //  GLTool::Exit(QStringLiteral("正常退出"), false);
  //});
}

void ZZLoginView::confirm()
{
    emit sig_infoLog("用户点击登录");
    Account account;
    account.userName = m_Controls.lineEdit_USER->text();
    account._password = m_Controls.lineEdit_PASSWORD->text();
    LoginStatus _status = verify(account);

    switch (_status)
    {
    case VerificationSuccess://成功
    {
        emit sig_infoLog("登录成功");

        //将账号和密码保存到全局的DataStorage
        loginModuleWidget->AddSignalNode("userName", account.userName.toStdString().c_str());
        loginModuleWidget->AddSignalNode("password", account._password.toStdString().c_str());

        //将用户关联到输出pdf的操作人
        GLTool::worker = account.userName;

        bool changedPassword = false;
        if (account.isFirstUse == 1)
        {
            //第一次使用，弹出是否修改密码的对话框
            //QAbstractButton* btnYes = new QPushButton(QStringLiteral("是"));
            //QPair<QAbstractButton*, QMessageBox::ButtonRole> btnYesPair(btnYes, QMessageBox::ButtonRole::YesRole);
            //QAbstractButton* btnNo = new QPushButton(QStringLiteral("否"));
            //QPair<QAbstractButton*, QMessageBox::ButtonRole> btnNoPair(btnNo, QMessageBox::ButtonRole::NoRole);
            //QVector<QPair<QAbstractButton*, QMessageBox::ButtonRole>> btnVec;
            //btnVec << btnYesPair << btnNoPair;
            //QString title = QStringLiteral("修改密码");
            //QString txt = QStringLiteral("是否修改密码");
            //GLMessageBox msg(title, txt, btnVec, QMessageBox::Question);
            MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("修改密码"), QStringLiteral("是否修改密码?"));
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
            //指isFirstUse标志为3
            GLDb db(this);
            if (!db.createDbConnect())
            {
                QTimer::singleShot(0, qApp, &QApplication::closeAllWindows);
            }
            db.setAccountIsFirstUse(account);
        }
        setLabelLogStatusText("", "", true, true, changedPassword);
        //根据勾选的状态，选择是否记住用户名
        //bool isRemember = m_Controls.chkLoginStatus->isChecked();
        if (true)
        {
            setCurrentLoginUsername(account.userName);
        }
        break;
    }
    case USBKeyNone:
        setLabelLogStatusText(QStringLiteral("请插入USB密钥"), "", false, false);
        emit sig_infoLog("请插入USB密钥");
        break;
    case USBKeyVerificationFailed:
        setLabelLogStatusText(QStringLiteral("请插入正确的USB密钥"), "", false, false);
        emit sig_infoLog("请插入正确的USB密钥");
        break;
    case SoftwareUserAgreementUnCheck:
        setLabelLogStatusText(QStringLiteral("请先阅读并同意服务协议和隐私政策"), "", false, false);
        emit sig_infoLog("请先阅读并同意服务协议和隐私政策");
        break;
    case UsernameEmpty:
        setLabelLogStatusText(QStringLiteral("请输入用户名"), "", false, true);
        emit sig_infoLog("请输入用户名");
        break;
    case UsernameFormatError:
        setLabelLogStatusText(QStringLiteral("用户名长度不能超过30！"), "", false, true);
        emit sig_infoLog("用户名长度不能超过30！");
        break;
    case UsernameError:
        setLabelLogStatusText(QStringLiteral("用户名不存在"), "", false, true);
        emit sig_infoLog("用户名不存在");
        break;
    case DataTampered:
        setLabelLogStatusText(QStringLiteral("数据被纂改"), "", false, true);
        emit sig_infoLog("数据被纂改");
        break;
    case PasswordEmpty:
        setLabelLogStatusText(QStringLiteral("请输入密码"), "", false, true);
        emit sig_infoLog("请输入密码");
        break;
    case PasswordFormatError:
        setLabelLogStatusText(QStringLiteral("密码长度不小于6位且不超过12位"), "", false, true);
        emit sig_infoLog("密码长度不小于6位且不超过12位");
        break;
    case PasswordError:
        setLabelLogStatusText(QStringLiteral("用户名或密码错误"), "", false, true);
        emit sig_infoLog("用户名或密码错误");
        break;
    case Undefined:
        break;
    case AdministratorVerificationSuccess:
    {
        //将账号和密码保存到全局的DataStorage
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
  //日志审核
  GLDb db;
  if (!db.createDbConnect())
  {
    QAbstractButton *btn = new QPushButton(QStringLiteral("确认"));
    QPair<QAbstractButton *, QMessageBox::ButtonRole> btnPair(btn, QMessageBox::ButtonRole::AcceptRole);
    QVector<QPair<QAbstractButton *, QMessageBox::ButtonRole>> btnVec(1, btnPair);
    QString title = QStringLiteral("日志审核");
    QString txt = QStringLiteral("数据库连接失败！");
    GLMessageBox msg(title, txt, btnVec, QMessageBox::Warning);
}
  else
  {
    SoftwareOperationRecord record;
    record.content = exitLogin; // QStringLiteral("退出登录")
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
		//切换图片
		QString curPath = qApp->applicationDirPath();
		QPixmap map(QString(":/GLExtensionLogin/Icons/insertUsbKey.png"));
		m_Controls.lblInsertUsbkeyIcon->setPixmap(map);
		m_Controls.labelInsertUsbkey->setText(QStringLiteral("已插入密钥"));
		//停止定时器
		timer.stop();
	}
	else if (usbkeyStatus == USBKeyVerificationFailed) //验证不通过
	{
		//setLabelLogStatusText(QStringLiteral("请插入正确的密钥"), "", false, false);
		//切换图片
		QString curPath = qApp->applicationDirPath();
		QPixmap map(":/GLExtensionLogin/Icons/noUsbkey.png");
		m_Controls.lblInsertUsbkeyIcon->setPixmap(map);
		m_Controls.labelInsertUsbkey->setText(QStringLiteral("未插入密钥"));
	}
	else if (usbkeyStatus == USBKeyNone) //没有检测到密钥
	{
		//setLabelLogStatusText(QStringLiteral("请插入密钥"), "", false);
	//切换图片
		QString curPath = qApp->applicationDirPath();
		QPixmap map(QString(":/GLExtensionLogin/Icons/noUsbkey.png"));
		m_Controls.lblInsertUsbkeyIcon->setPixmap(map);
		m_Controls.labelInsertUsbkey->setText(QStringLiteral("未插入密钥"));
	}
}

void ZZLoginView::ResetLastOperate() {
	//return;
	//if (!GLORecord->isHaveRecord) return;
	//QMessageBox messageBox(QMessageBox::NoIcon,
	//	QStringLiteral("自动恢复"), QStringLiteral("程序上次意外关闭, 是否恢复上次操作?"),
	//	QMessageBox::Yes | QMessageBox::No, NULL); ;
	//messageBox.setButtonText(QMessageBox::Yes, QStringLiteral("是"));
	//messageBox.setButtonText(QMessageBox::No, QStringLiteral("否"));
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