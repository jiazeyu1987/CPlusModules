#include "DicomConflictEliminate.h"
#include "qpainter.h"
//#include "GLCommonTool.h"

DicomConfilctEliminate::DicomConfilctEliminate(QVector<Patient> &comboxDataVec, Patient &result, QString desc, bool* acceptOrCancel, QWidget *parent)
    :QDialog(parent), comboxDataVec(comboxDataVec), result(result), acceptOrCancel(acceptOrCancel)
{
    ui.setupUi(this);
    //不明白为什么这三者必须同时用，否则要么不透明，要么不能显示（或者最上面显示）
    setWindowFlags(Qt::FramelessWindowHint);          //隐藏窗口标题栏
    setWindowModality(Qt::ApplicationModal);          //阻滞主窗口
    setAttribute(Qt::WA_TranslucentBackground, true); //设置透明
    ui.lblTitle->setText(QStringLiteral("患者管理"));
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
