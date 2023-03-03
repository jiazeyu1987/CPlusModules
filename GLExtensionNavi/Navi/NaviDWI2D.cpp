#include "NaviDWI2D.h"
#include "vtkMRMLColorTableNode.h"
#include <vtkMRMLDiffusionWeightedVolumeNode.h>

// VTK includes
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkVersion.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include "vtkTeemNRRDReader.h"
#include "vtkTeemNRRDWriter.h"
#include "itksys/SystemTools.hxx"

// MRML includes
#include <vtkMRMLNRRDStorageNode.h>

// vtkDMRI includes
#include <vtkTeemEstimateDiffusionTensor.h>
#include <vtkTensorMask.h>
#include "qSlicerApplication.h"
#include "qSlicerIOManager.h"
// ITK includes
#include <itkFloatingPointExceptions.h>


// vtkDMRI includes
#include "vtkDiffusionTensorMathematics.h"
#include "vtkSeedTracts.h"

// ITK includes
#include <itkConfigure.h>

// VTK includes
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageThreshold.h>
#include <vtkITKArchetypeDiffusionTensorImageReaderFile.h>
#include <vtkITKArchetypeImageSeriesScalarReader.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtksys/SystemTools.hxx>
#include <vtkXMLPolyDataWriter.h>
#include <vtkVersion.h>
#include "vtkSlicerFiberBundleLogic.h"
#include "vtkMRMLFiberBundleNode.h"
#include "vtkMRMLFiberBundleDisplayNode.h"

// STD includes
#include <string>
NaviDWI2D::NaviDWI2D(QWidget *parent)
	: NaviBase2D(parent)
{
	
    ui.setupUi(this);
	setWindowFlag(Qt::FramelessWindowHint); //隐藏标题框
	m_Data = new WidgetData();
	connect(ui.checkBox_2, SIGNAL(stateChanged(int)), this, SLOT(onRongjiChanged(int)));
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(onDTIBuild()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)),
        this, SLOT(OncbxChanged()));
	ui.VolumePropertyNodeWidget->setVisible(false); 
}


void NaviDWI2D::OncbxChanged() {
    if (ui.checkBox->isChecked())
    {
        SetTubeVisible(true);
    }
    else
    {
        SetTubeVisible(false);
    }
}

void NaviDWI2D::SetTubeVisible(bool b) {
    if (!m_Node) {
        return;
    }
    auto noded = m_Scene->GetFirstNodeByName(GetFiberBundleNodeName().toLatin1());
    vtkMRMLFiberBundleNode* fbNode = vtkMRMLFiberBundleNode::SafeDownCast(noded);
    if (!fbNode) {
        return;
    }
    vtkMRMLFiberBundleDisplayNode* displayNode = fbNode->GetTubeDisplayNode();
    vtkMRMLFiberBundleDisplayNode* displayNodeLine = fbNode->GetLineDisplayNode();
    vtkMRMLFiberBundleDisplayNode* displayNodeGly = fbNode->GetGlyphDisplayNode();
    if (b) {
        if (displayNode)
        {
           
        }
        else {
            fbNode->CreateDefaultDisplayNodes();
            displayNode = fbNode->GetTubeDisplayNode();
            
        }
        //displayNode->SetVisibility2D(true);
        //displayNode->SetVisibility3D(true);
        displayNode->SetVisibility(true);
        displayNodeLine->SetVisibility(true);
        //displayNodeGly->SetVisibility(true);

        displayNodeGly->RemoveAllViewNodeIDs();
        displayNodeLine->RemoveAllViewNodeIDs();
        displayNode->RemoveAllViewNodeIDs();
        displayNodeGly->AddViewNodeID("vtkMRMLViewNode3");
        displayNodeLine->AddViewNodeID("vtkMRMLViewNode3");
        displayNode->AddViewNodeID("vtkMRMLViewNode3");
       ui.DisplayNodeViewComboBox->setMRMLDisplayNode(displayNodeLine);
    }
    else {
        displayNode->SetVisibility(false);
        displayNodeLine->SetVisibility(false);
        displayNodeGly->SetVisibility(false);
    }
}

