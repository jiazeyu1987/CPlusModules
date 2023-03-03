/*==============================================================================
 
  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/
#include "itkMetaDataObject.h"

// FilterLeft Logic includes
#include "vtkSlicerFilterLeftLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

#include "vtkMRMLColorNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkEventBroker.h"
#include "vtkMRMLStorageNode.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diutils.h"
#include "dcmtk/dcmjpeg/dipijpeg.h"
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
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerFilterLeftLogic);

//----------------------------------------------------------------------------
vtkSlicerFilterLeftLogic::vtkSlicerFilterLeftLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerFilterLeftLogic::~vtkSlicerFilterLeftLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkSlicerFilterLeftLogic::ParseHelpNodeInfo(vtkMRMLNode* node)
{
    std::string helpName = node->GetAttribute("Help_Name");
    if (helpName == "create_png") {
        GetMRMLScene()->RemoveNode(node);
        std::string filename = node->GetAttribute("Help_Info_0");
        std::string spath = itksys::SystemTools::GetFilenamePath(filename);
        std::string imgPath = spath + "/cover.png";
        itkDicomSaveToPng(filename, imgPath);
    }
    if (helpName == "Filter_Node_Click") {
        GetMRMLScene()->RemoveNode(node);
        std::string nodeID = node->GetAttribute("Help_Info_0");
        auto selectNode = GetMRMLScene()->GetNodeByID(nodeID);
        if (selectNode == NULL) return;
        vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(selectNode);
        if (volumeNode == NULL) return;
        scanType = "";
        layerNumber = "";
        AcquisitionDateTime = "";
        serialNumber = "";
        patientName = "";
        const itk::MetaDataDictionary& dict = volumeNode->GetMetaDataDictionary();

        const char* filename = "";
        filename = volumeNode->GetAttribute("filename");
        if(!filename){
        if(volumeNode&& volumeNode->GetStorageNode())
            filename = volumeNode->GetStorageNode()->GetFileName();
        }

        if (volumeNode->GetAttribute("scanType") != nullptr) {
            patientName = std::string(volumeNode->GetAttribute("patientName"));
            scanType = std::string(volumeNode->GetAttribute("scanType"));
            layerNumber = std::string(volumeNode->GetAttribute("layerNumber"));

            const char* sn = volumeNode->GetAttribute("serialNumber");
            if(sn){
                serialNumber = std::string(sn);
            }

            const char* adt = volumeNode->GetAttribute("AcquisitionDateTime");
            if (adt) {
                AcquisitionDateTime = std::string(adt);
            }
            this->Modified();
            return;
        }

        DcmFileFormat dcmFormat;


        DcmMetaInfo metainfo;
        OFCondition status;
        status = metainfo.loadFile(filename);
        if (status.good())
        {
            OFString xferUID;
            if (metainfo.findAndGetOFString(DCM_TransferSyntaxUID, xferUID).good())
                std::cout << "Transfer Syntax UID: " << xferUID << OFendl;
           
        }

        OFCondition ofResult = dcmFormat.loadFile(filename);
        if (!ofResult.good())
        {
            std::cout << "dcmFormat read fail..." << ofResult.text();
            this->Modified();
            return ;
        }

        DcmDataset* pDataSet = dcmFormat.getDataset();

        if (nullptr == pDataSet)
        {
            this->Modified();
            return ;
        }

        OFString o_PatientName,o_scanType,o_SerialNumber,o_layerNumber, o_AcquisitionDateTime;
        ofResult = pDataSet->findAndGetOFString(DCM_SeriesDescription, o_PatientName);
        if (ofResult.good())
        {
            patientName = o_PatientName.c_str();
            volumeNode->SetAttribute("patientName", patientName.c_str());
        }

        ofResult = pDataSet->findAndGetOFString(DCM_Modality, o_scanType);
        if (ofResult.good())
        {
            scanType = o_scanType.c_str();
            volumeNode->SetAttribute("scanType", scanType.c_str());
        }

        ofResult = pDataSet->findAndGetOFString(DCM_DeviceSerialNumber, o_SerialNumber);
        if (ofResult.good())
        {
            serialNumber = o_SerialNumber.c_str();
            volumeNode->SetAttribute("serialNumber", serialNumber.c_str());
        }

        ofResult = pDataSet->findAndGetOFString(DCM_Rows, o_layerNumber);
        if (ofResult.good())
        {
            layerNumber = o_layerNumber.c_str();
            volumeNode->SetAttribute("layerNumber", layerNumber.c_str());
        }

        ofResult = pDataSet->findAndGetOFString(DCM_AcquisitionDate, o_AcquisitionDateTime);
        if (ofResult.good())
        {
            AcquisitionDateTime = o_AcquisitionDateTime.c_str();
            volumeNode->SetAttribute("AcquisitionDateTime", AcquisitionDateTime.c_str());
        }


        
        this->Modified();
    }
    
}



std::string vtkSlicerFilterLeftLogic::itkDicomSaveToPng(std::string source, std::string target)
{
    typedef short InputPixelType; // png support pixel type like unsigned char and unsigned short RGB, RGBA, jpeg support pixel type like unsigned char RGB<unsigned char>
    const unsigned int InputDimension = 2;
    typedef itk::Image<InputPixelType, InputDimension> InputImageType;
    typedef itk::ImageFileReader<InputImageType> ReaderType;

    typedef unsigned char outputPixelType;
    typedef itk::Image<outputPixelType, InputDimension> outputImageType;

    ReaderType::Pointer reader = ReaderType::New();
    const char* infilename = source.c_str();
    reader->SetFileName(infilename);
    typedef itk::GDCMImageIO ImageIOType;
    ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
    reader->SetImageIO(gdcmImageIO);
    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject& ex)
    {
        std::cerr << ex.what() << std::endl;

        return "";
    }
    int smallest = 0;
    int largest = 2048;
    const  itk::MetaDataDictionary& dictionary = gdcmImageIO->GetMetaDataDictionary();
    GetMaxAndMinOfDicom(dictionary, largest, smallest);

    typedef itk::PNGImageIO PNGIOType;
    PNGIOType::Pointer pngIO = PNGIOType::New();

    typedef itk::ImageFileWriter<outputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    const char* outfilename = target.c_str();
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
    catch (itk::ExceptionObject& ex)
    {
        std::cerr << ex.what() << std::endl;
        return "";
    }

    return target;
}


void vtkSlicerFilterLeftLogic::GetMaxAndMinOfDicom(const itk::MetaDataDictionary& dictionary, int& max, int& min)
{
    //If the window width and center has a value, it is calculated with the window width and center, otherwise it is calculated with largest and smallest
    bool isHaveLegalValue = false;
    auto end = dictionary.End();
    using MetaDataStringType = itk::MetaDataObject<std::string>;
    auto tmpWindowCenter = dictionary.Find("0028|1050");
    auto tmpWindowWidth = dictionary.Find("0028|1051");
    if (tmpWindowCenter != end && tmpWindowWidth != end) {
        auto entryvalueCenter = dynamic_cast<const MetaDataStringType*>(tmpWindowCenter->second.GetPointer());
        auto entryvalueWidth = dynamic_cast<const MetaDataStringType*>(tmpWindowWidth->second.GetPointer());
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
    auto entryvalueSmall = dynamic_cast<const MetaDataStringType*>(tmpSmallest->second.GetPointer());
    auto entryvalueLarge = dynamic_cast<const MetaDataStringType*>(tmpLargest->second.GetPointer());
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

//---------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//----------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic::ProcessMRMLLogicsEvents(vtkObject* caller,
    unsigned long event,
    void* callData)
{
    if (caller->IsA("vtkSlicerApplicationLogic") &&
        event == vtkSlicerApplicationLogic::RequestProcessedEvent)
    {
		int i = 1;
    }
}


//----------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic
::SetMRMLApplicationLogic(vtkMRMLApplicationLogic* logic)
{
    vtkMRMLApplicationLogic* oldLogic = this->GetMRMLApplicationLogic();
    if (logic == oldLogic)
    {
        return;
    }

    bool wasModifying = this->StartModify();
    if (oldLogic)
    {
        vtkEventBroker::GetInstance()->RemoveObservations(
            oldLogic, vtkSlicerApplicationLogic::RequestProcessedEvent,
            this, this->GetMRMLLogicsCallbackCommand());
    }

    Superclass::SetMRMLApplicationLogic(logic);
    assert(logic == this->GetMRMLApplicationLogic());

    // Observe application logic to know when the CLI is completed and the
    // associated data loaded.
    if (logic)
    {
        vtkEventBroker::GetInstance()->AddObservation(
            logic, vtkSlicerApplicationLogic::RequestProcessedEvent,
            this, this->GetMRMLLogicsCallbackCommand());
    }
    this->EndModify(wasModifying);
}

//-----------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
    vtkMRMLColorNode* colorNode = vtkMRMLColorNode::SafeDownCast(node);
    if (colorNode) {
        const char* isHelpNode = node->GetAttribute("Help_Node");
        if (isHelpNode&&strcmp(isHelpNode,"true")==0) {
            ParseHelpNodeInfo(node);
            return;
        }
        return;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerFilterLeftLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
}
