#include "qGLPatientMangeView.h"
#include "qGLPatientnewdialog.h"
#include "qGLPatientseri.h"
#include "qGLDb.h"
#include "qGLDicom.h"
#include "qGLTool.h"
#include <qGLlog.h>
#include "qSlicerPatientManagerModuleWidget.h"
#include "qPatientAnalyzeItem.h"
#include "DicomConflictEliminate.h"
// Qt
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QDir>
#include <qdebug>
#include <qfiledialog.h>
#include <QDate>
#include <QContextMenuEvent>
#include <qbitarray.h>
#include <QMenu>
#include <qsettings.h>
#include <qtextcodec.h>
#include <QDesktopServices>
#include "qGLMessageDialog.h"
#include "qGLCustomProgressBar.h"
#include <qdesktopwidget.h>

#include <Windows.h>
#include <winioctl.h>

//ITK
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <string>

//dcmtk
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>

#include "qSlicerIOManager.h"
#include "qJMainWindow.h"
patientMangeView::patientMangeView(Ui_patientMange& m_Controls, qSlicerPatientManagerModuleWidget* patientManagerMoudleWidget)
    : QObject(nullptr), m_Controls(m_Controls), patientManagerMoudleWidget(patientManagerMoudleWidget), parentWidget(patientManagerMoudleWidget)
{
    //Log* instance = Log::instance();
    //QObject::connect(this, &patientMangeView::sig_infoLog, instance, &Log::appendInfoLog);
    CreateQtPartControl(patientManagerMoudleWidget);
    
}
patientMangeView::~patientMangeView() {}


void patientMangeView::CreateQtPartControl(QWidget *parent)
{
    emit sig_infoLog("患者管理界面初始化");
    parentWidget = parent;
    parent->installEventFilter(this); //QObject对象监听数据
    m_Controls.tableWidget->setRowCount(0);
    m_Controls.setupUi(parentWidget);
    m_Controls.lineEdit_search->setPlaceholderText(QStringLiteral("请输入搜索内容"));
    m_Controls.pushButton0->hide(); //最近分析的5个按钮先隐藏
    m_Controls.pushButton1->hide();
    m_Controls.pushButton2->hide();
    m_Controls.pushButton3->hide();
    m_Controls.pushButton4->hide();

    widgetAll = new QWidget;
    m_Controls.scrollArea_3->setWidget(widgetAll);
    widgetPicture = new QWidget;
    m_Controls.scrollArea_2->setWidget(widgetPicture);
    widgetReport = new QWidget;
    m_Controls.scrollArea->setWidget(widgetReport);

    layoutAll = new QHBoxLayout;
    layoutPicture = new QHBoxLayout;
    layoutReport = new QHBoxLayout;
    widgetPicture->setLayout(layoutPicture);
    widgetReport->setLayout(layoutReport);
    widgetAll->setLayout(layoutAll);

    spacerItemAll = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacerItemPicture = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacerItemReport = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    table = new PatientTable(m_Controls.tableWidget);

    connect(m_Controls.btnLoad, &QPushButton::clicked, this, &patientMangeView::slot_btnLoadClick); //加载数据（一般是dicom序列）
    connect(m_Controls.btnLoadMITK, &QPushButton::clicked, this, &patientMangeView::OnLoadMitkClick);
    connect(m_Controls.btnSearch, &QPushButton::clicked, this, &patientMangeView::slot_btnSearch); //查询加载到表格的数据
	connect(m_Controls.comboBox, &QComboBox::currentTextChanged, this, [=]() { this->m_Controls.lineEdit_search->clear(); });

    btnVec.append(m_Controls.pushButton0);
    btnVec.append(m_Controls.pushButton1);
    btnVec.append(m_Controls.pushButton2);
    btnVec.append(m_Controls.pushButton3);
    btnVec.append(m_Controls.pushButton4);

    for (int i = 0; i < 5; i++) {
        setButtonDoubleClicked(btnVec.at(i), i);
    }
  
    //设置表格属性和信号和槽
    m_Controls.tableWidget->horizontalHeader()->setFixedHeight(64); //固定高度
    m_Controls.tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//设置可以Ctrl或者shitf选择，用于表格合并
	m_Controls.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不触发编辑选项
    m_Controls.tableWidget->horizontalHeader()->setSectionResizeMode(Report_, QHeaderView::ResizeToContents);
    m_Controls.tableWidget->horizontalHeader()->setStretchLastSection(true);

    connect(m_Controls.tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onCellDoubleClicked(int))); //
    connect(m_Controls.tableWidget, &QTableWidget::itemClicked, this, &patientMangeView::onItemClicked);
    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    //表格合并和分离右键菜单
    connect(m_Controls.tableWidget, &QTableWidget::customContextMenuRequested, this, &patientMangeView::slot_tableWidgetCustomContextMenuRequested);
    m_Controls.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //表格行合并右键菜单
    pop_menu = new QMenu(m_Controls.tableWidget);
    actionMergeRow = new QAction(QStringLiteral("合并"), pop_menu);
    pop_menu->addAction(actionMergeRow);
    connect(actionMergeRow, &QAction::triggered, this, &patientMangeView::slot_actionMergeRowTriggered);
    actionSeparate = new QAction(QStringLiteral("分离"), pop_menu);
    pop_menu->addAction(actionSeparate);
    connect(actionSeparate, &QAction::triggered, this, &patientMangeView::onActionSeparateRowTriggered);
   
    //排序
    connect(m_Controls.tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &patientMangeView::onSort);
    //给标题每一section加上排序角色部分
    for (int i = 0; i < m_Controls.tableWidget->horizontalHeader()->count(); i++)
    {
        m_Controls.tableWidget->horizontalHeaderItem(i)->setData(userRoleSort, -1); //-1表示没有排序
    }  
    m_Controls.tableWidget->horizontalHeader()->setSortIndicatorShown(true);//显示排序标志
  
    //loadPatientsToTableFromDb(); //从数据库加载表格
    //loadRecentlyAnalysisFromDb(); //加载最近新建分析（做多5个）
}

