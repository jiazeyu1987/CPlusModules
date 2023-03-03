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
  QString versionRelase; //���а汾��
  QString versionComplete; //�����汾��
  QString installationDateTime; //��װʱ��
  QString registerName = QStringLiteral("���ݼ���ҽ�ƿƼ����޹�˾"); //ע��������
  QString address = QStringLiteral("�������ຼ���峣�ֵ���һ��·998��(����԰)18��502��"); //��ַ
  QString contact = "0571-88621569"; //��ϵ��ʽ
  QString copyright = QStringLiteral("Copyright &#169; 2021���ݼ���ҽ�ƿƼ����޹�˾. All Rights Reserved");
  QString licenseKey; //���֤��
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
  //����������-�ʺ�-����-��ͷ
  static QWidget *topToolBar(int x, int y, int width, int height, bool autotest, QWidget *parent = nullptr);
  //��ȡע�����ȡ�汾����װʱ���
  static SoftwareInfo getSoftwareInfo();
  //�˳�Ӧ�ó���
  static void Exit(QString cause, bool autotest);
  static void ShowChangePassword();

  static QString curDir; //��ǰĿ¼
  static QString patientName; //��������
  static QString imagesDir; //ͼƬλ��
  static QString patientDataDir; //��������
  static QString fiberDir; //����
  static int calAge(QString date); //��������,Date��ʽ������2021/05/21����yyyy/MM/dd

  static QToolButton *exitBtn; //�˳���ť
  static QToolButton *saveBtn; //����-����
  static QToolButton *exitBtn1; //�˳�������ť

  static QFileInfoList fileInfoList;
  static QFileInfoList getFileInfoFromDir(QString dir);

private:
  //���ݿⱸ��
  static void databackup();

  static QToolButton* createBtn(QString icon, QString label, QSize size);
  static void createHelpMenu();
  static void createSetMenu();

private:
  static QWidget *toolBar;
  static QToolButton *setUpBtn; //����-����
  static QToolButton *helpBtn; //����-�ʺ�

  static QProcess myProcess; // ���ݿⱸ�ݽ���
  //static GLMessageBox msg; 
};