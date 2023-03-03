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
    //目标：导入患者数据
	void slot_btnLoadClick();

    //患者搜索
    void slot_btnSearch();

public Q_SLOTS:
	void onCellDoubleClicked(int);
	void highlight(int);
	void showScreetshotAndReport(int); //显示相关患者包含序列的缩略图和报告
    void finished_success(QStringList, PatientsAnalyse pa);
    void load_package_file(int pid);
protected:
  void CreateQtPartControl(QWidget *parent);
  
  Ui_patientMange& m_Controls;

  void labelclear();

private:
  //初始化患者表格
  void loadPatientsToTableFromDb();
  //初始化最近分析
  void loadRecentlyAnalysisFromDb();
  //创建最近分析窗体
  void createRecentlyAnalysisWidget(QWidget *parent, QString diseaseName, QString analysisName, QString patientName,
    QString sex, QString birthday, QString remark, QString createTime);
  
  bool eventFilter(QObject* obj, QEvent *event) override;
  
  //最近分析按钮-打开最近分析
  void onPushButtonXDoubleClicked(QPushButton* btn);

  //获取所有选中的行
  QVector<int> getSelectedRow();
  //获取最近一次导入目录
  QString getRecentDir();
  //保存最近的目录
  void saveRecentDir(QString dir);
  //设置按钮双击效果
  void setButtonDoubleClicked(QPushButton* btn, int i); //i表示第几个
  //获取新建分析相关数据
  PatientsAnalyse getNewAnalysis(QVector<Patient>, bool* acceptOrCancel);

  bool CopyDir(const QString& source, const QString& destination, bool override, GLCustomProgressBar* bar);
private slots:
    //设置手术纪要可编辑或者展示图片和报告/分析
    void onItemClicked(QTableWidgetItem *item);
    //当手术纪要编辑完之后，存入数据库
    void onOperationSummaryItemChanged(QTableWidgetItem *item);
    void onOpenAnalySe(PatientsAnalyse analy);
    //合并选中行
    void slot_actionMergeRowTriggered();
    //分离选中行
    void onActionSeparateRowTriggered();
    //右键菜单（合并和分离）
    void slot_tableWidgetCustomContextMenuRequested(const QPoint &pos);
    //排序
    void onSort(int row);
    //调整列宽等宽
    void resetColumnWidth();
    //打开分析报告
    void onOpenReport();
    //打开mitk文件
    void OnLoadMitkClick();
private:
    qSlicerPatientManagerModuleWidget* patientManagerMoudleWidget;
    QWidget *parentWidget;

    QWidget* widgetAll;
    QVector<QWidget*> labelAllVec; //显示患者相关图片和报告（全部）
    QHBoxLayout *layoutAll;
    QSpacerItem *spacerItemAll;
    QWidget* widgetPicture;
    QVector<QLabel *> labelPictureVec; //显示患者相关的图片
    QHBoxLayout *layoutPicture;
    QSpacerItem *spacerItemPicture;
    QWidget* widgetReport;
    QVector<QWidget*> labelAnalyseReportVec; //显示患者相关的分析/报告
    QHBoxLayout *layoutReport;
    QSpacerItem *spacerItemReport;

    QMenu *pop_menu;
    QAction *actionMergeRow; //表格右键合并
    QAction *actionSeparate; //表格右键分离

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
