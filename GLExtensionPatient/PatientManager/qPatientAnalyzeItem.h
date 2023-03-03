#ifndef Patient_Analyse_Item_h
#define Patient_Analyse_Item_h
#include "qtimer.h"
#include "qwidget.h"
#include "qGLCommonType.h"
#include "ui_qPatientAnalyzeItem.h"


class PatientAnalyseItem : public QWidget {
	Q_OBJECT
public:
	PatientAnalyseItem(QWidget *parent = nullptr);
	~PatientAnalyseItem();

	void SetInfo(PatientsAnalyse pa);
private slots:
	void OnOpenClick();

signals:
	void OpenAnalyByIndex(PatientsAnalyse pa);

private:
	PatientsAnalyse pa;
	int clickTimes = 0;


	QTimer *timer;
	Ui::PatientAnalyseItem ui;
};
#endif // !Patient_Analyse_Item_h
