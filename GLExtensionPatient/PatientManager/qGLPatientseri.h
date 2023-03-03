#ifndef PATIENTSERI_H
#define PATIENTSERI_H

#include <QDialog>
#include <QTableWidgetItem>
#include <qGLCommonType.h>
#include "qGLDb.h"

namespace Ui {
class PatientSeri;
}

class PatientSeri : public QDialog
{
    Q_OBJECT
		enum 
		{
			IsCheck, SequenceNumber, ScanTime, SequenceDescribe, Modalities, SliceCount
		};
public:
    explicit PatientSeri(QVector<Patient> patientVec, PatientsAnalyse pa, GLDb& db, QWidget* parent = nullptr);
    ~PatientSeri();

private:
	void paintEvent(QPaintEvent * event);
	//初始化表格
	void initTable();
	

Q_SIGNALS:
	void end_success(QStringList, PatientsAnalyse pa);
	void sig_infoLog(QString txt);
	void sig_customLog(QString txt);
	void sig_errorLog(QString txt);

protected Q_SLOTS:
	void onConfirmSeries();

private slots:
	/*
	 * 目标：单击序列表的第一列时，如果之前选中，则不选中，如果之前没有选中，则勾选
	 */
	void slot_itemColumn0Clicked(QTableWidgetItem * item);
	void onCancel();
	void onCancelOrSelectAll(int column);
private:
	Ui::PatientSeri* ui;
	QIcon icon; //选中icon
	QIcon iconUnSel; //未选中
	PatientsAnalyse pa; //保留新建分析对话框传入的病人分析表
	int checkRole = Qt::UserRole + 1;
	int patientsSequenceListIdRole = Qt::UserRole + 2; //因为表格没有患者序列清单id，所以存在其他角色，第一列的
	int filepathRole = Qt::UserRole + 3;
	QVector<PatientsSeries> patientsSeriesVec; //保存患者序列
	QStringList seriesDirs; //保存图片路径
	QVector<Patient> patientVec;
	GLDb& db;	
};

#endif // PATIENTSERI_H
