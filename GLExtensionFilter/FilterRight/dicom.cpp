#include "dicom.h"
#include <QDate>
#include <qdatetime.h>
#include <qdir.h>

#include <qdesktopwidget.h>
#include <qtextcodec.h>

//vtk
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//itk
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include <itkJPEGImageIO.h>
#include <itkPNGImageIO.h>

//dcmtk
#include <dcmtk/dcmdata/dcfilefo.h>
//#include <dcmtk/ofstd/ofstring.h>
#include <ctkDICOMImage.h>
#include <dcmtk/dcmimage/diregist.h>
#include <dcmtk/dcmimgle/dcmimage.h>


//gcdm
#include "gdcmImageReader.h"
#include "gdcmImageWriter.h"
#include "gdcmImage.h"
#include "gdcmPhotometricInterpretation.h"

//GLCustomDialog *Dicom::progressDialog = nullptr;
Dicom::Dicom(QObject *parent)
  : QObject(parent)
{
    //if (!progressDialog)
        //progressDialog = new GLCustomDialog;
}

Dicom::~Dicom()
{
}

///*
void Dicom::vtkReadDicom(std::string inputFilename)
{
  //vtkNew<vtkNamedColors> colors;

  ////Read all the DICOM files in the specified directory.
  //vtkNew<vtkDICOMImageReader> reader;
  //reader->SetFileName(inputFilename.c_str());
  //reader->Update();

  //// Visualize
  //vtkNew<vtkImageViewer2> imageViewer;
  //imageViewer->SetInputConnection(reader->GetOutputPort());
  //vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  //imageViewer->SetupInteractor(renderWindowInteractor);
  //imageViewer->Render();
  //imageViewer->GetRenderer()->SetBackground(
  //  colors->GetColor3d("SlateGray").GetData());
  //imageViewer->GetRenderWindow()->SetWindowName("ReadDICOM");
  //imageViewer->GetRenderer()->ResetCamera();
  //imageViewer->Render();

  //renderWindowInteractor->Start();
}
//*/