void patientMangeView::finished_success(QStringList list, PatientsAnalyse pa)
{
    emit sig_infoLog("完成新建分析和序列选择之后，进入图像筛选 ");
    //loadPatientsToTableFromDb(); //从数据库加载表格
    //loadRecentlyAnalysisFromDb();
    //需要传递信息给患者筛选界面
    //建议，直接将图片数据存入DataNode，然后存入DataStorage
    QString newAnalysisImagePath = list.join(';');
    patientManagerMoudleWidget->AddSignalNode("newAnalysisFilePath", newAnalysisImagePath.toStdString().c_str());   
    patientManagerMoudleWidget->AddSignalNode("patientAnalyseId", QString::number(pa.patientAnalyseId).toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("newAnalysisFilePath", newAnalysisImagePath.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientAnalyseId", QString::number(pa.patientAnalyseId).toStdString().c_str());
    if (pa.is_package) {
        patientManagerMoudleWidget->SetGlobalSaveValue("init_analyse", "2");
    }
    else {
        patientManagerMoudleWidget->SetGlobalSaveValue("init_analyse", "1");
    }
    
    patientManagerMoudleWidget->SetGlobalSaveValue("patientName", pa.patientName.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientBirthday", pa.birthday.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("analyseName", pa.analyseName.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientAge", pa.age.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientSex", pa.sex.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("doctorRemark", pa.doctorRemark.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("illnessKind", pa.illnessKind.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("remark", pa.remark.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("weight", pa.weight.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("height", pa.height.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("createTime", pa.createTime.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("changedTime", pa.changedTime.toStdString().c_str());

    GLTool::patientAnalyseId = pa.patientAnalyseId;
    GLTool::patientAnalyName = pa.analyseName;
    GLTool::patientName = pa.patientName;
    GLTool::patientBirthday = pa.birthday;

    GLTool::patientAge = pa.age;
    GLTool::patientSex = pa.sex;
    
    emit sig_nextPage();
    
}

void patientMangeView::load_package_file(int pid)
{
    patientManagerMoudleWidget->SetGlobalSaveValue("patientAnalyseId", QString::number(pid).toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("init_analyse", "2");
    emit sig_nextPage();
}

//双击患者管理界面行，打开新建分析页
void patientMangeView::onCellDoubleClicked(int row)
{
	emit sig_customLog("进入新建分析界面，患者所在的行：" + QString::number(row));
    QVector<Patient> patientVec = table->getPatients(row); //一行如果是合并的话，可能包含几个患者，因此包括多个患者的序列
    bool acceptOrCancel = true; //局部变量需要赋值，否则可能出现问题，比如debug和release版本不一致
    PatientsAnalyse pa = getNewAnalysis(patientVec, &acceptOrCancel);
    if (acceptOrCancel)
    {
        PatientNewDialog* pnd = new PatientNewDialog(patientVec, pa); //新建分析
        pnd->setWindowModality(Qt::ApplicationModal);
        connect(pnd, &PatientNewDialog::sig_PatientSeriConfirmClicked, this, &patientMangeView::finished_success);
        pnd->exec();
    }
}
    
void patientMangeView::slot_btnSearch() //使用QList<QTableWidgetItem *> QTableWidget::findItems(const QString &text, Qt::MatchFlags flags) const比较简单
{
    emit sig_customLog("患者搜索");
    QString searchStr = m_Controls.lineEdit_search->text().trimmed(); //获取搜索字符串

    if (searchStr.isEmpty()) //如果搜索内容为空，则加载所有行
    {
        loadPatientsToTableFromDb();
        return;
    }

    QVector<int> rowVec;
    QString searchType = m_Controls.comboBox->currentText().trimmed(); //获取搜索类型
    //遍历整个表格，搜索符合规定的患者姓名
    for (size_t row = 0; row < m_Controls.tableWidget->rowCount(); row++)
    {
        QStringList search;
        if (searchType == QStringLiteral("患者姓名"))
            search.append(m_Controls.tableWidget->item(row, PatientName)->text());
        else if (searchType.compare(QStringLiteral("患者ID")) == 0)
            search.append(m_Controls.tableWidget->item(row, DicomPatientId)->text());
        else if (searchType == QStringLiteral("模糊搜索"))
        {
            search.append(m_Controls.tableWidget->item(row, PatientName)->text());
            search.append(m_Controls.tableWidget->item(row, DicomPatientId)->text());
            search.append(m_Controls.tableWidget->item(row, PatientBirthday)->text());
            search.append(m_Controls.tableWidget->item(row, PatientAge)->text());
            search.append(m_Controls.tableWidget->item(row, PatientSex)->text());
            search.append(m_Controls.tableWidget->item(row, RecordModificationTime)->text());
            search.append(m_Controls.tableWidget->item(row, AccessionNumber)->text());
            search.append(m_Controls.tableWidget->item(row, Modalities)->text());
            search.append(m_Controls.tableWidget->item(row, SurgerSummary)->text());
        }

        //将符合要求的行加入到容器中
        if (search.filter(searchStr).size() > 0)
        {
            rowVec.append(row);
        }
    }

    //显示新的结果
    table->search(rowVec);
}

void patientMangeView::highlight(int row)
{
    emit sig_infoLog("当选中患者表中某列时，添加背景图片，行数：" + QString::number(row));
    QLabel *L1 = new QLabel();

    L1->setPixmap(QPixmap(":/Icons/linebg.png"));
    m_Controls.tableWidget->setCellWidget(row, 0, L1);
}

void patientMangeView::labelclear()
{
    emit sig_infoLog("清空图片 ");
    for (int i = 0; i < labelAllVec.size(); i++)
    {
        delete labelAllVec[i];
    }

    for (int i = 0; i < labelPictureVec.size(); i++)
    {
        delete labelPictureVec[i];
    }

    for (int i = 0; i < labelAnalyseReportVec.size(); i++)
    {
        delete labelAnalyseReportVec[i];
    }

    labelAllVec.clear();
    labelPictureVec.clear();
    labelAnalyseReportVec.clear();

    layoutAll->removeItem(spacerItemAll);
    layoutPicture->removeItem(spacerItemPicture);
    layoutReport->removeItem(spacerItemReport);

    //清空对应的计时器次数（不包括常显示的最近分析）
    clickTimerVec.resize(5);
    //清空对应的计时器（不包括常显示的最近分析）
    for (int i = 5; i < timerVec.size(); i++)
    {
        timerVec.at(i)->deleteLater();
    }
    timerVec.resize(5);
}

void patientMangeView::showScreetshotAndReport(int row)
{
    emit sig_infoLog("显示患者相关的缩略图和报告等，所在行: " + QString::number(row));
    labelclear(); //清空之前的图片

    m_Controls.tabWidget->setCurrentIndex(0); //默认显示全部tab页

    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("获取患者数据"), QStringLiteral("数据库连接失败！"));
        return;
    }

    //考虑到合并行，可能一行里可能有多个patientId，放入循环
    QStringList patientIdList = table->getPatientId(row);
    QStringList imagesPathList;
    QStringList modalityList;
    QStringList sliceCountList;
    QStringList pixelList; //部位
    QStringList describeList;
    QStringList datetimeList;
    for (int i = 0; i < patientIdList.size(); i++) //获取图片上显示的数据
    {
        Patient patient;
        patient.patientId = patientIdList.at(i).toInt();
        QVector<PatientsSeries> patientsSeriesVec = db.getPatientsSeries(patient);

        for (size_t i = 0; i < patientsSeriesVec.length(); i++)
        {
            imagesPathList.append(patientsSeriesVec.at(i).screenshot);

            QString seriesDescribe = patientsSeriesVec.at(i).seriesDescribe;
            describeList.append(seriesDescribe); //序列描述

            //模态和切片数量
            modalityList.append(patientsSeriesVec.at(i).modality);
            sliceCountList.append(QString::number(patientsSeriesVec.at(i).sliceCount));

            //层厚
            QString pixelSpacing = patientsSeriesVec.at(i).pixelSpacing;
            QString thickness = patientsSeriesVec.at(i).thickness;
            int index = thickness.lastIndexOf(QChar('.'));
            if (index != -1)
                thickness = thickness.left(index + 3);
            QStringList list = pixelSpacing.split("*");
            if (list.size() == 2) //保留小数点2位
            {
                QString space0 = list.at(0);
                QString space1 = list.at(1);

                int index0 = space0.lastIndexOf(QChar('.'));
                if (index0 != -1)
                    space0 = space0.left(index0 + 3);
                int index1 = space1.lastIndexOf(QChar('.'));
                if (index1 != -1)
                    space1 = space1.left(index1 + 3);

                pixelList.append(space0 + "*" + space1 + "*" + thickness);
            }
            else
                pixelList.append(pixelSpacing + "*" + thickness);

            //扫描时间
            datetimeList.append(patientsSeriesVec.at(i).scanTime.left(10));
        }
    }

    QVector<RowData> rdVec = table->getRowData(row);

    //显示图像序列
    for (int i = 0; i < imagesPathList.size(); i++)
    {
        auto path1 = imagesPathList.at(i);
        QFileInfo qfinfo(path1);
        if (qfinfo.isFile() || qfinfo.isDir()) {
        
        }
        else {
            continue;
        }
        QLabel* widget = new QLabel();
        widget->setFixedSize(336, 175);
        layoutPicture->addWidget(widget);
        labelPictureVec.append(widget);

        QLabel* image = new QLabel(widget);
        QPixmap Pix = QPixmap(imagesPathList.at(i));
        QSize PixSize = image->size();
        Pix.scaled(PixSize, Qt::KeepAspectRatioByExpanding);
        image->setPixmap(Pix);
        image->setScaledContents(true);
        int width = widget->width();
        int height = widget->height();
        image->setGeometry(0, 0, width / 2, height);

        QString txt = QString(
            "<font color=\"#FFFFFF\" size=\"18px\">%1</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%2(%3)</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%4</font><br/>"
            "<font color=\"#7886B0\" size=\"18px\">%5</font>")
            .arg(describeList.at(i)).arg(modalityList.at(i)).arg(sliceCountList.at(i)).arg(pixelList.at(i)).arg(datetimeList.at(i));
        QLabel* illustrate = new QLabel(widget);
        illustrate->setTextFormat(Qt::RichText);
        illustrate->setText(txt);
        illustrate->setGeometry(width / 2, 0, width / 2, height);
    }
    layoutPicture->addItem(spacerItemPicture);

    //显示分析/报告
    int n2 = clickTimerVec.size();
    QVector<int> patientAnalyseIdVec;
    for (int i = 0; i < rdVec.size(); i++)
    {
        QVector<PatientsAnalyse> paVec = rdVec.at(i).patientAnalyseVec; //获取该行（可能是合并行的）所有患者分析

        for (int j = 0; j < paVec.size(); j++)
        {
            if (!patientAnalyseIdVec.contains(paVec.at(j).patientAnalyseId)) //去掉重复的部分
            {
                PatientAnalyseItem* item = new PatientAnalyseItem();
                connect(item, &PatientAnalyseItem::OpenAnalyByIndex, this, &patientMangeView::onOpenAnalySe);
                item->setFixedSize(185, 194);
                item->SetInfo(paVec.at(j));
                layoutReport->addWidget(item);
                labelAnalyseReportVec.append(item);

                patientAnalyseIdVec.append(paVec.at(j).patientAnalyseId);
            }
        }
    }
    layoutReport->addItem(spacerItemReport);

    //显示全部文件
    //先显示图像序列
    for (int i = 0; i < imagesPathList.size(); i++)
    {
        auto path1 = imagesPathList.at(i);
        QFileInfo qfinfo(path1);
        if (qfinfo.isFile() || qfinfo.isDir()) {

        }
        else {
            continue;
        }
        QLabel* widget = new QLabel();
        widget->setFixedSize(336, 175);
        labelAllVec.append(widget);
        layoutAll->addWidget(widget);

        QLabel* image = new QLabel(widget);
        QPixmap Pix = QPixmap(imagesPathList.at(i));
        QSize PixSize = image->size();
        Pix.scaled(PixSize, Qt::KeepAspectRatioByExpanding);
        image->setPixmap(Pix);
        image->setScaledContents(true);
        int width = widget->width();
        int height = widget->height();
        image->setGeometry(0, 0, width / 2, height);

        QString txt = QString(
            "<font color=\"#FFFFFF\" size=\"18px\">%1</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%2(%3)</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%4</font><br/>"
            "<font color=\"#7886B0\" size=\"18px\">%5</font>")
            .arg(describeList.at(i)).arg(modalityList.at(i)).arg(sliceCountList.at(i)).arg(pixelList.at(i)).arg(datetimeList.at(i));
        QLabel* illustrate = new QLabel(widget);
        illustrate->setTextFormat(Qt::RichText);
        illustrate->setText(txt);
        illustrate->setGeometry(width / 2, 0, width / 2, height);
    }

    //后显示分析/报告，注意Qt的QObject之间没有拷贝和复制，因此控件必须自己创建
    int n1 = clickTimerVec.size();
    QVector<int> patientAnalyseIdVec1;
    for (size_t i = 0; i < rdVec.size(); i++)
    {
        QVector<PatientsAnalyse> paVec = rdVec.at(i).patientAnalyseVec;
        for (int j = 0; j < paVec.size(); j++)
        {
            if (!patientAnalyseIdVec1.contains(paVec.at(j).patientAnalyseId)) //去掉重复的部分
            {
                PatientAnalyseItem* item = new PatientAnalyseItem();
                connect(item, &PatientAnalyseItem::OpenAnalyByIndex, this, &patientMangeView::onOpenAnalySe);
                item->setFixedSize(185, 194);
                item->SetInfo(paVec.at(j));
                layoutAll->addWidget(item);
                labelAllVec.append(item);

                patientAnalyseIdVec1.append(paVec.at(j).patientAnalyseId);
            }
        }
    }

    layoutAll->addItem(spacerItemAll);
}

//从Patients中加载所有数据
void patientMangeView::loadPatientsToTableFromDb()
{
    emit sig_infoLog("从Patients表中加载数据");
    //断开手术概要单元格编辑的信号和槽的链接，因为后面加载数据，插入行的过程中，也会触发编辑的操作
    //暂时断开itemChanged信号连接
    disconnect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    //从数据库获取数据
    GLDb db(this);
    if (!db.createDbConnect())
    {
        GLMessageBox msg;
        msg.queRenError(QStringLiteral("患者管理"), QStringLiteral("数据库连接失败！无法从数据库加载数据到患者管理界面。"));
        return;
    }

    table->clearTable(); //清空数据
    table->patientVec = db.getPatients(GLDb::glAccount); //获取当前用户所拥有的所有患者

    for (int i = 0; i < table->patientVec.length(); i++)
    {
        RowData rd; //代表一行数据

        rd.patientId = QString::number(table->patientVec[i].patientId);
        Patient patient;
        patient.patientId = rd.patientId.toInt();
        QVector<int> patientAnalyseIdVec = db.getPatientAnalyseIdFromPatientAnalyseIdPatientIdId(patient);
        QVector<PatientsAnalyse> patientAnalyseVec;
        for (int i = 0; i < patientAnalyseIdVec.size(); i++)
        {
            PatientsAnalyse pa = db.getPatientsAnalyse1(patientAnalyseIdVec.at(i));
            if (pa.patientAnalyseId != -1) {
                QString curDirPath = QCoreApplication::applicationDirPath();
                QString backupPath = QStringLiteral("%1/%2").arg(curDirPath).arg("bak");
                auto fileName = QStringLiteral("%1/%2.mrb").arg(backupPath).arg(pa.patientAnalyseId);
                QFileInfo qfile(fileName);
                if (!qfile.isFile()) {
                    continue;
                }
                patientAnalyseVec.append(pa);
            }
        }
        rd.patientAnalyseVec = patientAnalyseVec; //是否一次性获取还是延迟获取
        for (size_t i = 0; i < patientAnalyseVec.size(); i++)
        {
            if (!patientAnalyseVec.at(i).reportPath.isEmpty())
                rd.report.append(patientAnalyseVec.at(i).reportPath); //获取报告
        }

        rd.patientName = table->patientVec[i].patientName;
        rd.dicomPatientId = table->patientVec[i].dicomPatientId;
        rd.birthday = table->patientVec[i].birthday;
        rd.age = QString::number(GLTool::calAge(rd.birthday));
        rd.sex = table->patientVec[i].sex;
        rd.height = table->patientVec[i].height;
        rd.weight = table->patientVec[i].weight;
        rd.modifyDateTime = table->patientVec[i].recordModificationTime;

        //获取序列的modality和accessionNumber
        QVector<PatientsSeries> patientsSeriesVec = db.getPatientsSeries(table->patientVec[i]);

        QStringList accessionNumberList;
        QStringList modalityList;
        for (int i = 0; i < patientsSeriesVec.size(); i++)
        {
            accessionNumberList.append(patientsSeriesVec[i].accessionNumber);
            modalityList.append(patientsSeriesVec[i].modality);
        }

        accessionNumberList.removeDuplicates();
        rd.accessionNumber = accessionNumberList.join(",");

        modalityList.removeDuplicates();
        rd.modalities = modalityList.join(",");

        rd.surgrySummary = table->patientVec[i].surgerSummary;
        rd.mergeSign = table->patientVec.at(i).mergeSign;

        //按照数据加入到table的原始仓库，原始仓库和数据库的每行一一对应
        int index = table->addRowDataToRootVec(rd);
        //按照合并标志为空，则先插入，不为空，则插入hash过渡（当然也可以统一插入到hash，然后统一处理）
        table->setMergeSignIndexHash(rd.mergeSign, index);
    }

    //更新hash到rootIndexList
    table->mergeSignIndexHashToRootIndexList();

    //重置表格
    table->updateTableWidget();

    resetColumnWidth();

    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);
}

//取出最近5个新建分析
void patientMangeView::loadRecentlyAnalysisFromDb()
{
    emit sig_infoLog("初始化最近分析");
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("获取最近分析"), QStringLiteral("数据库连接失败！无法加载最近分析"));
        return;
    }

    QVector<PatientsAnalyse> patientsAnalyseVec = db.getPatientsAnalyse(5, 2);
    for (int i = 0; i < patientsAnalyseVec.size(); i++)
    {
        QVector<Patient> patientVec1 = db.getPatientIdFromPatientAnalyseIdPatientIdId(patientsAnalyseVec.at(i));
        QVector<Patient> patientVec;
        for (int i = 0; i < patientVec1.size(); i++)
        {
            Patient patient = db.getPatients(patientVec1.at(i).patientId);
            patientVec.append(patient);
        }

        //此方法在mitk版本可以正确找到pushButton，但是slicer版本不可以
        //QPushButton* parent = parentWidget->findChild<QPushButton*>(QString("pushButton%1").arg(i));
        //下面的方法可以
        //QPushButton* parent = m_Controls.groupBox->findChild<QPushButton*>(QString("pushButton%1").arg(i));
        //但为了更可靠，直接使用m_Controls.pushButtonX
        if (btnVec.at(i))
        {
            //创建最近分析窗体
            createRecentlyAnalysisWidget(btnVec.at(i), patientsAnalyseVec.at(i).illnessKind, patientsAnalyseVec.at(i).analyseName,
                patientsAnalyseVec.at(i).patientName, patientsAnalyseVec.at(i).sex, patientsAnalyseVec.at(i).birthday, patientsAnalyseVec.at(i).doctorRemark,
                patientsAnalyseVec.at(i).changedTime);
            if (recentlyAnalysisHash.contains(btnVec.at(i)))
                recentlyAnalysisHash.remove(btnVec.at(i));
            recentlyAnalysisHash.insert(btnVec.at(i), patientsAnalyseVec.at(i));
        }
    }
}

