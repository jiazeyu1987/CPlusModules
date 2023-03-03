#include "DicomConflictEliminate.h"
#include "qpainter.h"
//#include "GLCommonTool.h"

DicomConfilctEliminate::DicomConfilctEliminate(QVector<Patient> &comboxDataVec, Patient &result, QString desc, bool* acceptOrCancel, QWidget *parent)
    :QDialog(parent), comboxDataVec(comboxDataVec), result(result), acceptOrCancel(acceptOrCancel)
{
    ui.setupUi(this);
    //������Ϊʲô�����߱���ͬʱ�ã�����Ҫô��͸����Ҫô������ʾ��������������ʾ��
    setWindowFlags(Qt::FramelessWindowHint);          //���ش��ڱ�����
    setWindowModality(Qt::ApplicationModal);          //����������
    setAttribute(Qt::WA_TranslucentBackground, true); //����͸��
    ui.lblTitle->setText(QStringLiteral("���߹���"));
    ui.label->adjustSize();
    ui.label->setWordWrap(true);
    ui.label->setText(desc);
    initCombox();
    *acceptOrCancel = false;
    connect(ui.pBtnOK2, &QPushButton::clicked, this, &DicomConfilctEliminate::onAccepted);
    connect(ui.pBtnCancel, &QPushButton::clicked, this, &DicomConfilctEliminate::close);

    exec();
}

void DicomConfilctEliminate::initCombox()
{
    ui.comboName->clear();
    ui.comboBirthday->clear();
    ui.comboSex->clear();
    for (int i = 0; i < comboxDataVec.length(); i++)
    {
        ui.comboName->addItem(comboxDataVec.at(i).patientName);  //GLCommonTool::GetDisplayPatientName(comboxDataVec.at(i).patientName)
        ui.comboBirthday->addItem(comboxDataVec.at(i).birthday);
        ui.comboSex->addItem(comboxDataVec.at(i).sex);
//        ui.comboHeight->addItem(comboxDataVec.at(i).height);
//        ui.comboWeight->addItem(comboxDataVec.at(i).weight);
    }
}

void DicomConfilctEliminate::onAccepted()
{
    result.patientName = ui.comboName->currentText();
    result.birthday = ui.comboBirthday->currentText();
    result.sex = ui.comboSex->currentText();
    //result.height = ui.comboHeight->currentText();
    //result.weight = ui.comboWeight->currentText();
    *acceptOrCancel = true;
    accept();
}

void DicomConfilctEliminate::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 180));
}
