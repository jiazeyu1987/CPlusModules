#pragma once

#include <qtablewidget.h>
#include "qGLCommonType.h"

//此类存储、处理表格的数据，用于表格本身的数据改变和访问，如加载，导入，搜索，合并，分离
//访问数据的方式，是访问表格内部的数据，而不是表格所显示的数据，为此，设定两张严格同步，至关重要
//理论上内部的数据（table）和表格中显示的数据应该联动，所以应该封装在一起，包括添加，修改，删除等等。
//表格展示数据，内部数据用来处理数据
//本类只所以从PatientMangerView中抽出来，是为了更好的数据和显示的分离，本来主要关注对表格的改变的变化，而对于获取和点击不感兴趣
class PatientTable : public QObject
{
    Q_OBJECT
public:
    PatientTable(QTableWidget *tableWidget);

    void addRow(RowData rd); //添加行

    int addRowDataToRootVec(RowData rowData); //添加一行数据到原始数据集

    void addRowToTableWidget(int row, RowData rd); //添加行到TableWidget
    void delRowFormTable(QVector<int> rowVec); //删除行
    void addIndexToTable(int row, QVector<int> rowVec); //添加行到表格数据
    void insertRowToTableWhenMerge(int row, QVector<int> rowVec); //当合并的时候插入行
    void insertRowToTableWidget(int row, RowData rd); //当合并的时候插入数据
    void splitRowFromTable(QVector<int> rowVec); //分离给定的行
    QStringList getPatientId(int row); //获取某行所对应的patientId
    QVector<Patient> getPatients(int row); //获取某行对应的Patient
    QVector<RowData> getRowData(int row);
    void clearTable();
    void updateTableWidget();
    QStringList merge(QVector<int> rowVec); //合并行
    QStringList split(QVector<int> rowVec); //分离行
    void search(QVector<int> rowVec); //搜索
    void sort(QVector<int> rowVec); //排序
    void setMergeSignIndexHash(QString mergeSign, int row);
    void mergeSignIndexHashToRootIndexList();

    QVector<Patient> patientVec; //数据库的数据
    QHash<QString, QVector<int>> mergeSignIndexHash; //合并标志和行
    QVector<RowData> rootVec; //代表原始仓库，没有合并，分离，搜索等
    //QVector<int>代表指向原始数据的行数，表格的更新基于此，因此非常重要，使用QList的进行插入的时候，如果大于链表，直接插入后面
    QList<QVector<int>> rootIndexList;  //下标和表格的每一行对应

    QTableWidget *tableWidget;
    QString reportImage; //报告图片路径
};