void patientMangeView::createRecentlyAnalysisWidget(QWidget * parent, QString diseaseName, 
    QString analysisName, QString patientName, QString sex, QString birthday, QString remark, QString createTime)
{
    //疾病名称
    QLabel* labelDiseaseName = parent->findChild<QLabel*>("labelDiseaseName");
    if (!labelDiseaseName)
    {
        labelDiseaseName = new QLabel();
        labelDiseaseName->setObjectName("labelDiseaseName");
    }
    labelDiseaseName->setText(diseaseName);
    labelDiseaseName->setStyleSheet("background-color: #3377FF; border-radius: 8px");

    //分析名称
    QLabel* labelAnalysisName = parent->findChild<QLabel*>("labelAnalysisName");
    if (!labelAnalysisName)
    {
        labelAnalysisName = new QLabel();
        labelAnalysisName->setObjectName("labelAnalysisName");
    }
    labelAnalysisName->setText(analysisName);

    //患者名称
    QLabel* labelPatientName = parent->findChild<QLabel*>("labelPatientName");
    if (!labelPatientName)
    {
        labelPatientName = new QLabel();
        labelPatientName->setObjectName("labelPatientName");
    }
    labelPatientName->setText(QStringLiteral("姓名: ") + patientName.split(":")[0].trimmed());

    //性别
    QLabel* labelSex = parent->findChild<QLabel*>("labelSex");
    if (!labelSex)
    {
        labelSex = new QLabel();
        labelSex->setObjectName("labelSex");
    }
    labelSex->setText(QStringLiteral("  性别: ") + sex);

    //生日
    QLabel* labelBirthday = parent->findChild<QLabel*>("labelBirthday");
    if (!labelBirthday)
    {
        labelBirthday = new QLabel();
        labelBirthday->setObjectName("labelBirthday");
    }
    labelBirthday->setText(QStringLiteral("生日: ") + birthday);

    //备注
    QLabel* labelRemark = parent->findChild<QLabel*>("labelRemark");
    if (!labelRemark)
    {
        labelRemark = new QLabel();
        labelRemark->setObjectName("labelRemark");
    }
    labelRemark->setText(QStringLiteral("备注: ") + remark);
    labelRemark->hide(); //备注太多了，暂时隐藏看效果

    //创建时间
    QLabel* labelCreateTime = parent->findChild<QLabel*>("labelCreateTime");
    if (!labelCreateTime)
    {
        labelCreateTime = new QLabel();
        labelCreateTime->setObjectName("labelCreateTime");
    }
    labelCreateTime->setText(createTime);
    labelCreateTime->setStyleSheet("color: #7886B0");

    QHBoxLayout *layoutTop = new QHBoxLayout;
    layoutTop->addWidget(labelDiseaseName);
    layoutTop->addWidget(labelAnalysisName, 10);
    layoutTop->addStretch();

    QGridLayout *layoutMid = new QGridLayout;
    layoutMid->addWidget(labelPatientName, 0, 0);
    layoutMid->addWidget(labelSex, 0, 1);
    layoutMid->addWidget(labelBirthday, 1, 0);
    layoutMid->addWidget(labelRemark, 1, 1);

    QHBoxLayout *layoutBottom = new QHBoxLayout;
    layoutBottom->addWidget(labelCreateTime);
    layoutBottom->addStretch();

    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addLayout(layoutTop);
    layoutMain->addLayout(layoutMid);
    layoutMain->addLayout(layoutBottom);

    parent->setLayout(layoutMain);
    parent->show();
    parent->setStyleSheet("background-color: #22305B; border: none;");
}