QString NaviDWI2D::GetDTIFilePath() {
    QString path = QCoreApplication::applicationDirPath();
    path.append("/~tmp/dwi_to_dti/");
    itksys::SystemTools::MakeDirectory(path.toLatin1());
    QString nrrd_path_1 = path + m_Node->GetName() + "_dti_output.nrrd";
    return nrrd_path_1;
}

QString NaviDWI2D::GetBaseLineFilePath() {
    QString path = QCoreApplication::applicationDirPath();
    path.append("/~tmp/dwi_to_dti/");
    itksys::SystemTools::MakeDirectory(path.toLatin1());
    QString nrrd_path_2 = path + m_Node->GetName() + "_dti_baseline.nrrd";
    return nrrd_path_2;
}

QString NaviDWI2D::GetFiberBundleFilePath() {
    QString path = QCoreApplication::applicationDirPath();
    path.append("/~tmp/dwi_to_dti/");
    itksys::SystemTools::MakeDirectory(path.toLatin1());
    QString nrrd_path_3 = path + m_Node->GetName() + "_dti_fiber.vtp";
    return nrrd_path_3.toLatin1();
}

QString NaviDWI2D::GetFiberBundleNodeName() {
    QString path = "";
    QString nrrd_path_3 = path + m_Node->GetName() + "_dti_fiber";
    return nrrd_path_3.toLatin1();
}