// This example illustrates how to read a DICOM series into a volume and then
// print most of the DICOM header information. The binary fields are skipped.
int Dicom::itkReadDicom(const std::string& path, Patient& patient,
    PatientsSeries& patientsSequenceList)
{
    patientsSequenceList.seriesDir = QString::fromStdString(path);
  // Next, we instantiate the type to be used for storing the image once it is
  // read into memory.
  using PixelType = signed short;
  constexpr unsigned int Dimension = 3;

  using ImageType = itk::Image<PixelType, Dimension>;

  // We use the image type for instantiating the series reader type and then we
  // construct one object of this class
  using ReaderType = itk::ImageSeriesReader<ImageType>;

  ReaderType::Pointer reader = ReaderType::New();

  // A GDCMImageIO object is created and assigned to the reader.
  using ImageIOType = itk::GDCMImageIO;

  ImageIOType::Pointer dicomIO = ImageIOType::New();

  reader->SetImageIO(dicomIO);

  // A GDCMSeriesFileName is declared in order to generate the names of DICOM
  // slice. We specify the directory with the SetInputDirectory() method
  // and, in this case, take the directory name from the command line arguments.
  // You could have obtained the directory name from a file dialog in a GUI.

  using NamesGeneratorType = itk::GDCMSeriesFileNames;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

  nameGenerator->SetInputDirectory(path);

  // The list of files to read is obtained from the name generator by invoking
  // the GetInputFileNames() method and receiving the result in a
  // container of strings. the list of filenames is passed to reader using
  // the SetFileNames() method.
  using FileNamesContainer = std::vector<std::string>;
  FileNamesContainer fileNames = nameGenerator->GetInputFileNames();
  patientsSequenceList.sliceCount = fileNames.size(); //��Ƭ��
  reader->SetFileNames(fileNames);

  // we trigger the reader by invoking the Update() method. This
  // invocation should normally be done inside a try/catch block given
  // that it may eventually throw exceptions.
  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject & ex)
  {
    std::cerr << ex << std::endl;
    return EXIT_FAILURE;
  }

  // ITK  internally queries GDCM and obtains all the DICOM tags from the file
  // headers. The tag values are stored in the MetaDataDictionary 
  // which is a general-purpose container for \{key, vlaue\} pairs. The Metadata
  // dictionary can be recvoered from any ImageIO class by invoking the 
  // GetMetaDataDictionary() method.
  using DictionaryType = itk::MetaDataDictionary;

  const DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();

  // In this example, we are only interersted in the DICOM tags that can be
  // represented as strings. Therefore, we declare a MetaDataObject of
  // string type in order to receive those particular values.
  using MetaDataStringType = itk::MetaDataObject<std::string>;

  // The metadata dictionary is organized as a container with its corresponding 
  // iterators. We can therefore visit all its entries by first getting access to 
  // its Begin() and end() methods.
  auto itr = dictionary.Begin();
  auto end = dictionary.End();

  // We are now ready for walking through the list of DICOM tags. for this
  // purpose we use the iterators that we just declared. At every entry we 
  // attempt to convert it into a string entry by using the dynamic_cast
  // based on RTTI information. The
  // dictionary is organized like a std::map structure, so we should use
  // the first and second members of every entry in order
  // to get access to the {key, vlaue} pairs.
  while (itr != end)
  {
    itk::MetaDataObjectBase::Pointer entry = itr->second;

    MetaDataStringType::Pointer entryvalue = dynamic_cast<MetaDataStringType *>(entry.GetPointer());

    if (entryvalue)
    {
      std::string tagkey = itr->first;
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
    }

    ++itr;
  }

  // It is also possible to query for specific entries instead of reading all of 
  // them as we did above. In this case. the user must provide the tag
  // identifier using the standard DICOM encoding. The identifier is stored in a
  // string and used as key in the dictionary.
  std::string entryIdPatientName = "0010|0010";
  std::string entryIdPatientID = "0010|0020"; //����ID
  std::string entryIdPatientBirthDate = "0010|0030";
  std::string entryIdPatientSex = "0010|0040";
  std::string entryIdModality = "0008|0060";
  std::string entryIdAccessionNumber = "0008|0050";

  std::string entryIdSeriesNumber = "0020|0011"; //���кţ�ʶ��ͬ���ĺ�.
  std::string entryIdSeriesDate = "0008|0021";
  std::string entryIdSeriesTime = "0008|0031";
  std::string entryIdSeriesDescription = "0008|103e";
  //std::string entryIdNumberOfSlices = "0054|0081";
  std::string entryIdSeriesInstanceUID = "0020|000e"; //����ʵ���ţ�Ψһ��ǲ�ͬ���еĺ���.  //ע���ϸ����ִ�Сд

  std::string entryIdPixelSpacing = "0028|0030"; //���ؼ��

  std::string entryIdEncode = "0008|0005"; //����

  auto tagItrPatientName = dictionary.Find(entryIdPatientName);
  auto tagItrPatientID = dictionary.Find(entryIdPatientID);
  auto tagItrPatientBirthDate = dictionary.Find(entryIdPatientBirthDate);
  auto tagItrPatientSex = dictionary.Find(entryIdPatientSex);
  auto tagItrModality = dictionary.Find(entryIdModality);
  auto tagItrAccessionNumber = dictionary.Find(entryIdAccessionNumber);

  auto tagItrSeriesNumber = dictionary.Find(entryIdSeriesNumber);
  auto tagItrSeriesDate = dictionary.Find(entryIdSeriesDate);
  auto tagItrPatientSeriesTime = dictionary.Find(entryIdSeriesTime);
  auto tagItrSeriesDescription = dictionary.Find(entryIdSeriesDescription);
  auto tagItrSeriesInstanceUID = dictionary.Find(entryIdSeriesInstanceUID);

  auto tagItrPixelSpacing = dictionary.Find(entryIdPixelSpacing);

  auto tagItrEncode = dictionary.Find(entryIdEncode);

  MetaDataStringType::ConstPointer entryvaluePatientName = nullptr;
  if (tagItrPatientName == end)
  {
    std::cerr << "Tag " << entryIdPatientName;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvaluePatientName = dynamic_cast<const MetaDataStringType *>(tagItrPatientName->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvaluePatientID = nullptr;
  if (tagItrPatientID == end)
  {
    std::cerr << "Tag " << entryIdPatientID;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvaluePatientID = dynamic_cast<const MetaDataStringType *>(tagItrPatientID->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvaluePatientBirthDate = nullptr;
  if (tagItrPatientBirthDate == end)
  {
    std::cerr << "Tag " << entryIdPatientBirthDate;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvaluePatientBirthDate = dynamic_cast<const MetaDataStringType *>(tagItrPatientBirthDate->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvaluePatientSex = nullptr;
  if (tagItrPatientSex == end)
  {
    std::cerr << "Tag " << entryIdPatientSex;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvaluePatientSex = dynamic_cast<const MetaDataStringType *>(tagItrPatientSex->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvalueModality = nullptr;
  if (tagItrModality == end)
  {
    std::cerr << "Tag " << entryIdModality;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvalueModality = dynamic_cast<const MetaDataStringType *>(tagItrModality->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvalueAccessionNumber = nullptr;
  if (tagItrAccessionNumber == end)
  {
    std::cerr << "Tag " << entryIdAccessionNumber;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvalueAccessionNumber = dynamic_cast<const MetaDataStringType *>(tagItrAccessionNumber->second.GetPointer());
  }

  //------------------------------------------------
  MetaDataStringType::ConstPointer entryvalueSeriesNumber = nullptr;
  if (tagItrSeriesNumber == end)
  {
    std::cerr << "Tag " << entryIdSeriesNumber;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvalueSeriesNumber = dynamic_cast<const MetaDataStringType *>(tagItrSeriesNumber->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvalueSeriesDate = nullptr;
  if (tagItrSeriesDate == end)
  {
    std::cerr << "Tag " << entryIdSeriesDate;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvalueSeriesDate = dynamic_cast<const MetaDataStringType *>(tagItrSeriesDate->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvalueSeriesTime = nullptr;
  if (tagItrPatientSeriesTime == end)
  {
    std::cerr << "Tag " << entryIdSeriesTime;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvalueSeriesTime = dynamic_cast<const MetaDataStringType *>(tagItrPatientSeriesTime->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvalueSeriesDescription = nullptr;
  if (tagItrSeriesDescription == end)
  {
    std::cerr << "Tag " << entryIdSeriesDescription;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvalueSeriesDescription = dynamic_cast<const MetaDataStringType *>(tagItrSeriesDescription->second.GetPointer());
  }

  MetaDataStringType::ConstPointer entryvalueSeriesInstanceUID = nullptr;
  if (tagItrSeriesInstanceUID == end)
  {
    std::cerr << "Tag " << entryIdSeriesInstanceUID;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvalueSeriesInstanceUID = dynamic_cast<const MetaDataStringType *>(tagItrSeriesInstanceUID->second.GetPointer());
  }

  //-----------------------------------------------------
  MetaDataStringType::ConstPointer entryvaluePixelSpacing = nullptr;
  if (tagItrPixelSpacing == end)
  {
    std::cerr << "Tag " << entryIdPixelSpacing;
    std::cerr << " not found in the DICOM header" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    entryvaluePixelSpacing = dynamic_cast<const MetaDataStringType *>(tagItrPixelSpacing->second.GetPointer());
  }

  // Since the entry may or may not be of string type we must again use a
  // dynamic_cast in order to attempe to convert it to a string dictionary
  // entry. If the conversion is successful, we can then print out its content.
  if (entryvaluePatientName)
  {
    std::string tagvalue = entryvaluePatientName->GetMetaDataObjectValue();
    patient.patientName = QString::fromStdString(tagvalue).trimmed();
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  if (entryvaluePatientID)
  {
    std::string tagvalue = entryvaluePatientID->GetMetaDataObjectValue();
    patient.dicomPatientId = QString::fromStdString(tagvalue).trimmed();
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  if (entryvaluePatientBirthDate)
  {
    std::string tagvalue = entryvaluePatientBirthDate->GetMetaDataObjectValue();
    QDate dt = QDate::fromString(QString::fromStdString(tagvalue), "yyyyMMdd");
    patient.birthday = dt.toString("yyyy/MM/dd");
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  if (entryvaluePatientSex)
  {
    std::string tagvalue = entryvaluePatientSex->GetMetaDataObjectValue();
    patient.sex = QString::fromStdString(tagvalue).trimmed();
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  if (entryvalueModality)
  { 
    std::string tagvalue = entryvalueModality->GetMetaDataObjectValue();
    patientsSequenceList.modality = QString::fromStdString(tagvalue);
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  if (entryvalueAccessionNumber)
  {
    std::string tagvalue = entryvalueAccessionNumber->GetMetaDataObjectValue();
    patientsSequenceList.accessionNumber = QString::fromStdString(tagvalue);
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  //--------------------------------------------------------
  if (entryvalueSeriesNumber)
  {
    std::string tagvalue = entryvalueSeriesNumber->GetMetaDataObjectValue();
    patientsSequenceList.seriesNumber = atoi(tagvalue.c_str());
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  QDateTime datetime;
  if (entryvalueSeriesDate)
  {
    std::string tagvalue = entryvalueSeriesDate->GetMetaDataObjectValue();
    QDate dt = QDate::fromString(QString::fromStdString(tagvalue), "yyyyMMdd");  
    datetime.setDate(dt);
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  if (entryvalueSeriesTime)
  {
    std::string tagvalue = entryvalueSeriesTime->GetMetaDataObjectValue();
    QString dt_str = QString::fromStdString(tagvalue).trimmed().right(6);
    QTime dt = QTime::fromString(dt_str, "hhmmss.zzz");
    datetime.setTime(dt);
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }
  patientsSequenceList.scanTime = datetime.toString("yyyy/MM/dd hh:mm:ss.zzz");
  if (entryvalueSeriesDescription)
  {
    std::string tagvalue = entryvalueSeriesDescription->GetMetaDataObjectValue();
    patientsSequenceList.seriesDescribe = QString::fromStdString(tagvalue).trimmed();
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  if (entryvalueSeriesInstanceUID)
  {
    std::string tagvalue = entryvalueSeriesInstanceUID->GetMetaDataObjectValue();
    patientsSequenceList.seriesInstanceUID = QString::fromStdString(tagvalue);
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }

  //-----------------------------------------------------------------
  if (entryvaluePixelSpacing)
  {

    std::string tagvalue = entryvaluePixelSpacing->GetMetaDataObjectValue();
  }
  else
  {
    std::cerr << "Entry was not of string type" << std::endl;
    //return EXIT_FAILURE;
  }



  // This type of functionality will probably be more useful when provided
  // through a graphical user interface. For a full description of the DICOM
  // dictionary please look at the following file.
  return EXIT_SUCCESS;
}

int Dicom::dcmtkReadDicom()
{
    //��ȡһ��DICOM�ļ�
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile("test.dcm");
    if (status.good())
    {
        OFString patientName;
        DcmTagKey patientNameKey(0x0010, 0x0010);
        if (fileformat.getDataset()->findAndGetOFString(patientNameKey, patientName).good())
        {
            cout << "Patient's Name: " << patientName << endl;
        }
        else
        {
            cerr << "Error: cannot access Patient's Name!" << endl;
        }
        return 1;
    }
    else
    {
        cerr << "Error: cannot read DICOM file (" << status.text() << ")" << endl;
        return 0;
    }

}

QFileInfoList Dicom::getFileInfoFromDir(QString dir)
{
    QDir _dir(dir);
    QFileInfoList _fileInfoList = _dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < _fileInfoList.size(); i++)
    {
        getFileInfoFromDir(_fileInfoList.at(i).filePath());
    }

    return _fileInfoList;
}

// ��������Ŀ¼�����ܰ�����Ŀ¼���ݹ鴦����ʱ��������ͬ�����зֲ��ڲ�ͬ���ļ�����
void Dicom::itkReadDicomSeriesAnyDir(QString path)
{
    //�Ȼ�ȡ����Ŀ¼
    QVector<QString> dirVec{path}; //�������л�ȡ��Ŀ¼
    QFileInfoList fileInfoList = getFileInfoFromDir(path);

    for (int i = 0; i < fileInfoList.size(); i++)
    {
        QString str = fileInfoList.at(i).filePath();
        dirVec << str;
    }

    QDesktopWidget *desk = QApplication::desktop();
    //����ʹ�þ�̬�ľֲ���������ȫ�ֻ��߾�̬��Աָ�룬��Ϊ�������ǵ�ʵ�ʳ�ʼ�������ڳ�������֮�󣬶�����֮ǰ
    //���ʹ��ȫ�ֵĻ��߾�̬�ĳ�Ա������Ϊ������û�����ɣ�QWidget��Ҫ��QApplication֮�󣩣���ʵ��û�м��ؽ�����
    //���ʹ�þֲ��������Ǿ�̬�������һ�ο��Գɹ����ڶ��α��������ԭ����ʱ����
    int size = dirVec.size();

    QStringList badFileList;
    QVector<struct Miss> missVec;
    for (int i = 0; i < size; i++) //��ʹ�õݹ���ã�Ϊ�˸���ʹ�ý�����
    {   
        QByteArray ba = dirVec.at(i).toLocal8Bit(); //��������
        itkReadDicomSeries(std::string(ba), badFileList, missVec);
    }

	ShowMissWarning(missVec, badFileList);
}

// ��������Ŀ¼�����������Զ�����ã���д��.nrrd�ļ�
int Dicom::itkReadDicomSeries1(std::string dirName)
{
    using PixelType = signed short;
    constexpr unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;

    using NamesGeneratorType = itk::GDCMSeriesFileNames;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

    nameGenerator->SetUseSeriesDetails(true);
    nameGenerator->AddSeriesRestriction("0008|0021");
    nameGenerator->SetGlobalWarningDisplay(false);
    nameGenerator->SetDirectory(dirName);

    try
    {
        using SeriesIdContainer = std::vector<std::string>;
        const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
        auto seriesItr = seriesUID.begin();
        auto seriesEnd = seriesUID.end();

        if (seriesItr != seriesEnd)
        {
            /*std::cout << "The directory: ";
            std::cout << dirName << std::endl;
            std::cout << "Contains the following DICOM Series: ";
            std::cout << std::endl;*/
        }
        else
        {
            std::cout << "No DICOMs in: " << dirName << std::endl;
            return EXIT_SUCCESS;
        }

       /* while (seriesItr != seriesEnd)
        {
            std::cout << seriesItr->c_str() << std::endl;
            ++seriesItr;
        }*/

        seriesItr = seriesUID.begin();
        while (seriesItr != seriesUID.end())
        {
            std::string seriesIdentifier;
            seriesIdentifier = seriesItr->c_str();
            seriesItr++;
            using FileNamesContainer = std::vector<std::string>;
            FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);

            using ReaderType = itk::ImageSeriesReader<ImageType>;
            ReaderType::Pointer reader = ReaderType::New();
            using ImageIOType = itk::GDCMImageIO;
            ImageIOType::Pointer dicomIO = ImageIOType::New();
            reader->SetImageIO(dicomIO);
            reader->SetFileNames(fileNames);
            //reader->ForceOrthogonalDirectionOff();

            using WriterType = itk::ImageFileWriter<ImageType>;
            WriterType::Pointer writer = WriterType::New();
            std::string outFileName;
            outFileName = dirName + std::string("/") + seriesIdentifier + ".nrrd";
            writer->SetFileName(outFileName);
            writer->UseCompressionOn();
            writer->SetInput(reader->GetOutput());
            try
            {
                writer->Update();
            }
            catch (itk::ExceptionObject & ex)
            {
                std::cerr << ex << std::endl;
                continue;
            }
        }
    }
    catch (itk::ExceptionObject & ex)
    {
        std::cerr << ex << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// ��������Ŀ¼�����������Զ�����ã���ȡ���б��
int Dicom::itkReadDicomSeries(std::string dirName, QStringList& badFileList, QVector<struct Miss>& missVec)
{
    using PixelType = signed short;
    constexpr unsigned int Dimension = 3;
    using ImageType = itk::Image<PixelType, Dimension>;

    using NamesGeneratorType = itk::GDCMSeriesFileNames;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

    //ʹ�ø��ӵ�ϵ����Ϣ(��ProtocolName��SeriesName)��ʶ�𵥸�SeriesUID��ʱ�������3D������ע��DTI������ܻ�����������
    nameGenerator->SetUseSeriesDetails(true);
    //��Series��ѡ����Ӹ������ơ� ����ѭ��SetUseSeriesDetails��ͬ�ķ������������û���Ӹ����DICOM��ǣ�
    //�Ա㽫һ��DICOM�ļ�ϸ��Ϊ���ϵ�С� ��ǩ�ĸ�ʽΪDICOM��ǩ�ġ�group|element���� �û���Ҫ����SetUseSeriesDetails(true) 
    nameGenerator->AddSeriesRestriction("0008|0021");
    nameGenerator->SetGlobalWarningDisplay(false); //�Ƿ���ʾ�κδ�����Ϣ�����Ƿ񵯳�������Ϣ��vtkOutputWindow
    nameGenerator->SetDirectory(dirName);

    //��ȡ���ļ�
    QDir dir(QString::fromStdString(dirName));
    QFileInfoList totalInfoList = dir.entryInfoList(QDir::Files); //��ȡĿ¼�����е��ļ�
    QStringList totalList;
    for (int i = 0; i < totalInfoList.size(); i++)
    {
        totalList.append(totalInfoList.at(i).filePath());
    }

    try
    {
        using SeriesIdContainer = std::vector<std::string>;
        const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
        auto seriesItr = seriesUID.begin();
        auto seriesEnd = seriesUID.end();

        if (seriesItr != seriesEnd)
        {
        }
        else
        {
            std::cerr << "No DICOMs in: " << dirName << std::endl;
            badFileList << totalList;
            return EXIT_SUCCESS;
        }

        while (seriesItr != seriesEnd)
        {
            ++seriesItr;
        }

        seriesItr = seriesUID.begin();
        //��һ�ο϶�����Ҫ�ƶ��ļ�
        std::string seriesIdentifier;
        seriesIdentifier = seriesItr->c_str();
        seriesItr++;
        using FileNamesContainer = std::vector<std::string>;
        FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);
        //ȥ������dcm�ļ�
        for (int i = 0; i < fileNames.size(); i++)
        {
            totalList.removeAll(QString::fromLocal8Bit(fileNames.at(i).c_str()));
        }
        Patient patient;
        PatientsSeries patientsSequenceList;
        Miss miss;
        itkReadDicom(fileNames, patient, patientsSequenceList, QString(), miss.missVec);
        if (miss.missVec.size() > 0)
        {
            QTextCodec *codec = QTextCodec::codecForName("GBK");
            miss.dir = codec->toUnicode(dirName.data());
            miss.seriesUID = QString::fromStdString(seriesIdentifier);
            missVec.append(miss);
        }
        
        //������Ҫ�ƶ��ļ�
        int file_ = 1;
        while (seriesItr != seriesUID.end())
        {
            std::string seriesIdentifier;
            seriesIdentifier = seriesItr->c_str();
            seriesItr++;
            using FileNamesContainer = std::vector<std::string>;
            FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);
            //ȥ������dcm�ļ�
            for (int i = 0; i < fileNames.size(); i++)
            {
                totalList.removeAll(QString::fromStdString(fileNames.at(i)));
            }
            Patient patient;
            PatientsSeries patientsSequenceList;
            QString moveDir = QString::fromStdString(dirName + "-" + std::to_string(file_++));
            Miss miss;
            itkReadDicom(fileNames, patient, patientsSequenceList, moveDir, miss.missVec);
            if (miss.missVec.size() > 0)
            {
                miss.dir = QString::fromStdString(dirName);
                miss.seriesUID = QString::fromStdString(seriesIdentifier);
                missVec.append(miss);
            }
        }
    }
    catch (itk::ExceptionObject & ex)
    {
        std::cerr << ex << std::endl;
        
        badFileList << totalList;
        return EXIT_FAILURE;
    }

    badFileList << totalList;
    return EXIT_SUCCESS;
}

// This example illustrates how to read a DICOM series into a volume and then
// print most of the DICOM header information. The binary fields are skipped.
int Dicom::itkReadDicom(std::vector<std::string>& fileNames, Patient& patient,
    PatientsSeries& series, QString moveDir, QVector<int>& missVec_)
{
    //�ж������Ƿ�ȱʧ
    QVector<int> missVec;
    int count = fileNames.size();
    for (int i = 0; i < count; i++)
    {
        int value = dicomSequenceErrorAndSeriesMiss(fileNames.at(i));
        if (value >= 0)
            missVec.append(value);
    }
    std::sort(missVec.begin(), missVec.end());

    if ((count != (missVec.last() - missVec.first() + 1)) || (count != missVec.size()))
    {
        for (int j = 0; j < missVec.size() - 1; j++)
        {
            int diffVal = missVec.at(j + 1) - missVec.at(j);
            if (diffVal != 1)
            {
                for (int k = 1; k < diffVal; k++)
                {
                    missVec_ << missVec.at(j) + k;
                }
            }
        }
    }

    
    // Next, we instantiate the type to be used for storing the image once it is
    // read into memory.
    using PixelType = signed short;
    constexpr unsigned int Dimension = 3;

    using ImageType = itk::Image<PixelType, Dimension>;

    // We use the image type for instantiating the series reader type and then we
    // construct one object of this class
    using ReaderType = itk::ImageSeriesReader<ImageType>;

    ReaderType::Pointer reader = ReaderType::New();

    // A GDCMImageIO object is created and assigned to the reader.
    using ImageIOType = itk::GDCMImageIO;

    ImageIOType::Pointer dicomIO = ImageIOType::New();

    reader->SetImageIO(dicomIO);

    // A GDCMSeriesFileName is declared in order to generate the names of DICOM
    // slice. We specify the directory with the SetInputDirectory() method
    // and, in this case, take the directory name from the command line arguments.
    // You could have obtained the directory name from a file dialog in a GUI.

    using NamesGeneratorType = itk::GDCMSeriesFileNames;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

    // The list of files to read is obtained from the name generator by invoking
    // the GetInputFileNames() method and receiving the result in a
    // container of strings. the list of filenames is passed to reader using
    // the SetFileNames() method.
    using FileNamesContainer = std::vector<std::string>;
    
    series.sliceCount = fileNames.size(); //��Ƭ�� amount��׼ȷ
    reader->SetFileNames(fileNames);

    // we trigger the reader by invoking the Update() method. This
    // invocation should normally be done inside a try/catch block given
    // that it may eventually throw exceptions.
    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject & ex)
    {
        std::cerr << ex << std::endl;
        return EXIT_FAILURE;
    }

    // ITK  internally queries GDCM and obtains all the DICOM tags from the file
    // headers. The tag values are stored in the MetaDataDictionary 
    // which is a general-purpose container for \{key, vlaue\} pairs. The Metadata
    // dictionary can be recvoered from any ImageIO class by invoking the 
    // GetMetaDataDictionary() method.
    using DictionaryType = itk::MetaDataDictionary;

    const DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();

    // In this example, we are only interersted in the DICOM tags that can be
    // represented as strings. Therefore, we declare a MetaDataObject of
    // string type in order to receive those particular values.
    using MetaDataStringType = itk::MetaDataObject<std::string>;

    // The metadata dictionary is organized as a container with its corresponding 
    // iterators. We can therefore visit all its entries by first getting access to 
    // its Begin() and end() methods.
    auto itr = dictionary.Begin();
    auto end = dictionary.End();

    // We are now ready for walking through the list of DICOM tags. for this
    // purpose we use the iterators that we just declared. At every entry we 
    // attempt to convert it into a string entry by using the dynamic_cast
    // based on RTTI information. The
    // dictionary is organized like a std::map structure, so we should use
    // the first and second members of every entry in order
    // to get access to the {key, vlaue} pairs.
    while (itr != end)
    {
        itk::MetaDataObjectBase::Pointer entry = itr->second;

        MetaDataStringType::Pointer entryvalue = dynamic_cast<MetaDataStringType *>(entry.GetPointer());

        if (entryvalue)
        {
            std::string tagkey = itr->first;
            std::string tagvalue = entryvalue->GetMetaDataObjectValue();
        }

        ++itr;
    }

    // It is also possible to query for specific entries instead of reading all of 
    // them as we did above. In this case. the user must provide the tag
    // identifier using the standard DICOM encoding. The identifier is stored in a
    // string and used as key in the dictionary.
    std::string entryIdPatientName = "0010|0010";
    std::string entryIdPatientID = "0010|0020"; //����ID
    std::string entryIdPatientBirthDate = "0010|0030";
    std::string entryIdPatientSex = "0010|0040";
    std::string entryIdModality = "0008|0060";
    std::string entryIdAccessionNumber = "0008|0050";

    std::string entryIdSeriesNumber = "0020|0011"; //���кţ�ʶ��ͬ���ĺ�.
    std::string entryIdSeriesDate = "0008|0021";
    std::string entryIdSeriesTime = "0008|0031";
    std::string entryIdSeriesDescription = "0008|103e";
    std::string entryIdNumberOfSlices = "0054|0081"; //�Ǳ�ѡ��tag���󲿷�����û�д�ѡ��
    std::string entryIdSeriesInstanceUID = "0020|000e"; //����ʵ���ţ�Ψһ��ǲ�ͬ���еĺ���.  //ע���ϸ����ִ�Сд

    std::string entryIdPixelSpacing = "0028|0030"; //���ؼ��
    std::string entryIdThickness = "0018|0050"; //���
    std::string entryIdSpaceBetween = "0018|0088"; //�����֮��ļ��

    std::string entryIdEncode = "0008|0005"; //����

    auto tagItrPatientName = dictionary.Find(entryIdPatientName);
    auto tagItrPatientID = dictionary.Find(entryIdPatientID);
    auto tagItrPatientBirthDate = dictionary.Find(entryIdPatientBirthDate);
    auto tagItrPatientSex = dictionary.Find(entryIdPatientSex);
    auto tagItrModality = dictionary.Find(entryIdModality);
    auto tagItrAccessionNumber = dictionary.Find(entryIdAccessionNumber);

    auto tagItrSeriesNumber = dictionary.Find(entryIdSeriesNumber);
    auto tagItrSeriesDate = dictionary.Find(entryIdSeriesDate);
    auto tagItrPatientSeriesTime = dictionary.Find(entryIdSeriesTime);
    auto tagItrSeriesDescription = dictionary.Find(entryIdSeriesDescription);
    auto tagItrSeriesInstanceUID = dictionary.Find(entryIdSeriesInstanceUID);

    auto tagItrPixelSpacing = dictionary.Find(entryIdPixelSpacing);
    auto tagItrThickness = dictionary.Find(entryIdThickness);
    auto tagItrSpaceBetween = dictionary.Find(entryIdSpaceBetween);

    auto tagItrEncode = dictionary.Find(entryIdEncode);
    //------------------------------------------------

    MetaDataStringType::ConstPointer entryvaluePatientName = nullptr;
    if (tagItrPatientName == end)
    {
        std::cerr << "Tag " << entryIdPatientName;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvaluePatientName = dynamic_cast<const MetaDataStringType *>(tagItrPatientName->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvaluePatientID = nullptr;
    if (tagItrPatientID == end)
    {
        std::cerr << "Tag " << entryIdPatientID;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvaluePatientID = dynamic_cast<const MetaDataStringType *>(tagItrPatientID->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvaluePatientBirthDate = nullptr;
    if (tagItrPatientBirthDate == end)
    {
        std::cerr << "Tag " << entryIdPatientBirthDate;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvaluePatientBirthDate = dynamic_cast<const MetaDataStringType *>(tagItrPatientBirthDate->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvaluePatientSex = nullptr;
    if (tagItrPatientSex == end)
    {
        std::cerr << "Tag " << entryIdPatientSex;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvaluePatientSex = dynamic_cast<const MetaDataStringType *>(tagItrPatientSex->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueModality = nullptr;
    if (tagItrModality == end)
    {
        std::cerr << "Tag " << entryIdModality;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueModality = dynamic_cast<const MetaDataStringType *>(tagItrModality->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueAccessionNumber = nullptr;
    if (tagItrAccessionNumber == end)
    {
        std::cerr << "Tag " << entryIdAccessionNumber;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueAccessionNumber = dynamic_cast<const MetaDataStringType *>(tagItrAccessionNumber->second.GetPointer());
    }

    
    MetaDataStringType::ConstPointer entryvalueSeriesNumber = nullptr;
    if (tagItrSeriesNumber == end)
    {
        std::cerr << "Tag " << entryIdSeriesNumber;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueSeriesNumber = dynamic_cast<const MetaDataStringType *>(tagItrSeriesNumber->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueSeriesDate = nullptr;
    if (tagItrSeriesDate == end)
    {
        std::cerr << "Tag " << entryIdSeriesDate;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueSeriesDate = dynamic_cast<const MetaDataStringType *>(tagItrSeriesDate->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueSeriesTime = nullptr;
    if (tagItrPatientSeriesTime == end)
    {
        std::cerr << "Tag " << entryIdSeriesTime;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueSeriesTime = dynamic_cast<const MetaDataStringType *>(tagItrPatientSeriesTime->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueSeriesDescription = nullptr;
    if (tagItrSeriesDescription == end)
    {
        std::cerr << "Tag " << entryIdSeriesDescription;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueSeriesDescription = dynamic_cast<const MetaDataStringType *>(tagItrSeriesDescription->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueSeriesInstanceUID = nullptr;
    if (tagItrSeriesInstanceUID == end)
    {
        std::cerr << "Tag " << entryIdSeriesInstanceUID;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueSeriesInstanceUID = dynamic_cast<const MetaDataStringType *>(tagItrSeriesInstanceUID->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvaluePixelSpacing = nullptr;
    if (tagItrPixelSpacing == end)
    {
        std::cerr << "Tag " << entryIdPixelSpacing;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvaluePixelSpacing = dynamic_cast<const MetaDataStringType *>(tagItrPixelSpacing->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueThickness = nullptr;
    if (tagItrThickness == end)
    {
        std::cerr << "Tag " << entryIdThickness;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueThickness = dynamic_cast<const MetaDataStringType *>(tagItrThickness->second.GetPointer());
    }

    MetaDataStringType::ConstPointer entryvalueSpaceBetween = nullptr;
    if (tagItrSpaceBetween == end)
    {
        std::cerr << "Tag " << entryIdSpaceBetween;
        std::cerr << " not found in the DICOM header" << std::endl;
        //return EXIT_FAILURE;
    }
    else
    {
        entryvalueSpaceBetween = dynamic_cast<const MetaDataStringType *>(tagItrSpaceBetween->second.GetPointer());
    }


    //---------------------------------��ʼ��ȡ����--------------------------
    // Since the entry may or may not be of string type we must again use a
    // dynamic_cast in order to attempe to convert it to a string dictionary
    // entry. If the conversion is successful, we can then print out its content.
    if (entryvaluePatientName)
    {
        std::string tagvalue = entryvaluePatientName->GetMetaDataObjectValue();
        patient.patientName = QString::fromStdString(tagvalue).trimmed();
        series.dicomPatientName = patient.patientName;
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvaluePatientID)
    {
        std::string tagvalue = entryvaluePatientID->GetMetaDataObjectValue();
        patient.dicomPatientId = QString::fromStdString(tagvalue).trimmed();
        series.dicomPatientId = patient.dicomPatientId;
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvaluePatientBirthDate)
    {
        std::string tagvalue = entryvaluePatientBirthDate->GetMetaDataObjectValue();
        QDate dt = QDate::fromString(QString::fromStdString(tagvalue), "yyyyMMdd");
        patient.birthday = dt.toString("yyyy/MM/dd");
        series.dicomBirthday = patient.birthday;
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvaluePatientSex)
    {
        std::string tagvalue = entryvaluePatientSex->GetMetaDataObjectValue();
        patient.sex = QString::fromStdString(tagvalue).trimmed();
        series.dicomSex = patient.sex;
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvalueModality)
    {
        std::string tagvalue = entryvalueModality->GetMetaDataObjectValue();
        series.modality = QString::fromStdString(tagvalue);
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvalueAccessionNumber)
    {
        std::string tagvalue = entryvalueAccessionNumber->GetMetaDataObjectValue();
        // ���������dicomPatientId��ͬʱ����ʾͬһ���ߣ���ʱ�������һ���µĻ��ߣ���������������֮ǰ�ļ�¼��ͬ����Ӧ����ҽ��ѡ�񣬱�ʾ����
        series.accessionNumber = QString::fromStdString(tagvalue).trimmed();
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvalueSeriesNumber)
    {
        std::string tagvalue = entryvalueSeriesNumber->GetMetaDataObjectValue();
        series.seriesNumber = atoi(tagvalue.c_str());
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    QDateTime datetime;
    if (entryvalueSeriesDate)
    {
        std::string tagvalue = entryvalueSeriesDate->GetMetaDataObjectValue();
        QDate dt = QDate::fromString(QString::fromStdString(tagvalue), "yyyyMMdd");
        datetime.setDate(dt);
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvalueSeriesTime)
    {
        std::string tagvalue = entryvalueSeriesTime->GetMetaDataObjectValue();
        QString dt_str = QString::fromStdString(tagvalue).trimmed().right(6);
        QTime dt = QTime::fromString(dt_str, "hhmmss.zzz");
        datetime.setTime(dt);
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }
    series.scanTime = datetime.toString("yyyy/MM/dd hh:mm:ss.zzz");
    if (entryvalueSeriesDescription)
    {
        std::string tagvalue = entryvalueSeriesDescription->GetMetaDataObjectValue();
        series.seriesDescribe = QString::fromStdString(tagvalue).trimmed();
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvalueSeriesInstanceUID)
    {
        std::string tagvalue = entryvalueSeriesInstanceUID->GetMetaDataObjectValue();
        series.seriesInstanceUID = QString::fromStdString(tagvalue);
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    //-----------------------------------------------------------------
    if (entryvaluePixelSpacing)
    {
        std::string tagvalue = entryvaluePixelSpacing->GetMetaDataObjectValue();
        series.pixelSpacing = QString::fromStdString(tagvalue);
        series.pixelSpacing.replace("\\", "*");
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvalueThickness)
    {
        std::string tagvalue = entryvalueThickness->GetMetaDataObjectValue();
        series.thickness = QString::fromStdString(tagvalue);
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }

    if (entryvalueSpaceBetween)
    {
        std::string tagvalue = entryvalueSpaceBetween->GetMetaDataObjectValue();
        series.spaceBetween = QString::fromStdString(tagvalue);
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        //return EXIT_FAILURE;
    }
   
    //Ŀǰȡ��һ���ϳ�����ͼ�������������Ҫ���ǲ�ͬ�˻���ͬһ���ļ��ж�����е��������ô���ֲ������⸲��
    //�����ļ��µ����У��������е�·��Ҳ�Ǹ��鷳����Ŀǰ�ݴ�һ��������itk��Ҳ��֧�ֿ��ļ��м��㲻ͬ�����У�����Ͽ��Խ����Ŀǰ�ݲ����ǣ�
    int mid = std::ceil(fileNames.size() / 2); //����ȡ�������ڻ���ڸ����������������
    std::string source = fileNames.at(mid); //ȡ�м�    
    if (!moveDir.isEmpty()) //�ж��Ƿ��ƶ����ݿ�
    {
        series.seriesDir = moveDir;
        createPath(series.seriesDir, Dir);
        // �ƶ�֮ǰ��������ͼ
        QString target = series.seriesDir + "/cover.png";
        QString screenshot = itkDicomSaveToPng(source, target.toStdString());
        series.screenshot = screenshot; //����ͼ
        //�ƶ��ļ�
        for (int i = 0; i < fileNames.size(); i++)
        {
            QString fileName = QString::fromStdString(fileNames.at(i));
            QFile file(fileName);
            QFileInfo info(fileName);
            bool flag = file.rename(series.seriesDir + "/" + info.fileName());
        }
    }
    else
    {
		QString tmpPath = QString::fromLocal8Bit(source.c_str());
        series.seriesDir = QFileInfo(tmpPath).absolutePath(); // 
        QString target = series.seriesDir + "/cover.png";
        QString screenshot = itkDicomSaveToPng(source, std::string(target.toLocal8Bit()));
        series.screenshot = screenshot; //����ͼ
    }
   
    //�������ݿ�
    GLDb db;
    if (db.createDbConnect())
    {
        db.setPatients(patient); //�����Ȳ���patient����֪��patientId���������Ӧ���ȵ���
        series.patientId = patient.patientId;
        db.setPatientsSeries(series);
    }

    return EXIT_SUCCESS;
}

bool Dicom::createPath(QString path, ::PathType type)
{
    if (type == ::PathType::File)
    {
        //�ж�·���Ƿ����
        QFile file(path);
        if (file.exists())
            return true;

        int first = path.lastIndexOf("/");

        //����Ŀ¼
        QString dirpath = path.mid(0, first);
        QDir dir(dirpath);
        if (!dir.exists())
            dir.mkdir(dirpath);

        //�����ļ�
        if (dir.exists())
        {
            QString filename = path.mid(first + 1);
            if (file.open(QIODevice::WriteOnly))
            {
                file.close();
                return true;
            }
        }
        return false;
    }

    if (type == ::PathType::Dir)
    {
        return QDir().mkpath(path);
    }

    return false;
}

// ���뵥��ͼƬ
int Dicom::dicomSequenceErrorAndSeriesMiss(std::string& fileName)
{
    using PixelType = signed short;
    constexpr unsigned int Dimension = 3;

    using ImageType = itk::Image<PixelType, Dimension>;
    using ReaderType = itk::ImageSeriesReader<ImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    using ImageIOType = itk::GDCMImageIO;

    ImageIOType::Pointer dicomIO = ImageIOType::New();

    reader->SetImageIO(dicomIO);
    using NamesGeneratorType = itk::GDCMSeriesFileNames;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    using FileNamesContainer = std::vector<std::string>;
    using FileNamesContainer = std::vector<std::string>;

    std::vector<std::string> fileNameVec{fileName};
    reader->SetFileNames(fileNameVec);

    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject & ex)
    {
        std::cerr << ex << std::endl;
        return -1;
    }

    using DictionaryType = itk::MetaDataDictionary;

    const DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();

    using MetaDataStringType = itk::MetaDataObject<std::string>;
    std::string entryIdSeriesSeq = "0020|0013"; //����˳��
    auto tagItrSeriesSeq = dictionary.Find(entryIdSeriesSeq);
    auto itr = dictionary.Begin();
    auto end = dictionary.End();
    MetaDataStringType::ConstPointer entryvalueSeriesSeq = nullptr;

    if (tagItrSeriesSeq == end)
    {
        std::cerr << "Tag " << entryIdSeriesSeq;
        std::cerr << " not found in the DICOM header" << std::endl;
        return -2;
    }
    else
    {
        entryvalueSeriesSeq = dynamic_cast<const MetaDataStringType *>(tagItrSeriesSeq->second.GetPointer());
    }
    if (entryvalueSeriesSeq)
    {
        std::string tagvalue = entryvalueSeriesSeq->GetMetaDataObjectValue();
        return std::stoi(tagvalue); //C++11��׼
    }
    else
    {
        std::cerr << "Entry was not of string type" << std::endl;
        return -3;
    }
}

int Dicom::gdcmReadDicom(std::string inputFilename)
{
	gdcm::ImageReader reader;
	reader.SetFileName(inputFilename.c_str());
	if (!reader.Read())
	{
		std::cerr << "Could not read: " << inputFilename << std::endl;
		return 1;
	}
	const gdcm::Image &image = reader.GetImage();
	unsigned int ndim = image.GetNumberOfDimensions();
	//Dimensions of the image
	const unsigned int *dims = image.GetDimensions();
	// Origin
	const double *origin = image.GetOrigin();
	const gdcm::PhotometricInterpretation &pi = image.GetPhotometricInterpretation();
	for (unsigned int i = 0; i < ndim; ++i)
	{
	}
	for (unsigned int i = 0; i < ndim; ++i)
	{
		gdcm::ImageWriter writer;
		writer.SetImage(image);
		//writer.SetFileName(inputFilename);
	}
	return 0;
}



void Dicom::dcmtkDicomSaveToPng(std::string source, std::string target)
{
    DicomImage *image = new DicomImage(source.c_str());
    if (image != NULL)
    {
        ;
    }
}

void Dicom::GetMaxAndMinOfDicom(const itk::MetaDataDictionary & dictionary, int & max, int & min)
{
	//If the window width and center has a value, it is calculated with the window width and center, otherwise it is calculated with largest and smallest
	bool isHaveLegalValue = false;
	auto end = dictionary.End();
	using MetaDataStringType = itk::MetaDataObject<std::string>;
	auto tmpWindowCenter = dictionary.Find("0028|1050");
	auto tmpWindowWidth = dictionary.Find("0028|1051");
	if (tmpWindowCenter != end && tmpWindowWidth != end) {
		auto entryvalueCenter = dynamic_cast<const MetaDataStringType *>(tmpWindowCenter->second.GetPointer());
		auto entryvalueWidth = dynamic_cast<const MetaDataStringType *>(tmpWindowWidth->second.GetPointer());
		if (entryvalueCenter && entryvalueWidth) {
			std::string tmpCenter = entryvalueCenter->GetMetaDataObjectValue();
			
			int center = atoi(tmpCenter.c_str());
			std::string tmpWidth = entryvalueWidth->GetMetaDataObjectValue();
			int width = atoi(tmpWidth.c_str());
			max = center + width / 2;
			min = center - width / 2;
			isHaveLegalValue = true;
		}
	}
	if (isHaveLegalValue) return;

	auto tmpSmallest = dictionary.Find("0028|0106");
	auto tmpLargest = dictionary.Find("0028|0107");
	if (tmpWindowCenter == end || tmpWindowWidth == end) return;
	auto entryvalueSmall = dynamic_cast<const MetaDataStringType *>(tmpSmallest->second.GetPointer());
	auto entryvalueLarge = dynamic_cast<const MetaDataStringType *>(tmpLargest->second.GetPointer());
	if (!entryvalueSmall || !entryvalueLarge) return;
	int smallest = 0;
	if (entryvalueSmall) {
		std::string smallest = entryvalueSmall->GetMetaDataObjectValue();
		min = atoi(smallest.c_str());
	}
	int largest = 0;
	if (entryvalueLarge) {
		std::string largest = entryvalueLarge->GetMetaDataObjectValue();
		max = atoi(largest.c_str());
	}
}

void Dicom::ShowMissWarning(QVector<struct Miss> missVec, QStringList badFileList)
{
	QStringList missList;
	for (int i = 0; i < missVec.size(); i++)
	{
		missVec.at(i).dir;
		QVector<int> miss = missVec.at(i).missVec;
		QStringList missList_;
		for (int j = 0; j < miss.size(); j++)
		{
			missList_.append(QString::number(miss.at(j)));
		}
		QString txt = QStringLiteral("Ŀ¼(%1)ȱʧ����%2").arg(missVec.at(i).dir).arg(missList_.join(","));
		missList.append(txt);
	}
	QString missTxt;
	if (missList.size() > 0)
		missTxt = QStringLiteral("����Ӱ���ļ�ȱʧ��<br>%1").arg(missList.join("; "));

	QStringList dcmList;
	for (int i = 0; i < badFileList.size(); i++)
	{
		dcmList.append(badFileList.takeAt(i));
		/*if (badFileList.at(i).endsWith(".dcm", Qt::CaseInsensitive))
		{
			dcmList.append(badFileList.takeAt(i));
		}*/
	}
	QString dcmTxt;
	if (dcmList.size() > 0)
		dcmTxt = QStringLiteral("����Ӱ���ļ�����<br>%1").arg(dcmList.join("<br>"));

	if (!missTxt.isEmpty() || !dcmTxt.isEmpty())
	{

		QString dicomBadTxt = QStringLiteral("%1%2")
			.arg(missTxt, dcmTxt);
	}
}

QString Dicom::itkDicomSaveToPng(std::string source, std::string target)
{
    typedef short InputPixelType; // png support pixel type like unsigned char and unsigned short RGB, RGBA, jpeg support pixel type like unsigned char RGB<unsigned char>
	const unsigned int InputDimension = 2;
	typedef itk::Image<InputPixelType, InputDimension> InputImageType;
	typedef itk::ImageFileReader<InputImageType> ReaderType;

	typedef unsigned char outputPixelType;
	typedef itk::Image<outputPixelType, InputDimension> outputImageType;

    ReaderType::Pointer reader = ReaderType::New();
    const char *infilename = source.c_str();
    reader->SetFileName(infilename);
    typedef itk::GDCMImageIO ImageIOType;
    ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
    reader->SetImageIO(gdcmImageIO);
    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject & ex)
    {
        std::cerr << ex.what() << std::endl;
        
        return QString();
    }
	int smallest = 0;
	int largest = 2048;
	const  itk::MetaDataDictionary & dictionary = gdcmImageIO->GetMetaDataDictionary();
	GetMaxAndMinOfDicom(dictionary, largest, smallest);

    typedef itk::PNGImageIO PNGIOType;
    PNGIOType::Pointer pngIO = PNGIOType::New();

	typedef itk::ImageFileWriter<outputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    const char * outfilename = target.c_str();
    writer->SetFileName(outfilename);
	auto input = reader->GetOutput();
	outputImageType::Pointer output = outputImageType::New();
	outputImageType::RegionType region = input->GetLargestPossibleRegion();
	output->SetRegions(region);
	output->Allocate();
	itk::ImageRegionConstIterator<InputImageType> inputIterator(input, input->GetLargestPossibleRegion());
	itk::ImageRegionIterator<outputImageType>      outputIterator(output, output->GetLargestPossibleRegion());
	while (!inputIterator.IsAtEnd())
	{
		float num = inputIterator.Get();
		num = (num - smallest) / (largest - smallest) * 255;
		if (num < 0) num = 0;
		if (num > 255) num = 255;
		
		unsigned char charNum = num;
		outputIterator.Set(num);
		++inputIterator;
		++outputIterator;
	}
	writer->SetInput(output);
    writer->SetImageIO(pngIO);
    try
    {
        writer->Update();
    }
    catch (itk::ExceptionObject & ex)
    {
        std::cerr << ex.what() << std::endl;
        return QString();
    }

    return QString::fromStdString(target);
}