bool patientMangeView::eventFilter(QObject* obj, QEvent* event)
{
	//if (event->type() == QEvent::ContextMenu)
	//{
	//	QContextMenuEvent* event1 = static_cast<QContextMenuEvent*>(event); //上下文菜单事件
	//	if (event1)
	//	{
	//		emit sig_infoLog("右键菜单，合并行");
	//		Qt::ContextMenuPolicy policy = m_Controls.tableWidget->contextMenuPolicy();
	//		QPoint point = QCursor::pos();
	//		pop_menu->popup(point);
	//		//pop_menu->exec(point);
	//	   // event1->accept();
	//		return true;
	//	}
	//}
    if (event->type() == QEvent::Show)
    {
        if (obj == parentWidget)
        {
            loadPatientsToTableFromDb(); //从数据库加载表格
            loadRecentlyAnalysisFromDb(); //加载最近新建分析（最多5个）
        }
    }
	return QObject::eventFilter(obj, event);
}

//合并
void patientMangeView::slot_actionMergeRowTriggered()
{  
    QVector<int> rowVec = getSelectedRow(); //存储所有的行数
    if (rowVec.size() < 2)
        return;

    //暂时断开itemChanged信号连接
    disconnect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    //合并到最上面的行，其他行删除或隐藏
    qSort(rowVec); //排序以便找到最上面的行
    QStringList patientIdList = table->merge(rowVec);
    table->updateTableWidget();

    //存入数据库
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("患者管理"), QStringLiteral("数据库连接失败，将不能将合并信息存入数据库！"));
    }
    else
    {
        db.setPatientMergeSplitSign(patientIdList, patientIdList.join(","));
    }

    //恢复信号的连接
    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);
}

