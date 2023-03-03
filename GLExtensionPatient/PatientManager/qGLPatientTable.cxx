#include "qGLPatientTable.h"
#include <qlabel.h>
#include "qGLTool.h"

PatientTable::PatientTable(QTableWidget * tableWidget)
    :tableWidget(tableWidget)
{
    reportImage = QString(":/Icons/report_small.png"); //显示存在报告
}

int PatientTable::addRowDataToRootVec(RowData rowData)
{
    rootVec.append(rowData);
    rootVec.append(rowData);
    return rootVec.size() - 1; //返回最后一项的索引
}

//将原始数据对应的行数加入到tableIndex数据中
void PatientTable::addIndexToTable(int row, QVector<int> rowVec)
{
    if (rootIndexList.size() < row + 1)
        rootIndexList.insert(row, rowVec);
    else
        rootIndexList[row] = rowVec;

    //访问rootVec中的数据，然后合并
    QVector<QString> rowData;

    QStringList reportList;
    QStringList patientNameList;
    QStringList dicomPatientIdList;
    QStringList birthdayList;
    QStringList ageList;
    QStringList sexList;
    QStringList modifyList;
    QStringList accessionNumberList;
    QStringList modalitiesList;
    QStringList surgerySummaryList;
    for (int i = 0; i < rowVec.size(); i++)
    {
        int rowIndex = rowVec.at(i);
       RowData rd = rootVec.at(rowIndex); //获取实际的数据
        reportList.append(rd.report);
        patientNameList.append(rd.patientName);
        dicomPatientIdList.append(rd.dicomPatientId);
        birthdayList.append(rd.birthday);
        ageList.append(rd.age);
        sexList.append(rd.sex);
        modifyList.append(rd.modifyDateTime);
        accessionNumberList.append(rd.accessionNumber);
        modalitiesList.append(rd.modalities);
        surgerySummaryList.append(rd.surgrySummary.trimmed());
    }

    //合并到一行
    RowData rds;

    reportList.removeDuplicates();
    rds.report = reportList;

    patientNameList.removeDuplicates();
    rds.patientName = patientNameList.join(";");

    dicomPatientIdList.removeDuplicates();
    rds.dicomPatientId = dicomPatientIdList.join(";");

    birthdayList.removeDuplicates();
    rds.birthday = birthdayList.join(";");

    ageList.removeDuplicates();
    rds.age = ageList.join(";");

    sexList.removeDuplicates();
    rds.sex = sexList.join(";");

    modifyList.removeDuplicates();
    rds.modifyDateTime = modifyList.join(";");

    accessionNumberList.removeDuplicates();
    rds.accessionNumber = accessionNumberList.join(";");

    modalitiesList.removeDuplicates();
    rds.modalities = modalitiesList.join(";");

    surgerySummaryList.removeDuplicates(); //比如两个空行，删除一个
    rds.surgrySummary = surgerySummaryList.join(";");

    addRowToTableWidget(row, rds);
}

