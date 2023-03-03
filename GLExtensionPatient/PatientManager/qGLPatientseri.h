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
	//��ʼ�����
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
	 * Ŀ�꣺�������б�ĵ�һ��ʱ�����֮ǰѡ�У���ѡ�У����֮ǰû��ѡ�У���ѡ
	 */
	void slot_itemColumn0Clicked(QTableWidgetItem * item);
	void onCancel();
	void onCancelOrSelectAll(int column);
private:
	Ui::PatientSeri* ui;
	QIcon icon; //ѡ��icon
	QIcon iconUnSel; //δѡ��
	PatientsAnalyse pa; //�����½������Ի�����Ĳ��˷�����
	int checkRole = Qt::UserRole + 1;
	int patientsSequenceListIdRole = Qt::UserRole + 2; //��Ϊ���û�л��������嵥id�����Դ���������ɫ����һ�е�
	int filepathRole = Qt::UserRole + 3;
	QVector<PatientsSeries> patientsSeriesVec; //���滼������
	QStringList seriesDirs; //����ͼƬ·��
	QVector<Patient> patientVec;
	GLDb& db;	
};

#endif // PATIENTSERI_H