//分离行
void patientMangeView::onActionSeparateRowTriggered()
{
    //暂时断开itemChanged信号连接
    disconnect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    QVector<int> rowVec = getSelectedRow(); //存储所有的行数

    QStringList patientIdList = table->split(rowVec); //开始分离

    resetColumnWidth();

    //存入数据库
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("患者管理"), QStringLiteral("数据库连接失败，将不能将分离信息存入数据库！"));
    }
    else
    {
        db.setPatientMergeSplitSign(patientIdList, QString());
    }

    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);
}

void patientMangeView::slot_tableWidgetCustomContextMenuRequested(const QPoint & pos)
{
    pop_menu->popup(QCursor::pos());
}

void patientMangeView::onSort(int column)
{
    QMultiMap<QString, int> map;
    for (int row = 0; row < m_Controls.tableWidget->rowCount(); row++)
    {
        QString txt = m_Controls.tableWidget->item(row, column)->text();
        map.insert(txt, row);
    }

    QMapIterator<QString, int> it(map);
    QVector<int> rowVec;
    while (it.hasNext())
    {
        it.next();
        rowVec.append(it.value());
    }

    int value = m_Controls.tableWidget->horizontalHeaderItem(column)->data(userRoleSort).toInt();
    if (value == -1 || value == Qt::DescendingOrder)
    {
        m_Controls.tableWidget->horizontalHeader()->setSortIndicator(column, Qt::AscendingOrder);
        //m_Controls.tableWidget->sortByColumn(column);
        //从大到小排列，反转
        int a = 0, b = 0;
        int size = rowVec.size();
        for (int i = 0; i < size; i++)
        {
            if (size - i - 1 <= i)
                break;
            int a = rowVec.at(i);
            rowVec[i] = rowVec[size - i - 1];
            rowVec[size - i - 1] = a;
        }
        table->sort(rowVec);
        m_Controls.tableWidget->horizontalHeaderItem(column)->setData(userRoleSort, Qt::AscendingOrder);
    }

    if (value == Qt::AscendingOrder)
    {
        m_Controls.tableWidget->horizontalHeader()->setSortIndicator(column, Qt::DescendingOrder);
        //m_Controls.tableWidget->sortByColumn(column);
        table->sort(rowVec);
        m_Controls.tableWidget->horizontalHeaderItem(column)->setData(userRoleSort, Qt::DescendingOrder);
    }
}