void NaviDWI2D::DoCreateDTINrrd() {
    itk::FloatingPointExceptions::Disable();
    bool applyMask = false;

    {
        vtkNew<vtkTeemNRRDReader> reader;
        //reader->SetFileName("E:/MK/patient_datas/dwi.nhdr");
        reader->Update();
        if (reader->GetReadStatus())
        {
            return;
        }

        vtkNew<vtkDoubleArray> bValues;
        vtkNew<vtkDoubleArray> grads;
        vtkNew<vtkMRMLNRRDStorageNode> helper;

        if (!helper->ParseDiffusionInformation(reader.GetPointer(), grads.GetPointer(), bValues.GetPointer()))
        {
            return;
        }
        vtkNew<vtkTeemEstimateDiffusionTensor> estim;

        estim->SetInputConnection(reader->GetOutputPort());
        estim->SetNumberOfGradients(grads->GetNumberOfTuples());
        estim->SetDiffusionGradients(grads.GetPointer());
        estim->SetBValues(bValues.GetPointer());
        estim->SetShiftNegativeEigenvalues(0);

        // Compute Transformation that brings the gradients to ijk
        // double *sp = reader->GetOutput()->GetSpacing();
        vtkNew<vtkMatrix4x4> mf;
        mf->DeepCopy(reader->GetMeasurementFrameMatrix());
        vtkNew<vtkMatrix4x4> rasToIjkRotation;
        rasToIjkRotation->DeepCopy(reader->GetRasToIjkMatrix());
        // Set Translation to zero
        for (int i = 0; i < 3; i++)
        {
            rasToIjkRotation->SetElement(i, 3, 0);
        }
        // Remove scaling in rasToIjk to make a real rotation matrix
        double col[3];
        for (int jjj = 0; jjj < 3; jjj++)
        {
            for (int iii = 0; iii < 3; iii++)
            {
                col[iii] = rasToIjkRotation->GetElement(iii, jjj);
            }
            vtkMath::Normalize(col);
            for (int iii = 0; iii < 3; iii++)
            {
                rasToIjkRotation->SetElement(iii, jjj, col[iii]);
            }
        }

        vtkNew<vtkTransform> trans;
        trans->PostMultiply();
        trans->SetMatrix(mf.GetPointer());
        trans->Concatenate(rasToIjkRotation.GetPointer());
        trans->Update();

        estim->SetTransform(trans.GetPointer());
        if (true)
        {
            estim->SetEstimationMethodToLLS();
        }
        else
        {
            estim->SetEstimationMethodToWLS();
        }
        estim->Update();
        vtkImageData* tensorImage = estim->GetOutput();
        // Read the tensor mask
        vtkNew<vtkImageData> mask;
        if (false)
        {
            vtkNew<vtkTeemNRRDReader> maskReader;
            maskReader->SetFileName("");
            maskReader->Update();
            if (maskReader->GetReadStatus())
            {
                return;
            }

            //// Check if the transforms are equal
            //if (transformsNotEqual(maskReader->GetRasToIjkMatrix(), reader->GetRasToIjkMatrix()))
            //{
            //    std::cerr << argv[0] << ": Error reading Mask file, wrong coordinate space" << std::endl;
            //    return EXIT_FAILURE;
            //}

            vtkNew<vtkImageCast> cast;
            cast->SetInputConnection(maskReader->GetOutputPort());
            cast->SetOutputScalarTypeToUnsignedChar();
            cast->Update();

            mask->DeepCopy(cast->GetOutput());
            applyMask = true;
        }
        else
        {
            applyMask = false;
        }

        // Mask tensor
        vtkNew<vtkTensorMask> tensorMask;
        tensorMask->SetNumberOfThreads(1);
        if (applyMask)
        {
            tensorMask->SetMaskAlpha(0.0);
            tensorMask->SetInputConnection(estim->GetOutputPort());
            tensorMask->SetMaskInputData(mask.GetPointer());
            tensorMask->Update();
            tensorImage = tensorMask->GetOutput();
        }
        /**/
        // Compute IjkToRas (used by Writer)
        vtkMatrix4x4* ijkToRasMatrix = reader->GetRasToIjkMatrix();
        ijkToRasMatrix->Invert();



        // Don't save the scalars array, only the tensor array.
        // Save tensor
        vtkNew<vtkTeemNRRDWriter> writer;
        tensorImage->GetPointData()->SetScalars(NULL);
        writer->SetInputData(tensorImage);
        writer->SetFileName(GetDTIFilePath().toLatin1());
        writer->UseCompressionOn();
        writer->SetIJKToRASMatrix(ijkToRasMatrix);
        // Compute measurement frame: Take into account that we have transformed
        // the gradients so tensor components are defined in ijk.
        rasToIjkRotation->Invert();
        writer->SetMeasurementFrameMatrix(rasToIjkRotation.GetPointer());
        writer->Write();

        // Save baseline
        vtkNew<vtkTeemNRRDWriter> writer2;
        writer2->SetInputData(estim->GetBaseline());
        writer2->SetFileName(GetBaseLineFilePath().toLatin1());
        writer2->UseCompressionOn();
        writer2->SetIJKToRASMatrix(ijkToRasMatrix);
        writer2->Write();

        auto iomanager = qSlicerApplication::application()->ioManager();
        iomanager->loadFile(GetDTIFilePath().toLatin1());
        iomanager->loadFile(GetBaseLineFilePath().toLatin1());
        QString name1 = QString(m_Node->GetName()) + "_dti_output.nrrd";
        QString name2 = QString(m_Node->GetName()) + "_dti_baseline.nrrd";
        node_dti_output = vtkMRMLDiffusionWeightedVolumeNode::SafeDownCast(m_Scene->GetFirstNodeByName(name1.toLatin1()));
        node_dti_baseline = vtkMRMLScalarVolumeNode::SafeDownCast(m_Scene->GetFirstNodeByName(name2.toLatin1()));
        
    }
}

void NaviDWI2D::onDTIBuild() {
    if (itksys::SystemTools::FileExists(GetFiberBundleFilePath().toLatin1())) {
        DoLoadDTIFiberBundle();
        return;
    }
    DoCreateDTINrrd();
    DoBuidDTI();
    DoLoadDTIFiberBundle();

}

