#pragma once

#include <qobject.h>
//#include "org_gl_comm_Export.h"
//#include "internal/dbCommonType.h"
#include <qtoolbutton.h>
#include <qprocess.h>
//#include "GLMessageBox.h"
#include <qfileinfo.h>
#include "qSlicerTopMenuModuleExport.h"

enum PathType
{
  File, Dir
};

typedef struct
{
  QString versionRelase; //发行版本号
  QString versionComplete; //完整版本号
  QString installationDateTime; //安装时间
  QString registerName = QStringLiteral("杭州佳量医疗科技有限公司"); //注册人名称
  QString address = QStringLiteral("杭州市余杭区五常街道文一西路998号(海创园)18幢502室"); //地址
  QString contact = "0571-88621569"; //联系方式
  QString copyright = QStringLiteral("Copyright &#169; 2021杭州佳量医疗科技有限公司. All Rights Reserved");
  QString licenseKey; //许可证号
} SoftwareInfo;

class Q_SLICER_QTMODULES_TOPMENU_EXPORT qSlicerTopMenuTool : public QObject
{
  Q_OBJECT
public:
  qSlicerTopMenuTool() {};
  ~qSlicerTopMenuTool() {};

  static bool createPath(QString path, ::PathType type);
  static void setPatientName(QString patientName);
  //static void setSoftwareoperationrecord(SoftwareOperationRecord& record);
  //顶部工具条-问号-齿轮-箭头
  static QWidget *topToolBar(int x, int y, int width, int height, bool autotest, QWidget *parent = nullptr);
  //读取注册表，获取版本，安装时间等
  static SoftwareInfo getSoftwareInfo();
  //退出应用程序
  static void Exit(QString cause, bool autotest);
  static void ShowChangePassword();

  static QString curDir; //当前目录
  static QString patientName; //患者姓名
  static QString imagesDir; //图片位置
  static QString patientDataDir; //患者数据
  static QString fiberDir; //光纤
  static int calAge(QString date); //计算年龄,Date格式必须是2021/05/21，即yyyy/MM/dd

  static QToolButton *exitBtn; //退出按钮
  static QToolButton *saveBtn; //设置-齿轮
  static QToolButton *exitBtn1; //退出分析按钮

  static QFileInfoList fileInfoList;
  static QFileInfoList getFileInfoFromDir(QString dir);

private:
  //数据库备份
  static void databackup();

  static QToolButton* createBtn(QString icon, QString label, QSize size);
  static void createHelpMenu();
  static void createSetMenu();

private:
  static QWidget *toolBar;
  static QToolButton *setUpBtn; //设置-齿轮
  static QToolButton *helpBtn; //帮助-问号

  static QProcess myProcess; // 数据库备份进程
  //static GLMessageBox msg; 
};