//当合并的时候，采用全部删除，然后合并的方式
void PatientTable::insertRowToTableWhenMerge(int row, QVector<int> rowVec)
{
    rootIndexList.insert(row, rowVec); //直接插入到原先的行

    //访问rootVec中的数据，然后合并
    QStringList reportList;
    QStringList patientNameList;
    QStringList dicomPatientIdList;
    QStringList birthdayList;
    QStringList ageList;
    QStringList sexList;
    QStringList modifyList;
    QStringList accessionNumberList;
    QStringList modalitiesList;
    QStringList surgerySummaryList;
    for (int i = 0; i < rowVec.size(); i++)
    {
        int rowIndex = rowVec.at(i);
        RowData rd = rootVec.at(rowIndex); //获取实际的数据
        reportList.append(rd.report);
        patientNameList.append(rd.patientName);
        dicomPatientIdList.append(rd.dicomPatientId);
        birthdayList.append(rd.birthday);
        ageList.append(rd.age);
        sexList.append(rd.sex);
        modifyList.append(rd.modifyDateTime);
        accessionNumberList.append(rd.accessionNumber);
        modalitiesList.append(rd.modalities);
        surgerySummaryList.append(rd.surgrySummary.trimmed());
    }

    //合并到一行
    RowData rds;

    reportList.removeDuplicates();
    rds.report = reportList;

    patientNameList.removeDuplicates();
    rds.patientName = patientNameList.join(";");

    dicomPatientIdList.removeDuplicates();
    rds.dicomPatientId = dicomPatientIdList.join(";");

    birthdayList.removeDuplicates();
    rds.birthday = birthdayList.join(";");

    ageList.removeDuplicates();
    rds.age = ageList.join(";");

    sexList.removeDuplicates();
    rds.sex = sexList.join(";");

    modifyList.removeDuplicates();
    rds.modifyDateTime = modifyList.join(";");

    accessionNumberList.removeDuplicates();
    rds.accessionNumber = accessionNumberList.join(";");

    modalitiesList.removeDuplicates();
    rds.modalities = modalitiesList.join(";");

    surgerySummaryList.removeDuplicates(); //比如两个空行，删除一个
    rds.surgrySummary = surgerySummaryList.join(";");

    insertRowToTableWidget(row, rds);
}