void NaviDWI2D::DoBuidDTI() {
    try
    {
        vtkNew<vtkITKArchetypeDiffusionTensorImageReaderFile> reader;
        reader->SetArchetype(GetDTIFilePath().toLatin1());
        reader->SetSingleFile(1);
        reader->SetUseOrientationFromFile(1);
        reader->SetUseNativeOriginOn();
        reader->SetOutputScalarTypeToNative();
        reader->SetDesiredCoordinateOrientationToNative();
        reader->Update();

        vtkSmartPointer<vtkAlgorithmOutput> ROIConnection;
        vtkNew<vtkITKArchetypeImageSeriesScalarReader> reader2;
        vtkNew<vtkImageCast> imageCast;
        vtkNew<vtkDiffusionTensorMathematics> math;
        vtkNew<vtkImageThreshold> th;
        vtkNew<vtkMatrix4x4> ROIRASToIJK;

        if (false)
        {
            //reader2->SetArchetype(InputROI.c_str());
            //reader2->SetUseOrientationFromFile(1);
            //reader2->SetUseNativeOriginOn();
            //reader2->SetOutputScalarTypeToNative();
            //reader2->SetDesiredCoordinateOrientationToNative();
            //reader2->SetSingleFile(1);
            //reader2->Update();

            //if (reader2->GetOutput()->GetPointData()->GetScalars() == NULL)
            //{
            //    std::cerr << argv[0] << ": No roi data" << std::endl;
            //    return EXIT_FAILURE;
            //}

            //// cast roi to short data type
            //imageCast->SetOutputScalarTypeToShort();
            //imageCast->SetInputConnection(reader2->GetOutputPort());
            //ROIConnection = imageCast->GetOutputPort();

            //// Set up the matrix that will take points in ROI
            //// to RAS space.  Code assumes this is world space
            //// since  we have no access to external transforms.
            //// This will only work if no transform is applied to
            //// ROI and tensor volumes.
            ////
            //ROIRASToIJK->DeepCopy(reader2->GetRasToIjkMatrix());
        }
        else { // If the mask does not exist, create one
            math->SetInputConnection(reader->GetOutputPort());

            math->SetOperationToFractionalAnisotropy();

            th->SetInputConnection(math->GetOutputPort());
            th->ThresholdBetween(0.3, 1);
            th->SetInValue(1);
            th->SetOutValue(0);
            th->ReplaceInOn();
            th->ReplaceOutOn();
            th->SetOutputScalarTypeToShort();
            ROIConnection = th->GetOutputPort();

            // Set up the matrix that will take points in ROI
            // to RAS space.  Code assumes this is world space
            // since  we have no access to external transforms.
            // This will only work if no transform is applied to
            // ROI and tensor volumes.
            ROIRASToIJK->DeepCopy(reader->GetRasToIjkMatrix());
        }

        vtkNew<vtkSeedTracts> seed;

        // 1. Set Input
        seed->SetInputTensorFieldConnection(reader->GetOutputPort());

        // 2. Set Up matrices
        vtkNew<vtkMatrix4x4> TensorRASToIJK;
        TensorRASToIJK->DeepCopy(reader->GetRasToIjkMatrix());

        // VTK seeding is in ijk space with voxel scale included.
        // Calculate the matrix that goes from tensor "scaled IJK",
        // the array with voxels that know their size (what vtk sees for tract seeding)
        // to our RAS.
        double sp[3];
        reader->GetOutput()->GetSpacing(sp);
        vtkNew<vtkTransform> trans;
        trans->Identity();
        trans->PreMultiply();
        trans->SetMatrix(TensorRASToIJK.GetPointer());
        // Trans from IJK to RAS
        trans->Inverse();
        // Take into account spacing (remove from matrix) to compute Scaled IJK to RAS matrix
        trans->Scale(1 / sp[0], 1 / sp[1], 1 / sp[2]);
        trans->Inverse();

        // Set Transformation to seeding class
        seed->SetWorldToTensorScaledIJK(trans.GetPointer());

        // Rotation part of matrix is only thing tensor is transformed by.
        // This is to transform output tensors into RAS space.
        // Tensors are output along the fibers.
        // This matrix is not used for calculating tractography.
        // The following should be replaced with finite strain method
        // rather than assuming rotation part of the matrix according to
        // slicer convention.
        vtkNew<vtkMatrix4x4> TensorRASToIJKRotation;
        TensorRASToIJKRotation->DeepCopy(TensorRASToIJK.GetPointer());
        // Set Translation to zero
        for (int i = 0; i < 3; i++)
        {
            TensorRASToIJKRotation->SetElement(i, 3, 0);
        }
        // Remove scaling in rasToIjk to make a real rotation matrix
        double col[3];
        for (int jjj = 0; jjj < 3; jjj++)
        {
            for (int iii = 0; iii < 3; iii++)
            {
                col[iii] = TensorRASToIJKRotation->GetElement(iii, jjj);
            }
            vtkMath::Normalize(col);
            for (int iii = 0; iii < 3; iii++)
            {
                TensorRASToIJKRotation->SetElement(iii, jjj, col[iii]);
            }
        }
        TensorRASToIJKRotation->Invert();
        seed->SetTensorRotationMatrix(TensorRASToIJKRotation.GetPointer());

        // 3. Set up ROI (not based on Cl mask), from input now

        vtkNew<vtkTransform> trans2;
        trans2->Identity();
        trans2->PreMultiply();

        // no longer assume this ROI is in tensor space
        // trans2->SetMatrix(TensorRASToIJK.GetPointer());
        trans2->SetMatrix(ROIRASToIJK.GetPointer());
        trans2->Inverse();
        seed->SetROIToWorld(trans2.GetPointer());

        // PENDING: Do merging with input ROI

        seed->SetInputROIConnection(ROIConnection);
        seed->SetInputROIValue(1);
        seed->UseStartingThresholdOn();
        seed->SetStartingThreshold(0.3);


        // 4. Set Tractography specific parameters

        if (false)
        {
            seed->SetIsotropicSeeding(0);
        }
        else
        {
            seed->SetIsotropicSeeding(1);
        }

        seed->SetIsotropicSeedingResolution(2);
        seed->SetMinimumPathLength(20);
        seed->UseVtkHyperStreamlinePoints();
        vtkNew<vtkHyperStreamlineDTMRI> streamer;
        seed->SetVtkHyperStreamlinePointsSettings(streamer.GetPointer());

        streamer->SetThresholdModeToFractionalAnisotropy();

        streamer->SetStoppingThreshold(0.25);
        streamer->SetMaximumPropagationDistance(800);
        streamer->SetRadiusOfCurvature(0.7);
        streamer->SetIntegrationStepLength(0.5);

        // 5. Run the thing
        seed->SeedStreamlinesInROI();

        // Save result

        // 6. Extract PolyData in RAS
        vtkNew<vtkPolyData> outFibers;
        seed->TransformStreamlinesToRASAndAppendToPolyData(outFibers.GetPointer());

        std::string fileExtension = vtksys::SystemTools::LowerCase(vtksys::SystemTools::GetFilenameLastExtension(GetFiberBundleFilePath().toStdString()));
        if (fileExtension == ".vtk")
        {
            vtkNew<vtkPolyDataWriter> writer;
            writer->SetFileName(GetFiberBundleFilePath().toLatin1());
            writer->SetFileTypeToBinary();
            writer->SetInputData(outFibers.GetPointer());
            writer->Write();
        }
        else
        {
            if (fileExtension != ".vtp")
            {
                cerr << "Extension not recognize, saving the information in VTP format" << endl;
            }
            vtkNew<vtkXMLPolyDataWriter> writer;
            writer->SetFileName(GetFiberBundleFilePath().toLatin1());
            writer->SetInputData(outFibers.GetPointer());
            writer->Write();
        }
        
    }
    catch (...)
    {
        
        return ;
    }
}

