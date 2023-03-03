#pragma once

#include <qobject.h>
#include "qGLCommonType.h"
#include <qfileinfo.h>
#include <itkMetaDataDictionary.h>

class Dicom : public QObject
{
  Q_OBJECT
public:
	struct Miss
	{
		QString dir;
		QString seriesUID;
		QVector<int> missVec;
	};
public:
    explicit Dicom(QObject *parent = nullptr);
    ~Dicom();

    static void vtkReadDicom(std::string inputFilename);
	static  int itkReadDicom(const std::string& path, Patient& patient,
        PatientsSeries& patientsSequenceList);

	static int dcmtkReadDicom();

    // 给定任意目录（可能包含子目录）递归处理
	static void itkReadDicomSeriesAnyDir(QString dir);

    // 给出任意目录，按照序列自动分类好，并写入.nrrd文件
	static  int itkReadDicomSeries1(std::string dirName);

    // 给出任意目录（不包含子目录），按照序列自动分类好，读取序列标记
	//isJustGenPng is just generate cover.png
	static int itkReadDicomSeries(std::string dirName, QStringList& badFileList, QVector<struct Miss>& missVec);

	//isJustGenPng is just generate cover.png
	static  int itkReadDicom(std::vector<std::string>& fileNames, Patient& patient,
        PatientsSeries& series, QString moveDir = QString(), QVector<int>& missVec_ = QVector<int>());

	//判断数据是否有缺失
	//判断的依据是通过(0020,0013)可以知道当前图像在序列中的编号
	//通常情况下DICOM文件的命名与其序列编号顺序一致，但不排除例外
	//对于头和尾缺少的情况无法判断，除非事前知道序列的总数
	//顺序正确、序列完整	01.DCM（1）02.DCM（2）03.DCM（3）04.DCM（4）05.DCM（5）
	//顺序错误、序列完整	01.DCM（3）02.DCM（2）03.DCM（1）04.DCM（5）05.DCM（4）
	//顺序正确、序列缺失	01.DCM（1）02.DCM（2）03.DCM（4）04.DCM（6）05.DCM（7）
	static int dicomSequenceErrorAndSeriesMiss(std::string& fileName);

	static int gdcmReadDicom(std::string inputFilename);

	static void dcmtkDicomSaveToPng(std::string source, std::string target);

	//get the largest value and smallest value of a dicom
	static void GetMaxAndMinOfDicom(const  itk::MetaDataDictionary& dictionary, int& max, int& min);

	static QString itkDicomSaveToPng(std::string source, std::string target);

	static void ShowMissWarning(QVector<struct Miss> missVec, QStringList badFileList);

	static QFileInfoList getFileInfoFromDir(QString dir);


	static QFileInfoList fileInfoList;
};
