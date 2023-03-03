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
    emit sig_infoLog("打开新建分析-构造函数");
    ui->setupUi(this);

    //设置半透明背景 by zbs
    this->setWindowFlags(Qt::FramelessWindowHint);          //隐藏窗口标题栏
    this->setWindowModality(Qt::ApplicationModal);          //阻滞主窗口
    this->setAttribute(Qt::WA_TranslucentBackground, true); //设置透明

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

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onConfirm())); //确定
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIllnessKindSelect(int))); //疾病种类
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close())); //取消
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(close())); //取消
    //获取性别，身高，体重，年龄和生日
    ui->label_name->setText(pa.patientName.split(":")[0].trimmed()); //暂时取第一个吧，真无语GLCommonTool::GetDisplayPatientName(pa.patientName)
    ui->label_birthday->setText(pa.birthday);
    
    ui->label_sex->setText(pa.sex);
    ui->label_age->setText(pa.age);

    connect(ui->btnAddIcon, &QPushButton::clicked, this, &PatientNewDialog::onBtnAddIconClicked); //添加备注
}

PatientNewDialog::~PatientNewDialog()
{
    emit sig_infoLog("新建分析-析构函数 ");
    delete ui;
}

//背景重绘
void PatientNewDialog::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QColor(0, 0, 0, 150));  //QColor最后一个参数代表背景的透明度
}

void PatientNewDialog::onConfirm()
{
    emit sig_infoLog("新建分析-确认选择");
    //将新建数据存入数据库
    pa.analyseName = ui->lineEdit->text().trimmed();
    if (pa.analyseName.size() > 10)
    {
        //GLMessageDialog::ShowInformation(QStringLiteral("新建分析"), QStringLiteral("分析名称不能超过10个字！"));
        GLMessageBox msg;
        msg.queRenWarn(QStringLiteral("新建分析"), QStringLiteral("分析名称不能超过10个字！"));
        return;
    }
    pa.illnessKind = ui->comboBox->currentText().trimmed();
    pa.doctorRemark = ui->doctorRemark->toPlainText();

    if (pa.doctorRemark.size() > 150)
    {
        //GLMessageDialog::ShowInformation(QStringLiteral("新建分析"), QStringLiteral("备注不能超过150个字！"));
        GLMessageBox msg;
        msg.queRenWarn(QStringLiteral("新建分析"), QStringLiteral("备注不能超过150个字！"));
        return;
    }
    //从患者分析中找到相关的序列，是通过患者分析id从患者分析序列中查找，而不是患者id，实际上这里的患者id是不重要的
    GLDb db(this);
    if (!db.createDbConnect())
        return;

    db.setPatientsAnalyse(pa);
    for (int i = 0; i < patientVec.size(); i++) //将患者分析id和患者id绑定在疫情，存到不同的行
    {
        PatientAnalyseIdPatientIdId patientAnalyseIdPatientIdId;
        patientAnalyseIdPatientIdId.patientAnalyseId = pa.patientAnalyseId; //患者分析id
        patientAnalyseIdPatientIdId.patientId = patientVec.at(i).patientId; //患者id
        db.setPatientAnalyseIdPatientIdId(patientAnalyseIdPatientIdId);
    }

    PatientSeri* patientSer = new PatientSeri(patientVec, pa, db); //患者序列
    connect(patientSer, &PatientSeri::end_success, this, &PatientNewDialog::sig_PatientSeriConfirmClicked);
    close();
    patientSer->exec();
}

void PatientNewDialog::onComboBoxIllnessKindSelect(int index)
{
    emit sig_customLog("新建分析-选择疾病种类显示不同的图片，索引：" + QString::number(index));
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
        editDiseases = new QLineEdit; //必须每次都初始化一个，否则会崩溃，猜测是因为QComboBox在切换项目的时候，会将其释放掉
        editDiseases->setStyleSheet("background-color: #22305B");
        ui->comboBox->setLineEdit(editDiseases);
		editDiseases->clear();
        editDiseases->setPlaceholderText(QStringLiteral("请输入疾病种类"));
        ui->label_type->clear();
    }
}

void PatientNewDialog::onBtnAddIconClicked()
{
  emit sig_customLog("新建分析-添加备注信息 ");
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