void PatientTable::insertRowToTableWidget(int row, RowData rd)
{
    tableWidget->insertRow(row);

    QLabel *L1 = new QLabel();
    if (!rd.report.isEmpty())
    {
        L1->setPixmap(QPixmap(reportImage));
    }
    tableWidget->setCellWidget(row, Report_, L1);

    QTableWidgetItem *item1 = new QTableWidgetItem(rd.patientName.split(":")[0].trimmed());
    tableWidget->setItem(row, PatientName, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem(rd.dicomPatientId);
    tableWidget->setItem(row, DicomPatientId, item2);

    QTableWidgetItem *item3 = new QTableWidgetItem(rd.birthday);
    tableWidget->setItem(row, PatientBirthday, item3);

    QTableWidgetItem *item4 = new QTableWidgetItem(rd.age);
    tableWidget->setItem(row, PatientAge, item4);

    QTableWidgetItem *item5 = new QTableWidgetItem(rd.sex);
    tableWidget->setItem(row, PatientSex, item5);

    QTableWidgetItem *item6 = new QTableWidgetItem(rd.modifyDateTime);
    tableWidget->setItem(row, RecordModificationTime, item6);

    QTableWidgetItem *item7 = new QTableWidgetItem(rd.accessionNumber);
    tableWidget->setItem(row, AccessionNumber, item7);

    QTableWidgetItem *item8 = new QTableWidgetItem(rd.modalities);
    tableWidget->setItem(row, Modalities, item8);

    QTableWidgetItem *item9 = new QTableWidgetItem(rd.surgrySummary);
    tableWidget->setItem(row, SurgerSummary, item9);
}

void PatientTable::splitRowFromTable(QVector<int> rowVec) //rowVec包含了所有选中的行
{
    for each (int row in rowVec)
    {
        QVector<int>  rows = rootIndexList.at(row);
        if (rows.size() <= 1) //小于或等于1就不用分开
            return;

        delRowFormTable(QVector<int>(1, row)); //先删除后分离

        for (int i = 0; i < rows.size(); i++)
        {
            int rowIndex = rows.at(i);
            RowData rd = rootVec.at(rowIndex);
            QVector<RowData> rdVec(1, rd);
            addIndexToTable(rows.at(i), QVector<int>(1, rows.at(i)));
        }
    }
}

QStringList PatientTable::getPatientId(int row)
{
    QVector<int> rows = rootIndexList.at(row); //获取原始的行
    QStringList patientIdList;
    for (int i = 0; i < rows.size(); i++)
    {
        QString patientId = rootVec.at(rows.at(i)).patientId;
        patientIdList.append(patientId);
    }

    return patientIdList;
}

QVector<Patient> PatientTable::getPatients(int row)
{
    QVector<int> rootRowVec = rootIndexList.at(row);
    QVector<Patient> patientVec;
    for each (int rr in rootRowVec)
    {
        Patient patient;
        patient.patientId = rootVec.at(rr).patientId.toInt();
        patient.reportList = rootVec.at(rr).report;
        patient.patientName = rootVec.at(rr).patientName;
        patient.dicomPatientId = rootVec.at(rr).dicomPatientId;
        patient.birthday = rootVec.at(rr).birthday;
        patient.age = rootVec.at(rr).age;
        patient.sex = rootVec.at(rr).sex;
        patient.recordModificationTime = rootVec.at(rr).modifyDateTime;
        patient.surgerSummary = rootVec.at(rr).surgrySummary;
        patientVec.append(patient);
    }
    return patientVec;
}

QVector<RowData> PatientTable::getRowData(int row)
{
    QVector<int> rootRowVec = rootIndexList.at(row); //表格中row行所对应的原始二维表的行数
    QVector<RowData> rowDataVec;
    for each (int rr in rootRowVec) //从原始数据rootVec获取数据
    {
        RowData rd = rootVec.at(rr);
        rowDataVec.append(rd);
    }
    return rowDataVec;
}

void PatientTable::clearTable()
{
    tableWidget->setRowCount(0); //清空表格
    rootVec.clear(); //清空数据
    rootIndexList.clear(); //清空所对应的索引
    mergeSignIndexHash.clear();
    patientVec.clear(); //清空患者
}

void PatientTable::updateTableWidget()
{
    tableWidget->setRowCount(0);
    for (int row = 0; row < rootIndexList.size(); row++)
    {
        QVector<int> rootIndexVec = rootIndexList.at(row);

        QStringList reportList;
        QStringList patientNameList;
        QStringList dicomPatientIdList;
        QStringList birthdayList;
        QStringList ageList;
        QStringList sexList;
        QStringList modifyList;
        QStringList accessionNumberList;
        QStringList modalitiesList;
        QStringList surgerySummaryList;
        for (int i = 0; i < rootIndexVec.size(); i++)
        {
            int rootIndex = rootIndexVec.at(i);
            RowData rd = rootVec.at(rootIndex);
            reportList.append(rd.report);
            patientNameList.append(rd.patientName);
            dicomPatientIdList.append(rd.dicomPatientId);
            birthdayList.append(rd.birthday);
            ageList.append(rd.age);
            sexList.append(rd.sex);
            modifyList.append(rd.modifyDateTime);
            accessionNumberList.append(rd.accessionNumber);
            modalitiesList.append(rd.modalities);
            surgerySummaryList.append(rd.surgrySummary.trimmed());
        }

        //合并到一行
        RowData rds;

        reportList.removeDuplicates();
        rds.report = reportList;

        patientNameList.removeDuplicates();
        rds.patientName = patientNameList.join(";");

        dicomPatientIdList.removeDuplicates();
        rds.dicomPatientId = dicomPatientIdList.join(";");

        birthdayList.removeDuplicates();
        rds.birthday = birthdayList.join(";");

        ageList.removeDuplicates();
        rds.age = ageList.join(";");

        sexList.removeDuplicates();
        rds.sex = sexList.join(";");

        modifyList.removeDuplicates();
        rds.modifyDateTime = modifyList.join(";");

        accessionNumberList.removeDuplicates();
        rds.accessionNumber = accessionNumberList.join(";");

        modalitiesList.removeDuplicates();
        rds.modalities = modalitiesList.join(";");

        surgerySummaryList.removeDuplicates(); //比如两个空行，删除一个
        rds.surgrySummary = surgerySummaryList.join(";");

        insertRowToTableWidget(row, rds);
    }
}

QStringList PatientTable::merge(QVector<int> rowVec)
{
    QVector<int> rootIndexMergeVec;
    int size = rowVec.size();
    for (int i = 0; i < size; i++)
    {
        int row = rowVec.at(i);
        QVector<int> rootIndexVec = rootIndexList.at(row);
        rootIndexMergeVec.append(rootIndexVec);
    }

    int row = rowVec.at(0); //保存第一行

    for (int i = size - 1; i >= 0; i--)
    {
        int row = rowVec.at(i);
        rootIndexList.removeAt(row);
    }

    rootIndexList.insert(row, rootIndexMergeVec);

    QStringList patientIdList;
    for (int i = 0; i < rootIndexMergeVec.size(); i++)
    {
        //获取原始行的数据
        int index = rootIndexMergeVec.at(i);
        RowData rd = rootVec.at(index);
        patientIdList.append(rd.patientId);
    }

    return patientIdList; //返回合并标记
}

QStringList PatientTable::split(QVector<int> rowVec)
{
    std::sort(rowVec.begin(), rowVec.end()); //确保从小到大
    QStringList patientIdList;
    for (int i = rowVec.size() - 1; i >= 0; i--)
    {
        int row = rowVec.at(i);
        QVector<int> rootIndexVec = rootIndexList.at(row);
        if (rootIndexVec.size() < 2)
            continue;

        rootIndexList.removeAt(row);
        for (int j = 0; j < rootIndexVec.size(); j++)
        {
            QVector<int> _rowIndexVec(1, rootIndexVec.at(j));
            rootIndexList.insert(row, _rowIndexVec);
            row++;
            patientIdList.append(rootVec.at(rootIndexVec.at(j)).patientId);
        }
    }

    updateTableWidget();

    return patientIdList;
}

void PatientTable::search(QVector<int> rowVec)
{
    QList<QVector<int>> _rootIndexList;
    //取出对应的行
    for (int i = 0; i < rowVec.size(); i++)
    {
        int row = rowVec.at(i);
        QVector<int> rowIndexVec = rootIndexList.at(row);
        _rootIndexList.append(rowIndexVec);
    }

    rootIndexList = _rootIndexList;
    updateTableWidget();
}

void PatientTable::sort(QVector<int> rowVec)
{
    QList<QVector<int>> _rootIndexList;
    //取出对应的行
    for (int i = 0; i < rowVec.size(); i++)
    {
        int row = rowVec.at(i);
        QVector<int> rowIndexVec = rootIndexList.at(row);
        _rootIndexList.append(rowIndexVec);
    }

    rootIndexList = _rootIndexList;
    updateTableWidget();
}

void PatientTable::setMergeSignIndexHash(QString mergeSign, int index)
{
    if (mergeSign.isEmpty()) //如果为空，则直接插入原始列
        rootIndexList.append(QVector<int>(1, index));
    else //否则插入hash
    {
        QHash<QString, QVector<int>>::iterator it = mergeSignIndexHash.find(mergeSign);
        if (it == mergeSignIndexHash.end())
            mergeSignIndexHash.insert(mergeSign, QVector<int>(1, index));
        else
            it.value().append(index);
    }
}

void PatientTable::mergeSignIndexHashToRootIndexList()
{
    QHashIterator<QString, QVector<int> > it(mergeSignIndexHash);
    while (it.hasNext())
    {
        it.next();
        rootIndexList.append(it.value());
    }
}

void PatientTable::delRowFormTable(QVector<int> rowVec)
{
    qSort(rowVec); //确保从大到小删除
    for (int i = rowVec.size() - 1; i >= 0; i--)
    {
        int row = rowVec.at(i);
        rootIndexList.removeAt(row);
        tableWidget->removeRow(row);
    }
}

void PatientTable::addRow(RowData rd)
{
    int currentRow = tableWidget->rowCount();

    addRowDataToRootVec(rd);
    addIndexToTable(currentRow, QVector<int>(1, currentRow));
}

void PatientTable::addRowToTableWidget(int row, RowData rd)
{
    if (tableWidget->rowCount() > row + 1) //如果行数存在，先删除，然后修改
    {
        tableWidget->removeRow(row);
    }

    insertRowToTableWidget(row, rd);
}

