#pragma once

#include <qdialog.h>
#include "ui_dicomConflictEliminate.h"
#include "qGLCommonType.h"

class DicomConfilctEliminate : public QDialog
{
    Q_OBJECT
public:
    DicomConfilctEliminate(QVector<Patient> &comboxDataVec, Patient &result, 
        QString desc, bool* acceptOrCancel, QWidget *parent = nullptr);

private:
    void initCombox();
    void onAccepted();
    void paintEvent(QPaintEvent *event);
private:
    Ui::DicomConfict ui;

    QVector<Patient> &comboxDataVec;
    Patient &result;
    bool* acceptOrCancel; //1表示接收，0表示取消
};
