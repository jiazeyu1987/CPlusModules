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
#ifndef patientMangeView_h
#define patientMangeView_h

#include "qGLPatientnewdialog.h"
#include "qGLPatientseri.h"
#include <qSlicerGLModuleWidget.h>
#include "ui_qGLPatientMangeView.h"
#include "qGLCommonType.h"
#include <qwidget.h>
#include "qGLPatientTable.h"
#include <QHBoxLayout>
#include "qGLTool.h"
class GLCustomProgressBar;
class qSlicerPatientManagerModuleWidget;
class patientMangeView : public QObject
{
  Q_OBJECT

signals:
    void sig_infoLog(QString txt);
    void sig_errorLog(QString txt);
    void sig_customLog(QString txt);
    void sig_nextPage();

public:
  patientMangeView(Ui_patientMange& m_Controls, qSlicerPatientManagerModuleWidget* patientManagerMoudleWidget);
  ~patientMangeView();

  QVector<QLabel*> labellist;

public slots:
    //Ŀ�꣺���뻼������
	void slot_btnLoadClick();

    //��������
    void slot_btnSearch();

public Q_SLOTS:
	void onCellDoubleClicked(int);
	void highlight(int);
	void showScreetshotAndReport(int); //��ʾ��ػ��߰������е�����ͼ�ͱ���
    void finished_success(QStringList, PatientsAnalyse pa);
    void load_package_file(int pid);
protected:
  void CreateQtPartControl(QWidget *parent);
  
  Ui_patientMange& m_Controls;

  void labelclear();

private:
  //��ʼ�����߱��
  void loadPatientsToTableFromDb();
  //��ʼ���������
  void loadRecentlyAnalysisFromDb();
  //���������������
  void createRecentlyAnalysisWidget(QWidget *parent, QString diseaseName, QString analysisName, QString patientName,
    QString sex, QString birthday, QString remark, QString createTime);
  
  bool eventFilter(QObject* obj, QEvent *event) override;
  
  //���������ť-���������
  void onPushButtonXDoubleClicked(QPushButton* btn);

  //��ȡ����ѡ�е���
  QVector<int> getSelectedRow();
  //��ȡ���һ�ε���Ŀ¼
  QString getRecentDir();
  //���������Ŀ¼
  void saveRecentDir(QString dir);
  //���ð�ť˫��Ч��
  void setButtonDoubleClicked(QPushButton* btn, int i); //i��ʾ�ڼ���
  //��ȡ�½������������
  PatientsAnalyse getNewAnalysis(QVector<Patient>, bool* acceptOrCancel);

  bool CopyDir(const QString& source, const QString& destination, bool override, GLCustomProgressBar* bar);
private slots:
    //����������Ҫ�ɱ༭����չʾͼƬ�ͱ���/����
    void onItemClicked(QTableWidgetItem *item);
    //��������Ҫ�༭��֮�󣬴������ݿ�
    void onOperationSummaryItemChanged(QTableWidgetItem *item);
    void onOpenAnalySe(PatientsAnalyse analy);
    //�ϲ�ѡ����
    void slot_actionMergeRowTriggered();
    //����ѡ����
    void onActionSeparateRowTriggered();
    //�Ҽ��˵����ϲ��ͷ��룩
    void slot_tableWidgetCustomContextMenuRequested(const QPoint &pos);
    //����
    void onSort(int row);
    //�����п�ȿ�
    void resetColumnWidth();
    //�򿪷�������
    void onOpenReport();
    //��mitk�ļ�
    void OnLoadMitkClick();
private:
    qSlicerPatientManagerModuleWidget* patientManagerMoudleWidget;
    QWidget *parentWidget;

    QWidget* widgetAll;
    QVector<QWidget*> labelAllVec; //��ʾ�������ͼƬ�ͱ��棨ȫ����
    QHBoxLayout *layoutAll;
    QSpacerItem *spacerItemAll;
    QWidget* widgetPicture;
    QVector<QLabel *> labelPictureVec; //��ʾ������ص�ͼƬ
    QHBoxLayout *layoutPicture;
    QSpacerItem *spacerItemPicture;
    QWidget* widgetReport;
    QVector<QWidget*> labelAnalyseReportVec; //��ʾ������صķ���/����
    QHBoxLayout *layoutReport;
    QSpacerItem *spacerItemReport;

    QMenu *pop_menu;
    QAction *actionMergeRow; //����Ҽ��ϲ�
    QAction *actionSeparate; //����Ҽ�����

    int userRoleReport = Qt::UserRole + 1;
    int userRoleSort = Qt::UserRole + 2;
    const char* reportProperty = "report";
   
    PatientTable *table;
    QHash<QPushButton*, PatientsAnalyse> recentlyAnalysisHash;

    QVector<int> clickTimerVec;
    QVector<QPushButton*> btnVec;
    QVector<QTimer*> timerVec;
};
#endif // MyView_h