void patientMangeView::OnLoadMitkClick()
{
    //QString fileName = "";
    //QString curPath = QDir::currentPath();
    //fileName = QFileDialog::getOpenFileName(nullptr, QStringLiteral("打开分析文件"), curPath, "mitk File(*.mitk)");
    //if (fileName == "") {
    //    GLMessageDialog::ShowInformation(QStringLiteral("导入分析"), QStringLiteral("导入失败, 请选择一个有效的分析文件！"));
    //    return;
    //}
    ////-99 means to open a mitk file without analyse
    //SetGlobalValue("analyseId", -99);
    //SetGlobalValue("NewAnalysisForFilter", true);
    //SetGlobalValue("NewAnalysisForModel", true);
    //SetGlobalValue("NewAnalysisForTools", true);
    //SetGlobalValue("newAnalysisFilePath", fileName.toStdString());

    //berry::SmartPointer<berry::IWorkbenchPage> pPage = berry::PlatformUI::GetWorkbench()->ShowPerspective(PICTURE_CHOOSE_PERSPECTIVE,
    //    berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow()); //图像筛选界面
}


void patientMangeView::resetColumnWidth()
{
    int total = m_Controls.tableWidget->width() - m_Controls.tableWidget->columnWidth(Report_);
    int columnWidth = total / SurgerSummary;
    for (int column = PatientName; column < m_Controls.tableWidget->columnCount() - 1; column++)
    {
        m_Controls.tableWidget->setColumnWidth(column, columnWidth);
    }
}

void patientMangeView::setButtonDoubleClicked(QPushButton* btn, int i)
{
    clickTimerVec.append(0);
    connect(btn, &QPushButton::clicked, this, [=]()
    {
        clickTimerVec[i]++; //点击一次计时
        if (clickTimerVec[i] >= 2)
        {
            onPushButtonXDoubleClicked(btn);
            btn->setEnabled(false);
        }
        if (timerVec[i]->isActive() == false)
            timerVec[i]->start(350);
    });
    QTimer* timer = new QTimer();
    timerVec.append(timer);
    connect(timer, &QTimer::timeout, this, [=]() {
        clickTimerVec[i] = 0;
        timer->stop();
        btn->setEnabled(true);
    });
}

PatientsAnalyse patientMangeView::getNewAnalysis(QVector<Patient> patientVec, bool* acceptOrCancel)
{
    QStringList nameList;
    QStringList birthList;
    QStringList sexList;
    QStringList ageList;
    QStringList heightList;
    QStringList weightList;
    for (int i = 0; i < patientVec.size(); i++)
    {
        nameList.append(patientVec.at(i).patientName); //GLCommonTool::GetDisplayPatientName(patientVec.at(i).patientName)
        birthList.append(patientVec.at(i).birthday);
        sexList.append(patientVec.at(i).sex);
        ageList.append(patientVec.at(i).age);
        heightList.append(patientVec.at(i).height);
        weightList.append(patientVec.at(i).weight);
    }
    nameList.removeDuplicates();
    birthList.removeDuplicates();
    sexList.removeDuplicates();
    ageList.removeDuplicates();
    heightList.removeDuplicates();
    weightList.removeDuplicates();

    PatientsAnalyse pa;
    if (nameList.size() > 1 || birthList.size() > 1 || sexList.size() > 1/* || heightList.size() > 1 || weightList.size() > 1*/)
    {
        Patient patient;
        DicomConfilctEliminate dce(patientVec, patient, QStringLiteral("合并之后，请手动选择"), acceptOrCancel);
        pa.patientName = patient.patientName;
        pa.birthday = patient.birthday;
        pa.sex = patient.sex;
        pa.age = QString::number(GLTool::calAge(pa.birthday));
        pa.height = patient.height;
        pa.weight = patient.weight;
    }
    else
    {
        pa.patientName = nameList.join(",");
        pa.birthday = birthList.join(",");
        pa.sex = sexList.join(",");
        pa.age = QString::number(GLTool::calAge(pa.birthday));
        pa.height = heightList.join(",");
        pa.weight = weightList.join(",");
    }

    return pa;
}


void patientMangeView::onOpenReport()
{
    QObject* obj = sender();
    if (obj)
    {
        QString report = obj->property(reportProperty).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(report));
    }
}

void patientMangeView::onPushButtonXDoubleClicked(QPushButton* btn)
{
    /*GLTool::progressBar = new qSlicerProgressBar;
    GLTool::progressBar->InitProgressBar(QStringLiteral("正在打开分析..."),0,100);
    GLTool::progressBar->show();*/
    qJMainWindow* mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
    auto im = qSlicerApplication::application()->ioManager();
    mw->ShowProgressBar(QStringLiteral("正在打开分析"));

    PatientsAnalyse pa = recentlyAnalysisHash.value(btn);

    //GLTool::progressBar->setProgressValue(4);
    onOpenAnalySe(pa);

    
    
}

QVector<int> patientMangeView::getSelectedRow()
{
    QList<QTableWidgetSelectionRange> ranges = m_Controls.tableWidget->selectedRanges(); //获取所有选择区

    QVector<int> rowVec; //存储所有的行数
    for (int i = 0; i < ranges.size(); i++)
    {
        QTableWidgetSelectionRange range = ranges.at(i);//获取每一个range
        for (int row = range.topRow(); row <= range.bottomRow(); row++)
        {
            rowVec.append(row);
        }
    }

    return rowVec;
}

QString patientMangeView::getRecentDir()
{
    QString curPath = qApp->applicationDirPath();
    QSettings set(curPath + "/configure/cfg.ini", QSettings::IniFormat);
    set.setIniCodec(QTextCodec::codecForName("GBK")); //防止中文乱码
    QString dir = set.value("PatientManager/importRecentDir").toString();
    if (dir.isEmpty())
        return QDir::currentPath();
    return dir;
}

void patientMangeView::saveRecentDir(QString dir)
{
    QString curPath = qApp->applicationDirPath();
    QSettings set(curPath + "/configure/cfg.ini", QSettings::IniFormat);
    set.setIniCodec(QTextCodec::codecForName("GBK")); //防止中文乱码
    set.setValue("PatientManager/importRecentDir", dir);
}

