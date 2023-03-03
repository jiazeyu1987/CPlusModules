#ifndef PATIENTNEWDIALOG_H
#define PATIENTNEWDIALOG_H

#include <QDialog>
#include "qGLPatientseri.h"
#include "qGLPatientMangeView.h"

namespace Ui {
class PatientNewDialog;
}

class PatientNewDialog : public QDialog
{
    Q_OBJECT

signals:
    void sig_infoLog(QString txt);
    void sig_errorLog(QString txt);
    void sig_customLog(QString txt);

public:
    explicit PatientNewDialog(QVector<Patient> patientVec, PatientsAnalyse pa, QWidget* parent = nullptr);
	  
    ~PatientNewDialog();
signals:
	void sig_PatientSeriConfirmClicked(QStringList, PatientsAnalyse pa);

private:
	void paintEvent(QPaintEvent *event);

protected Q_SLOTS:
	void onConfirm(); //最终确认
	void onComboBoxIllnessKindSelect(int); //疾病种类选择
	//void on_comboBox_currentIndexChanged(const QString &arg1);
private Q_SLOTS:
  //添加备注信息
  void onBtnAddIconClicked();

private:
    Ui::PatientNewDialog* ui;
    QLineEdit* editDiseases;
    QVector<Patient> patientVec;
    PatientsAnalyse pa;
};

#endif // PATIENTNEWDIALOG_H