void NaviDWI2D::DoLoadDTIFiberBundle() {
    vtkSlicerFiberBundleLogic* LC = vtkSlicerFiberBundleLogic::New();
    LC->SetMRMLScene(m_Scene);
    LC->AddFiberBundle(GetFiberBundleFilePath().toLatin1(), GetFiberBundleNodeName().toLatin1());
    ui.checkBox->setChecked(true);
    ui.pushButton->setVisible(false);
}

// --------------------------------------------------------------------------
void NaviDWI2D::setColorNode(vtkMRMLNode* colorNode)
{
	vtkMRMLVolumeNode* volumeNode = m_Node;
	vtkMRMLScalarVolumeDisplayNode* displayNode = volumeNode ? vtkMRMLScalarVolumeDisplayNode::SafeDownCast(
		volumeNode->GetDisplayNode()) : nullptr;

	if (!displayNode || !colorNode)
	{
		return;
	}
	Q_ASSERT(vtkMRMLColorNode::SafeDownCast(colorNode));
	displayNode->SetAndObserveColorNodeID(colorNode->GetID());
}

void NaviDWI2D::onRongjiChanged(int state) {
	if (state == Qt::Checked) // "选中"
	{
		m_Rongji = 1;
	}
	else {
		m_Rongji = 0;
	}
	emit(onRongjiChanged());
}