void patientMangeView::onOperationSummaryItemChanged(QTableWidgetItem * item)
{
  if (item->column() != SurgerSummary)
    return;

  QWidget *label = m_Controls.tableWidget->cellWidget(item->row(), Report_);
  if (!label)
      return;

  Patient patient;
  patient.patientId = table->rootVec.at(table->rootIndexList.at(item->row()).at(0)).patientId.toInt();
  QString operationSummary = item->text();
  patient.surgerSummary = operationSummary;

  //存入数据库
  GLDb db(this);
  if (!db.createDbConnect())
  {
    QMessageBox::warning(nullptr, QStringLiteral("数据库"), QStringLiteral("数据库连接失败！"));
    return;
  }
  db.setSurgerySummary(patient);
}

void patientMangeView::onOpenAnalySe(PatientsAnalyse analy)
{
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("获取患者数据"), QStringLiteral("数据库连接失败，无法获取患者图像序列！"));
        return;
    }
    auto im = qSlicerApplication::application()->ioManager();
    qJMainWindow* mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());

    mw->ShowProgressBar(QStringLiteral("正在打开分析"));
    //获取患者序列清单表-这是患者序列所有的数据
    QVector<PatientsAnalyseSeries> patientsSeriesVec = db.getPatientAnalyseSeries(analy);
    QStringList list;
    QVector<PatientsSeries> vec = db.getPatientsSeries(patientsSeriesVec);
    for (int i = 0; i < vec.size(); i++)
    {
        list.append(vec.at(i).seriesDir);
    }
    analy.is_package = true;
    finished_success(list, analy);
}

void patientMangeView::onItemClicked(QTableWidgetItem *item)
{
  if (item->column() == SurgerSummary)
    m_Controls.tableWidget->editItem(item);
  else
      showScreetshotAndReport(item->row());
}

