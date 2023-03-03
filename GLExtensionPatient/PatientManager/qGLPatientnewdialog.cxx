#include "qGLPatientnewdialog.h"
#include "ui_qGLPatientnewdialog.h"
#include <QPainter>
#include "qGLDb.h"
#include "qGLTool.h"

extern int gdata5;

QString errortype;

PatientNewDialog::PatientNewDialog(QVector<Patient> patientVec, PatientsAnalyse pa, QWidget* parent)
    : QDialog(parent), pa(pa), ui(new Ui::PatientNewDialog), patientVec(patientVec)
{
    emit sig_infoLog("���½�����-���캯��");
    ui->setupUi(this);

    //���ð�͸������ by zbs
    this->setWindowFlags(Qt::FramelessWindowHint);          //���ش��ڱ�����
    this->setWindowModality(Qt::ApplicationModal);          //����������
    this->setAttribute(Qt::WA_TranslucentBackground, true); //����͸��

    ui->doctorRemark->hide();

    onComboBoxIllnessKindSelect(0);

    QImage* img = new QImage;
    GLTool tool;
    img->load(tool.imagesDir + "/titlebg.png");
    img->scaled(ui->label_title->size(), Qt::KeepAspectRatio);
    ui->label_title->setScaledContents(true);
    ui->label_title->setPixmap(QPixmap::fromImage(*img));

    QImage* img2 = new QImage;
    img2->load(tool.imagesDir + "/onlybg.png");

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onConfirm())); //ȷ��
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIllnessKindSelect(int))); //��������
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close())); //ȡ��
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(close())); //ȡ��
    //��ȡ�Ա���ߣ����أ����������
    ui->label_name->setText(pa.patientName.split(":")[0].trimmed()); //��ʱȡ��һ���ɣ�������GLCommonTool::GetDisplayPatientName(pa.patientName)
    ui->label_birthday->setText(pa.birthday);
    
    ui->label_sex->setText(pa.sex);
    ui->label_age->setText(pa.age);

    connect(ui->btnAddIcon, &QPushButton::clicked, this, &PatientNewDialog::onBtnAddIconClicked); //��ӱ�ע
}

PatientNewDialog::~PatientNewDialog()
{
    emit sig_infoLog("�½�����-�������� ");
    delete ui;
}

//�����ػ�
void PatientNewDialog::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QColor(0, 0, 0, 150));  //QColor���һ��������������͸����
}

void PatientNewDialog::onConfirm()
{
    emit sig_infoLog("�½�����-ȷ��ѡ��");
    //���½����ݴ������ݿ�
    pa.analyseName = ui->lineEdit->text().trimmed();
    if (pa.analyseName.size() > 10)
    {
        //GLMessageDialog::ShowInformation(QStringLiteral("�½�����"), QStringLiteral("�������Ʋ��ܳ���10���֣�"));
        GLMessageBox msg;
        msg.queRenWarn(QStringLiteral("�½�����"), QStringLiteral("�������Ʋ��ܳ���10���֣�"));
        return;
    }
    pa.illnessKind = ui->comboBox->currentText().trimmed();
    pa.doctorRemark = ui->doctorRemark->toPlainText();

    if (pa.doctorRemark.size() > 150)
    {
        //GLMessageDialog::ShowInformation(QStringLiteral("�½�����"), QStringLiteral("��ע���ܳ���150���֣�"));
        GLMessageBox msg;
        msg.queRenWarn(QStringLiteral("�½�����"), QStringLiteral("��ע���ܳ���150���֣�"));
        return;
    }
    //�ӻ��߷������ҵ���ص����У���ͨ�����߷���id�ӻ��߷��������в��ң������ǻ���id��ʵ��������Ļ���id�ǲ���Ҫ��
    GLDb db(this);
    if (!db.createDbConnect())
        return;

    db.setPatientsAnalyse(pa);
    for (int i = 0; i < patientVec.size(); i++) //�����߷���id�ͻ���id�������飬�浽��ͬ����
    {
        PatientAnalyseIdPatientIdId patientAnalyseIdPatientIdId;
        patientAnalyseIdPatientIdId.patientAnalyseId = pa.patientAnalyseId; //���߷���id
        patientAnalyseIdPatientIdId.patientId = patientVec.at(i).patientId; //����id
        db.setPatientAnalyseIdPatientIdId(patientAnalyseIdPatientIdId);
    }

    PatientSeri* patientSer = new PatientSeri(patientVec, pa, db); //��������
    connect(patientSer, &PatientSeri::end_success, this, &PatientNewDialog::sig_PatientSeriConfirmClicked);
    close();
    patientSer->exec();
}

void PatientNewDialog::onComboBoxIllnessKindSelect(int index)
{
    emit sig_customLog("�½�����-ѡ�񼲲�������ʾ��ͬ��ͼƬ��������" + QString::number(index));
    if (index == 0)
    {
        QImage *img = new QImage;
        img->load(":/GLExtensionPatient/Icons/1.png");
        img->scaled(ui->label_type->size(), Qt::KeepAspectRatio);
        ui->label_type->setScaledContents(true);
        ui->label_type->setPixmap(QPixmap::fromImage(*img));
        ui->comboBox->setEditable(false);
    }
    else if (index == 1)
    {
        QImage *img = new QImage;
        img->load(":/GLExtensionPatient/Icons/2.png");
        img->scaled(ui->label_type->size(), Qt::KeepAspectRatio);
        ui->label_type->setScaledContents(true);
        ui->label_type->setPixmap(QPixmap::fromImage(*img));
        ui->comboBox->setEditable(false);
    }
    else if (index == 2)
    {
        QImage *img = new QImage;
        img->load(":/GLExtensionPatient/Icons/3.png");
        img->scaled(ui->label_type->size(), Qt::KeepAspectRatio);
        ui->label_type->setScaledContents(true);
        ui->label_type->setPixmap(QPixmap::fromImage(*img));
        ui->comboBox->setEditable(false);
    }
    else if (index == 3)
    {
        QImage *img = new QImage;
        img->load(":/GLExtensionPatient/Icons/4.png");
        img->scaled(ui->label_type->size(), Qt::KeepAspectRatio);
        ui->label_type->setScaledContents(true);
        ui->label_type->setPixmap(QPixmap::fromImage(*img));
        ui->comboBox->setEditable(false);
    }
    else if (index == 4)
    {      
        editDiseases = new QLineEdit; //����ÿ�ζ���ʼ��һ���������������²�����ΪQComboBox���л���Ŀ��ʱ�򣬻Ὣ���ͷŵ�
        editDiseases->setStyleSheet("background-color: #22305B");
        ui->comboBox->setLineEdit(editDiseases);
		editDiseases->clear();
        editDiseases->setPlaceholderText(QStringLiteral("�����뼲������"));
        ui->label_type->clear();
    }
}

void PatientNewDialog::onBtnAddIconClicked()
{
  emit sig_customLog("�½�����-��ӱ�ע��Ϣ ");
  QRect rect = ui->groupBox->geometry();
  ui->btnAddIcon->setAttribute(Qt::WA_DeleteOnClose, true);
  ui->btnAddIcon->hide();
  ui->label->hide();
  //QVBoxLayout *layout = new QVBoxLayout;
  //layout->addWidget(ui->doctorRemark);
  ui->doctorRemark->clear();
  ui->doctorRemark->show();
  ui->doctorRemark->setFocus();
  QFont font(ui->doctorRemark->fontFamily(), 16);
  ui->doctorRemark->setFont(font);
 // ui->groupBox->setContentsMargins(0, 0, 0, 0);
  //ui->groupBox->setLayout(layout);
}