void NaviDWI2D::OnOpacityValueChanged(int value) {
	m_Data->m_opacity = value;
	emit(onOpacityChanged(value));
}

void NaviDWI2D::onVisibleChanged(int state) {
	if (state == Qt::Checked) // "选中"
	{
		m_Node->SetDisplayVisibility(1);
		m_Node->SetDisplayVisibility(1);
	}
	else {
		m_Node->SetDisplayVisibility(0);
		m_Node->SetDisplayVisibility(0);
	}
}

vtkMRMLScalarVolumeDisplayNode* NaviDWI2D::GetDisplayNode() {
	auto displayNode = m_Node->GetDisplayNode();
	auto scalarDisplayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(displayNode);
	
	return scalarDisplayNode;
}

void NaviDWI2D::onOpenColorPad()
{
	m_Data->m_pColorDialog = new QColorDialog;
	connect(m_Data->m_pColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(onGetColor(QColor)));
	m_Data->m_pColorDialog->exec();
}

void NaviDWI2D::onGetColor(QColor color)
{
	m_Data->m_red = color.red();
	m_Data->m_green = color.green();
	m_Data->m_blue = color.blue();
	freshColor();
}

void NaviDWI2D::freshColor() {
	QString colorStr = QString("background-color:rgb(%1,%2,%3);").arg(m_Data->m_red).arg(m_Data->m_green).arg(m_Data->m_blue);
	double Color[4];
	Color[0] = m_Data->m_red / 255.0;
	Color[1] = m_Data->m_green / 255.0;
	Color[2] = m_Data->m_blue / 255.0;
	Color[3] = 0.5;
	vtkMRMLColorNode* colorNode = GetDisplayNode()->GetColorNode();
	vtkMRMLColorTableNode* tableColorNode = vtkMRMLColorTableNode::SafeDownCast(colorNode);
	tableColorNode->SetSimpleColorHue(Color[0]*0.33+ Color[1]*0.33+ Color[2]*0.33);
}

void NaviDWI2D::SetNode(vtkMRMLScene* scene,vtkMRMLScene* m_PresetScene, vtkMRMLScalarVolumeNode* in_node, vtkMRMLVolumeRenderingDisplayNode* displayNode) {
	m_Node = in_node;
    m_Scene = scene;
	char* name = m_Node->GetName();
	ui.lblName->setText(QString::fromStdString(std::string(name)));
	vtkMRMLAnnotationROINode* annotationROINode = (displayNode ? displayNode->GetROINode() : nullptr);
	m_VolumeRenderingDisplayNode = displayNode;
    ui.DisplayNodeViewComboBox = new qMRMLDisplayNodeViewComboBox(nullptr);
    ui.DisplayNodeViewComboBox->setMRMLScene(scene);
}

void NaviDWI2D::onCropToggled(bool toggle)
{
	vtkMRMLVolumeRenderingDisplayNode* displayNode = m_VolumeRenderingDisplayNode;
	if (!displayNode)
	{
		return;
	}
	// When the display box is visible, it should probably activate the
	// cropping (to follow the "what you see is what you get" pattern).
	if (toggle)
	{
		displayNode->SetCroppingEnabled(toggle);
	}

	vtkMRMLDisplayableNode* roiNode = m_VolumeRenderingDisplayNode->GetROINode();

	int numberOfDisplayNodes = roiNode->GetNumberOfDisplayNodes();

	std::vector<int> wasModifying(numberOfDisplayNodes);

	for (int index = 0; index < numberOfDisplayNodes; index++)
	{
		wasModifying[index] = roiNode->GetNthDisplayNode(index)->StartModify();
	}

	roiNode->SetDisplayVisibility(toggle);

	for (int index = 0; index < numberOfDisplayNodes; index++)
	{
		roiNode->GetNthDisplayNode(index)->EndModify(wasModifying[index]);
	}
}