void lubksb(double b[], double vv[], int indx[], itk::Matrix<double, 3, 3> matrix) {
    int n = 3, ii = 0;
    // y
    for (int i = 0; i < n; i++) {
        int ip = indx[i];
        double sum = b[ip];
        b[ip] = b[i];
        if (ii != 0)
            for (int j = ii - 1; j < i; j++) sum -= matrix[i][j] * b[j];
        else
            ii = i + 1;
        b[i] = sum;
    }
    // x
    for (int i = n - 1; i >= 0; i--) {
        double sum = b[i];
        for (int j = i + 1; j < n; j++) sum -= matrix[i][j] * b[j];
        b[i] = sum / matrix[i][i];
    }
}
void lucmp(itk::Matrix<double, 3, 3>& matrix, double vv[], int indx[]) {
    int n = 3, imax = 0;
    for (int i = 0; i < n; i++) {
        double big = 0.0;
        for (int j = 0; j < n; j++) {
            double temp = abs(matrix[i][j]);
            if (temp > big) big = temp;
        }
        vv[i] = 1.0 / big;
    }
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < j; i++) {
            double sum = matrix[i][j];
            for (int k = 0; k < i; k++) sum -= matrix[i][k] * matrix[k][j];
            matrix[i][j] = sum;
        }
        double big = 0.0;
        for (int i = j; i < n; i++) {
            double sum = matrix[i][j];
            for (int k = 0; k < j; k++) sum -= matrix[i][k] * matrix[k][j];
            matrix[i][j] = sum;
            double dum = vv[i] * abs(sum);
            if (dum >= big) {
                big = dum;
                imax = i;
            }
        }
        if (j != imax) {
            for (int i = 0; i < n; i++) {
                double mid = matrix[imax][i];
                matrix[imax][i] = matrix[j][i];
                matrix[j][i] = mid;
            }
            double mid = vv[j];
            vv[j] = vv[imax];
            vv[imax] = mid;
        }
        indx[j] = imax;
        if (j != n - 1) {
            double dum = 1.0 / matrix[j][j];
            for (int i = j + 1; i < n; i++) matrix[i][j] *= dum;
        }
    }
}
//Gets the inverse matrix of the matrix use lut
void GetInvertMatrix(itk::Matrix<double, 3, 3> matrix, itk::Matrix<double, 3, 3>& newMatrix)
{
    double vv[3] = { 0 };
    int indx[3] = { 0 };
    lucmp(matrix, vv, indx);
    double b[3] = { 0 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) b[j] = 0;
        b[i] = 1.0;
        lubksb(b, vv, indx, matrix);
        for (int j = 0; j < 3; j++) newMatrix[j][i] = b[j];
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="path"></param>
/// <returns></returns>

bool IsFixDisk(QString path)
{
    itk::Matrix<double, 3, 3> matrix; //3行3列矩阵
    itk::Matrix<double, 3, 3> invmatrix;
    matrix[0][0] = 1;
    matrix[0][1] = -4;
    matrix[0][2] = -3;
    matrix[1][0] = 1;
    matrix[1][1] = -5;
    matrix[1][2] = -3;
    matrix[2][0] = -1;
    matrix[2][1] = 6;
    matrix[2][2] = 4;

    invmatrix[0][0] = 1;
    invmatrix[0][1] = 0;
    invmatrix[0][2] = 0;
    invmatrix[1][0] = 0;
    invmatrix[1][1] = 1;
    invmatrix[1][2] = 0;
    invmatrix[2][0] = 0;
    invmatrix[2][1] = 0;
    invmatrix[2][2] = 1;
    GetInvertMatrix(matrix, invmatrix);


    QStringList list = path.split("/");
    UINT res = ::GetDriveType(list[0].toStdString().c_str()); //判断磁盘类型
    return res == DRIVE_FIXED;
}

bool IsUSB(QString path)
{
    QStringList list = path.split("/");
    QString test = QString("\\\\.\\%1").arg(list[0]);
    DWORD dwOutBytes;
    BOOL bResult;
    HANDLE hDevice = CreateFile(
        test.toStdString().c_str(), // 要打开的驱动器设备；格式必须为“\\.\C:”，否则打开失败，返回错误码2：系统找不到指定的文件。
        GENERIC_READ | GENERIC_WRITE,     // 访问权限
        FILE_SHARE_READ | FILE_SHARE_WRITE, //共享模式
        NULL,             // 使用默认的安全属性
        OPEN_EXISTING,    // 打开存在的设备
        NULL,
        NULL
    );
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    STORAGE_DESCRIPTOR_HEADER* pDevDescHeader;
    pDevDescHeader = (STORAGE_DESCRIPTOR_HEADER*)malloc(sizeof(STORAGE_DESCRIPTOR_HEADER));
    if (NULL == pDevDescHeader)
    {
        return FALSE;
    }
    STORAGE_PROPERTY_QUERY Query;
    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;
    bResult = DeviceIoControl(
        hDevice,     // 设备句柄
        IOCTL_STORAGE_QUERY_PROPERTY,     // 操作控制码
        &Query, sizeof Query,   //输入参数及缓冲区大小
        pDevDescHeader,     //输出参数
        sizeof(STORAGE_DESCRIPTOR_HEADER),  //输出参数的缓冲区大小
        &dwOutBytes,     //实际输出的大小
        NULL);
    if (!bResult)
    {
        free(pDevDescHeader);
        (void)CloseHandle(hDevice);
        return FALSE;
    }
    PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
    pDevDesc = (STORAGE_DEVICE_DESCRIPTOR*)malloc(pDevDescHeader->Size);
    bResult = DeviceIoControl(hDevice,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &Query, sizeof Query,
        pDevDesc, pDevDescHeader->Size,
        &dwOutBytes,
        NULL);
    free(pDevDescHeader);
    (void)CloseHandle(hDevice);
    if (pDevDesc->BusType == BusTypeUsb)//总线类型为USB，返回TRUE
    {
        free(pDevDesc);
        return TRUE;
    }
    free(pDevDesc);
    return FALSE;
}

QString GetExResPath(QString path, QString typeDes)
{
    QStringList list = path.split("/");
    int size = list.size();
    QString filePath = "";
    //"C:/Users/Administrator/AppData/Local"
    QString storageDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    //创建路径 标准位置下面的jlsp文件+文件类型+之前的部分路径
    if (size == 2)
        filePath = QStringLiteral("%1/jlsq/%2/%3").arg(storageDir).arg(typeDes).arg(list[size - 1]);
    else if (size > 2)
        filePath = QStringLiteral("%1/jlsq/%2/%3/%4").arg(storageDir).arg(typeDes).arg(list[size - 2]).arg(list[size - 1]);
    else if (typeDes == "")
        filePath = QStringLiteral("%1/jlsq/%2").arg(storageDir).arg(typeDes);
    else
        filePath = QStringLiteral("%1/jlsq/%2").arg(storageDir).arg(typeDes);

    return filePath;
}

bool patientMangeView::CopyDir(const QString& source, const QString& destination, bool override, GLCustomProgressBar *bar)
{
    QDir directory(source);
    if (!directory.exists())
    {
        return false;
    }

    QString srcPath = QDir::toNativeSeparators(source);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QString dstPath = QDir::toNativeSeparators(destination);
    if (!dstPath.endsWith(QDir::separator()))
        dstPath += QDir::separator();
    QDir tmp(dstPath);
    tmp.mkpath(dstPath);

    bool error = false;
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (QStringList::size_type i = 0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        QString srcFilePath = srcPath + fileName;
        QString dstFilePath = dstPath + fileName;
        QFileInfo fileInfo(srcFilePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if (override)
            {
                QFile::setPermissions(dstFilePath, QFile::WriteOwner);
            }
            QFile::copy(srcFilePath, dstFilePath);
        }
        else if (fileInfo.isDir())
        {
            QDir dstDir(dstFilePath);
            dstDir.mkpath(dstFilePath);
            if (!CopyDir(srcFilePath, dstFilePath, override, bar))
            {
                error = true;
            }
            //mitk::ProgressBar::GetInstance()->Progress(1);
            bar->setValue(i);
        }
    }
    return !error;
}

void patientMangeView::slot_btnLoadClick()
{
    emit sig_customLog("导入患者信息（DICOM文件）");
    //QString path = QFileDialog::getExistingDirectory(parentWidget, QStringLiteral("选择一个DICOM序列目录"), getRecentDir()); //从资源管理器中获取DICOM文件序列
   QString path;
   QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
    bool JTest = set.value("General/qfiledialog_fixed").toString() == "2";
    if (JTest) {
        path = QStringLiteral("E:/CYH");
    }
    else 
    {
        path = QFileDialog::getExistingDirectory(parentWidget, QStringLiteral("选择一个DICOM序列目录"), getRecentDir()); //从资源管理器中获取DICOM文件序列
    }

    if (path.isEmpty())
        return;

    bool isFixDisk = IsFixDisk(path); //判断是否是固定磁盘
    bool isUSB = IsUSB(path); //判读是否为U盘
    if (!isFixDisk || isUSB) { //不是固定磁盘
        QString exPath = GetExResPath(path, "dcmData"); //找到合适路径
        QDir dir(exPath);
        if (dir.exists()) //路径已经存在，则使用原来的数据
        {
            qGLMessageDialog::ShowInformation(QStringLiteral("导入序列"), QStringLiteral("发现使用可移动介质, 将使用本地同名数据"));
        }
        else {
            QStringList tmpList = { QStringLiteral("拷贝"), QStringLiteral("不拷贝") };
            MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("导入序列"), QStringLiteral("发现使用可移动介质, 将拷贝数据到本地"), MessageButtonType::BUTTON_OK_AND_CANCEL, tmpList);
            if (res == MessageResult::OK) {
                //拷贝数据
                QDir oldDir(path);
                auto list = oldDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
                dir.mkdir(exPath);
                //AddValueSignal("show_progress_bar", QStringLiteral("数据拷贝中...").toStdString().c_str());
                GLCustomProgressBar progressBar;
                QDesktopWidget* desk = QApplication::desktop();
                progressBar.setGeometry(desk->availableGeometry()); //必须加这个，否则不显示

                progressBar.setText(QStringLiteral("数据拷贝中..."));
                
                progressBar.setWindowFlags(progressBar.windowFlags() | Qt::WindowStaysOnTopHint);
                progressBar.showNormal();
                progressBar.showFullScreen();
                /*QTimer::singleShot(200, this, [=]() */
                {
                    //mitk::ProgressBar::GetInstance()->AddStepsToDo(list.size());
                    progressBar.setMaximum(list.size());
                    CopyDir(path, exPath, true, &progressBar);
                    //mitk::ProgressBar::GetInstance()->Progress(list.size());
                    progressBar.setValue(list.size());
                    //AddSignal("hide_progress_bar");
                    progressBar.close();
                    Dicom dicom;
                    dicom.itkReadDicomSeriesAnyDir(exPath);

                    loadPatientsToTableFromDb(); //目前采用导入一次dicom，就从数据库里全部加载，这虽然可以保证一致性，但可能会影响性能

                    saveRecentDir(exPath);
                };
                return;
            }
        }
    }

    Dicom dicom;
    dicom.itkReadDicomSeriesAnyDir(path);

    loadPatientsToTableFromDb(); //目前采用导入一次dicom，就从数据库里全部加载，这虽然可以保证一致性，但似乎有待改正

    saveRecentDir(path);
}
