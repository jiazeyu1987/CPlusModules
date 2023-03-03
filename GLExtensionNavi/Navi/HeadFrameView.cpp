#include "HeadFrameView.h"


#include "itkImage.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkFlatStructuringElement.h"

#include <random>
#include <algorithm>
#include <iterator>
#include <vtkLineSource.h>
#include <vtkPolyLine.h>
#include <itkBinaryThinningImageFilter3D.h>
#include "qlistview.h"
#include "itkVector.h"
template <typename TImage> void
DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output)
{
	output->SetRegions(input->GetLargestPossibleRegion());
	output->Allocate();

	itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
	itk::ImageRegionIterator<TImage>      outputIterator(output, output->GetLargestPossibleRegion());

	while (!inputIterator.IsAtEnd())
	{
		outputIterator.Set(inputIterator.Get());
		++inputIterator;
		++outputIterator;
	}
}

//DeepCopyPart<OutputImageType, BinaryImageType>(parts, segThin, labelValue);
template <typename TImage, typename OutImageType, typename LabelPixelType>
void DeepCopyPart(typename TImage::Pointer input, typename OutImageType::Pointer output, LabelPixelType labelValue)
{
	output->SetRegions(input->GetLargestPossibleRegion());
	output->Allocate();

	itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
	itk::ImageRegionIterator<OutImageType>      outputIterator(output, output->GetLargestPossibleRegion());

	while (!inputIterator.IsAtEnd())
	{
		outputIterator.Set(inputIterator.Get() == labelValue);
		++inputIterator;
		++outputIterator;
	}
}

double* convertIndexToPoint(itk::Image<short, 3>::Pointer volume, double* cvPoint)
{
	itk::Image<short, 3>::IndexType index = { int(cvPoint[0]), int(cvPoint[1]), int(cvPoint[2]) };
	itk::Image<short, 3>::PointType point;
	volume->TransformIndexToPhysicalPoint(index, point);
	//somehow, the origin has to be removed for the STL to show up correctly
	//not sure about coords transform for target and entry point, double check on that 
	//Point3d pt = { point[0] - volume->GetOrigin()[0], point[1] - volume->GetOrigin()[1], point[2] - volume->GetOrigin()[2] };
	double pt[3];
	pt[0] = point[0];
	pt[1] = point[1];
	pt[2] = point[2];
	return pt;
}

//Point3d convertIndexToPoint(itk::Image<short, 3>::Pointer volume, Point3d cvPoint, const mitk::MAPRegistrationWrapper* wrapper)
//{
//	itk::Image<short, 3>::IndexType index = { int(cvPoint.x), int(cvPoint.y), int(cvPoint.z) };
//	itk::Image<short, 3>::PointType point;
//	volume->TransformIndexToPhysicalPoint(index, point);
//	itk::Image<short, 3>::PointType outPoint = point;
//	if (wrapper)
//	{
//		wrapper->MapPoint(point, outPoint);
//	}
//	//somehow, the origin has to be removed for the STL to show up correctly
//	//not sure about coords transform for target and entry point, double check on that 
//	//Point3d pt = { point[0] - volume->GetOrigin()[0], point[1] - volume->GetOrigin()[1], point[2] - volume->GetOrigin()[2] };
//	Point3d pt = { outPoint[0] - volume->GetOrigin()[0], outPoint[1] - volume->GetOrigin()[0], outPoint[2] - volume->GetOrigin()[0] };
//
//	return pt;
//}
//
//Point3d convertIndexToPoint(itk::Image<short, 3>::Pointer volume, itk::Image<short, 3>::Pointer refVolume, Point3d cvPoint, const mitk::MAPRegistrationWrapper* wrapper)
//{
//	itk::Image<short, 3>::IndexType index = { int(cvPoint.x), int(cvPoint.y), int(cvPoint.z) };
//	itk::Image<short, 3>::PointType point;
//	volume->TransformIndexToPhysicalPoint(index, point);
//	itk::Image<short, 3>::PointType outPoint = point;
//	if (wrapper)
//	{
//		wrapper->MapPoint(point, outPoint);
//	}
//	//somehow, the origin has to be removed for the STL to show up correctly
//	//not sure about coords transform for target and entry point, double check on that 
//	Point3d pt = { outPoint[0], outPoint[1], outPoint[2] };
//	//Point3d pt = { outPoint[0] - refVolume->GetOrigin()[0], outPoint[1] - refVolume->GetOrigin()[0], outPoint[2] - refVolume->GetOrigin()[0] };
//
//	return pt;
//}

void lineRansac(std::vector<Point3i*>& points, Point3d &normal, Point3d &startPoint, std::vector<Point3i*> &pointsFit)
{
	//using ransac to fit a line: the MSE or robust MSE not suitable for multiple lines detection
	float distTH = 1.5;
	int maxIterations = 1000;
	//srand(time(NULL));
	//float random = ((float)rand() / (float)RAND_MAX + 1);
	int N = points.size();
	std::uniform_int_distribution<> dis(0, N - 1);
	std::vector<int> best_inliers;

	for (int it = 0; it < maxIterations; it++)
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		int id1 = dis(gen);
		int id2 = id1;
		while (id2 == id1) {
			id2 = dis(gen);
		}
		//read pyransac3d for reference
		Point3i* point1 = points[id1];
		Point3i* point2 = points[id2];
		//Point3d vecA = { double(point1->x - point2->x), double(point1->y - point2->y), double(point1->z - point2->z) };
		//Point3d vecA_norm = cv::normalize((cv::Vec3d)vecA);
		HeadFrameView::VectorType vecA;
		vecA[0] = double(point1->x - point2->x);
		vecA[1] = double(point1->y - point2->y);
		vecA[2] = double(point1->z - point2->z);
		HeadFrameView::VectorType vecA_norm = vecA.Normalize();

		//Distance from a point to a line
		std::vector<int>	pt_id_inliers;
		std::vector<int>	pt_id_outliers;

		for (int i = 0; i < N; i++)
		{
			//Point3d vecB = { double(point1->x - points[i]->x), double(point1->y - points[i]->y), double(point1->z - points[i]->z) };
			HeadFrameView::VectorType vecB;
			vecB[0] = double(point1->x - points[i]->x);
			vecB[1] = double(point1->y - points[i]->y);
			vecB[2] = double(point1->z - points[i]->z);
			HeadFrameView::VectorType vecC = itk::CrossProduct(vecA_norm, vecB);
			float distance = vecC.GetNorm();
			//float distance = cv::norm((cv::Vec3d)vecA_norm.cross(vecB));
			//Select indexes where distance is smaller than the threshold
			if (distance < distTH)
				pt_id_inliers.push_back(i);
		}
		if (pt_id_inliers.size() > best_inliers.size())
		{
			best_inliers = pt_id_inliers;
			normal.x = vecA_norm[0];
			normal.y = vecA_norm[1];
			normal.z = vecA_norm[2];
			startPoint.x = (double)(point1->x);
			startPoint.y = (double)(point1->y);
			startPoint.z = (double)(point1->z);
		}
	}
	//for the best line params, retrieve fitted points, which is the best_inliers
	//remove those points online, and fit another one
	//optimize later using a pointer
	//Since the ids in idsToRemove are known to be in items and in the same order, you can use a couple of iterators into items to 
	//keep track of the current compare element, the current destination, and walk thru idsToRemove and items, comparing both elements, 
	//moving the ones that you want to keep.At the end of that process, resize items to be the new smaller size.

	std::vector<Point3i*> pointsRemained;
	std::vector<int>::iterator it = best_inliers.begin();
	std::vector<Point3i*>::iterator iter = points.begin();
	for (int i = 0; iter != points.end(), it != best_inliers.end(); iter++, i++)
	{
		//check current index with current best_inliers, if it is , add to pointsFit, otherwise add to pointsRemained
		if (i == *it)
		{
			pointsFit.push_back(*iter);
			it++;		//best_inliers move ahead
		}
		else
		{
			pointsRemained.push_back(*iter);
		}
	}

	//std::vector<Point3i> pointsRemained;
	//std::vector<Point3i>::iterator iter = points.begin();
	//for (int i = 0; iter != points.end(); iter++, i++)
	//{
	//	if (std::find(best_inliers.begin(), best_inliers.end(), i) == best_inliers.end())
	//		pointsRemained.push_back(*iter);
	//}
	points = pointsRemained;
}


//HeadFrameView::HeadFrameView(QWidget *parent) : QWidget(parent)
//{
//	ui.setupUi(this);
//	dataStorage = NULL;
//	isContainCT = false;
//	ui.cmbSeries->setView(new QListView());
//	ui.cmbType->setView(new QListView());
//	dataMgr = NaviDataMgr::GetInstance();
//	connect(ui.btnClose, &QPushButton::clicked, this, [=]() {
//		if(isContainCT)	GLCommonTool::AddSignal(dataStorage, "restoreAllWidget");
//		GLCommonTool::AddSignal(dataStorage, "CloseBottomView");
//	});
//	connect(ui.btnIdentify, &QPushButton::clicked, this, &HeadFrameView::OnIdentifyHeadFramePositionNew);
//	connect(ui.cmbSeries, SIGNAL(currentIndexChanged(int)), this, SLOT(ShowSelectSeries(int)));
//}
//
//HeadFrameView::~HeadFrameView()
//{
//}
//
//void HeadFrameView::SetDataStorage(mitk::DataStorage * storage)
//{
//	dataStorage = storage;
//}
//
//void HeadFrameView::InitSeriesList()
//{
//	selectNode = NULL;
//	ui.cmbSeries->clear();
//	nodeList.clear();
//	displayNodeList.clear();
//	auto predicate = mitk::NodePredicateProperty::New("alias_name");
//	auto dataSet = dataStorage->GetSubset(predicate);
//	bool isFirstNode = true;
//	for (auto iter = dataSet->begin(); iter != dataSet->end(); ++iter) {
//		auto node = (*iter);
//		if (!dataMgr->IsCTModality(node)) continue;
//		std::string showName;
//		node->GetStringProperty("alias_name", showName);
//		QString tmp = QString::fromStdString(showName);
//		if (tmp == "basic") {
//			tmp = "Basic";
//			displayNodeList.append(node);
//		}
//		else {
//			//Only those who have registration data can do headstock identification
//			auto nodes = dataStorage->GetSubset(mitk::NodePredicateProperty::New("wrapper_name", mitk::StringProperty::New(showName)));
//			if (nodes->size() <= 0) continue;
//			nodes = dataStorage->GetSubset(mitk::NodePredicateProperty::New("register_name", mitk::StringProperty::New(showName)));
//			if (nodes->size() <= 0) continue;
//			displayNodeList.append(nodes->at(0));
//		}
//		nodeList.append(node);
//		ui.cmbSeries->addItem(tmp);
//		ui.btnIdentify->setEnabled(true);
//	}
//	isContainCT = nodeList.size() != 0;
//	if (nodeList.size() <= 0) {
//		GLMessageDialog::ShowInformation(QStringLiteral("头架识别"), QStringLiteral("没有可供头架识别的数据!"));
//		return;
//	}
//}
//
//void HeadFrameView::ShowSelectSeries(int num)
//{
//	if (num == -1) return;
//	//HideAllNodes();
//	MITK_CUSTOM << "头架识别-选择序列" << ui.cmbSeries->currentText().toStdString();
//
//	selectNode = nodeList.at(num);
//	displayNodeList.at(num)->SetVisibility(true);
//	mitk::RenderingManager::GetInstance()->RequestUpdateAll(); 
//}

//void HeadFrameView::SaveHeadFrameInfo(Point3d point, std::string saveKey)
//{
//	QString frameInfo = QString("%1,%2,%3").arg(point.x).arg(point.y).arg(point.z);
//	GLCommonTool::SetGlobalSaveValue(dataStorage, saveKey, frameInfo.toStdString());
//}

HeadFrameView::VectorType HeadFrameView::GetVector(Point3d p, HeadFrameView::VectorType& vecDiff) {
	vecDiff[0] = p.x;
	vecDiff[1] = p.y;
	vecDiff[2] = p.z;
	return vecDiff;
}

double HeadFrameView::GetDot(Point3d p1, Point3d p2) {
	return abs(p1.x* p2.x + p1.y * p2.y + p1.z * p2.z);
}

double HeadFrameView::GetDot(Point3i p1, Point3d p2) {
	return abs(p1.x * p2.x + p1.y * p2.y + p1.z * p2.z);
}

double HeadFrameView::GetDistance(Point3d p1, Point3d p2) {
	HeadFrameView::VectorType vec1;
	vec1[0] = p1.x;
	vec1[1] = p1.y;
	vec1[2] = p1.z;

	HeadFrameView::VectorType vec2;
	vec2[0] = p2.x;
	vec2[1] = p2.y;
	vec2[2] = p2.z;

	HeadFrameView::VectorType vec3 = itk::CrossProduct(vec1, vec2);
	return vec3.GetNorm();
}



void HeadFrameView::OnIdentifyHeadFramePositionNew()
{
	constexpr unsigned int Dimension = 3;
	using InputImageType = itk::Image<short, Dimension>;
	using BinaryImageType = itk::Image<unsigned char, Dimension>;	//float
	using OutputImageType = itk::Image<unsigned short, Dimension>;	//float
	InputImageType::Pointer volume = InputImageType::New();
	
	short lowerThreshold = 350;
	short upperThreshold = 2000;
	//fixed length 120mm, N shape, number of pixels(2 + sqrt(2)) * 120mm*(1 / resolution3)
	unsigned short regionLoThreshold = 2200; //around 6000 - 7000
	unsigned short regionHiThreshold = 20000;	//20000 originally

	// first step, do otsu thesholding to extract foreground(including head and frame) from background
	// seg = sitk.BinaryThreshold(img, lowerThreshold = 30, upperThreshold = 300, insideValue = 1, outsideValue = 0)
	unsigned char insideValue = 255;
	unsigned char outsideValue = 0;
	using FilterType = itk::BinaryThresholdImageFilter<InputImageType, BinaryImageType>;
	auto binary_filter = FilterType::New();
	binary_filter->SetInput(volume);
	binary_filter->SetLowerThreshold(lowerThreshold);
	binary_filter->SetUpperThreshold(upperThreshold);
	binary_filter->SetInsideValue(insideValue);	//max of unsigned char
	binary_filter->SetOutsideValue(outsideValue);	//1
	binary_filter->Update();

	//replace cc
	using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter<BinaryImageType, OutputImageType>;
	ConnectedComponentImageFilterType::Pointer connectedComponentImageFilter = ConnectedComponentImageFilterType::New();
	connectedComponentImageFilter->SetInput(binary_filter->GetOutput());
	connectedComponentImageFilter->Update();

	//label image statistics filter
	//2nd step, perform a shape analysis, and use elongation to get only the frames
	//cc = connectedThreshold->GetOutput();
	using LabelStatisticsImageFilterType = itk::LabelStatisticsImageFilter<InputImageType, OutputImageType>;
	LabelStatisticsImageFilterType::Pointer labelStatisticsImageFilter = LabelStatisticsImageFilterType::New();
	labelStatisticsImageFilter->SetInput(volume);
	labelStatisticsImageFilter->SetLabelInput(connectedComponentImageFilter->GetOutput());	//connectedThreshold
	labelStatisticsImageFilter->Update();

	//iterate thru labels
	std::cout << "Number of labels: " << connectedComponentImageFilter->GetNumberOfIndexedOutputs() << std::endl;
	NShape NShapes[3];
	int nShapeNum = 0;

	using LabelPixelType = LabelStatisticsImageFilterType::LabelPixelType;
	for (auto vIt = labelStatisticsImageFilter->GetValidLabelValues().begin();
		vIt != labelStatisticsImageFilter->GetValidLabelValues().end();
		++vIt)
	{
		int pixelCount = labelStatisticsImageFilter->GetCount(*vIt);
		bool bHeadFrame = pixelCount > regionLoThreshold && pixelCount < regionHiThreshold && labelStatisticsImageFilter->GetMean(*vIt) > lowerThreshold;
		if (pixelCount > 13000) bHeadFrame = false;	//temp hack for outliers
		if (bHeadFrame)
		{
			LabelPixelType labelValue = *vIt;
			std::cout << labelValue << " , min: " << labelStatisticsImageFilter->GetMinimum(labelValue) << " , max: " <<
				labelStatisticsImageFilter->GetMaximum(labelValue) << ", median: " << labelStatisticsImageFilter->GetMedian(labelValue) <<
				" , mean: " << labelStatisticsImageFilter->GetMean(labelValue) << ", count: " << labelStatisticsImageFilter->GetCount(labelValue) <<
				" , region: " << labelStatisticsImageFilter->GetRegion(labelValue) << std::endl;
			//connectedThreshold->GetOutput()
			//Then, you may obtain the pixels of each label object like this:		
			//set a volume with the correct labels
			//thining and ransac for each segment
			//thinningAndLineFitting();
			//extract all labels, then extract the centerlines; use thinning insteaduse
			OutputImageType::Pointer parts = connectedComponentImageFilter->GetOutput();
			//double check if revise seg, would affect next iteration thinning
			BinaryImageType::Pointer segThin = BinaryImageType::New();
			DeepCopyPart<OutputImageType, BinaryImageType, LabelPixelType>(parts, segThin, labelValue);

			//line detection could work without thinning (or first erosion)
			bool bUseErosion = false;
			bool bUseThinning = false;

			if (bUseThinning)
			{
				//the binary thinning needs much work to optimize, current running time is too long
				//using ThinningFilterType = itk::BinaryThinningImageFilter<BinaryImageType, BinaryImageType>;
				using ThinningFilterType = itk::BinaryThinningImageFilter3D<BinaryImageType, BinaryImageType>;
				ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
				//original binary thinning takes too long
				thinningFilter->SetInput(segThin);		//segThin
				thinningFilter->Update();
				segThin = thinningFilter->GetOutput();
			}

			//extract the binary points				

			//fill into a Point3d vector
			std::vector<Point3i*> points;
			itk::ImageRegionConstIterator<BinaryImageType> inputIterator(segThin, segThin->GetLargestPossibleRegion());
			while (!inputIterator.IsAtEnd())
			{
				if (inputIterator.Get())
				{
					BinaryImageType::IndexType index = inputIterator.GetIndex();
					points.push_back(new Point3i(int(index[0]), int(index[1]), int(index[2])));
				}
				++inputIterator;
			}

			std::vector<Point3i*>	best_inliers, best_inliers2, best_inliers3;
			Point3d startPoint1, startPoint2, startPoint3;
			Point3d normal1, normal2, normal3;
			std::vector<Point3i*> pointsRemain = points;
			//extract 1st line
			lineRansac(pointsRemain, normal1, startPoint1, best_inliers);
			//extract 2nd line 
			lineRansac(pointsRemain, normal2, startPoint2, best_inliers2);
			//extract 3rd line 
			lineRansac(pointsRemain, normal3, startPoint3, best_inliers3);
			//cout << "normal1: " << normal1 << " , startPoint1: " << startPoint1 << endl;
			//cout << "normal2: " << normal2 << " , startPoint2: " << startPoint2 << endl;
			//cout << "normal3: " << normal3 << " , startPoint3: " << startPoint3 << endl;

			////fit a cvLine
			//lineParam first 3: represents the starting point, last 3: represents the normal vector
			//Output line parameters. In case of 2D fitting, it should be a vector of 4 elements (like Vec4f) - (vx, vy, x0, y0), 
			//where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line. 
			//In case of 3D fitting, it should be a vector of 6 elements (like Vec6f) - (vx, vy, vz, x0, y0, z0), 
			//where (vx, vy, vz) is a normalized vector collinear to the line and (x0, y0, z0) is a point on the line.				
			//or I can compute the intersection of 2 lines directly

			//find the point which are close to two lines
			float minDist12 = 1e10, minDist13 = 1e10, minDist23 = 1e10;
			Point3i minPoint12, minPoint13, minPoint23;
			std::vector<Point3i*>::iterator iter = points.begin();

			for (; iter != points.end(); ++iter)
			{
				Point3i* point = *iter;
				Point3d vecDiff = { point->x - startPoint1.x, point->y - startPoint1.y, point->z - startPoint1.z };
				//float distance = cv::norm((cv::Vec3d)vecDiff.cross(normal1)); 
				float distance = GetDistance(vecDiff, normal1);
				vecDiff = { point->x - startPoint2.x, point->y - startPoint2.y, point->z - startPoint2.z };
				float distance2 = GetDistance(vecDiff,normal2);
				vecDiff = { point->x - startPoint3.x, point->y - startPoint3.y, point->z - startPoint3.z };
				float distance3 = GetDistance(vecDiff,normal3);
				if ((distance + distance2) < minDist12)
				{
					minDist12 = distance + distance2;
					minPoint12 = *point;
				}
				if ((distance + distance3) < minDist13)
				{
					minDist13 = distance + distance3;
					minPoint13 = *point;
				}
				if ((distance2 + distance3) < minDist23)
				{
					minDist23 = distance2 + distance3;
					minPoint23 = *point;
				}
			}
			//cout << " min point length: " << minDist12 << " ,  position: " << minPoint12 << endl;
			//cout << " min point length: " << minDist13 << " ,  position: " << minPoint13 << endl;
			//cout << " min point length: " << minDist23 << " ,  position: " << minPoint23 << endl;

			//to determine the order of end points
			float dTH = 10;
			if (minDist12 < dTH && minDist13 < dTH)
			{
				//it means: segment 2 and segment 3 are end branch, segment 1 is the middle branch
				//now find the correct endPoints on the other side
				//loop thru segment 2, and find the point farthest from minPoint12
				float maxDistance1 = -1e10, maxDistance2 = -1e10;

				for (std::vector<Point3i*>::const_iterator iter = best_inliers2.begin(); iter != best_inliers2.end(); iter++)
				{
					Point3i vecDiff = { (*iter)->x - minPoint12.x, (*iter)->y - minPoint12.y, (*iter)->z - minPoint12.z };
					float distance = GetDot(vecDiff,normal2);
					if (distance > maxDistance1)
					{
						maxDistance1 = distance;
						startPoint2.x = (*iter)->x;
						startPoint2.y = (*iter)->y;
						startPoint2.z = (*iter)->z;
					}
				}
				//loop thru segment3, and find the point farthest from minPoint13
				for (std::vector<Point3i*>::const_iterator iter = best_inliers3.begin(); iter != best_inliers3.end(); iter++)
				{
					Point3i vecDiff = { (*iter)->x - minPoint13.x, (*iter)->y - minPoint13.y,(*iter)->z - minPoint13.z };
					float distance = GetDot(vecDiff,normal3);
					if (distance > maxDistance2)
					{
						maxDistance2 = distance;
						startPoint3.x = (*iter)->x;
						startPoint3.y = (*iter)->y;
						startPoint3.z = (*iter)->z;
					}
				}

				InputImageType::Pointer volRef = volume;
				if (nShapeNum >= 3)
				{
					return;
				}
				NShapes[nShapeNum].startPoints[0] = convertIndexToPoint(volume, volRef, startPoint2, wrapper);	// startPoint2;
				NShapes[nShapeNum].endPoints[0] = convertIndexToPoint(volume, volRef, minPoint12, wrapper);
				Point3d vecA = { double(minPoint12.x - startPoint2.x), double(minPoint12.y - startPoint2.y), double(minPoint12.z - startPoint2.z) };
				NShapes[nShapeNum].normals[0] = cv::normalize((cv::Vec3d)vecA);

				NShapes[nShapeNum].startPoints[1] = convertIndexToPoint(volume, volRef, minPoint12, wrapper);
				NShapes[nShapeNum].endPoints[1] = convertIndexToPoint(volume, volRef, minPoint13, wrapper);
				vecA = { double(minPoint13.x - minPoint12.x), double(minPoint13.y - minPoint12.y), double(minPoint13.z - minPoint12.z) };
				NShapes[nShapeNum].normals[1] = cv::normalize((cv::Vec3d)vecA);

				NShapes[nShapeNum].startPoints[2] = convertIndexToPoint(volume, volRef, minPoint13, wrapper);
				NShapes[nShapeNum].endPoints[2] = convertIndexToPoint(volume, volRef, startPoint3, wrapper);
				NShapes[nShapeNum].startPoints[3] = NShapes[nShapeNum].endPoints[2];	//also fill in the last startPoint
				vecA = { double(startPoint3.x - minPoint13.x), double(startPoint3.y - minPoint13.y), double(startPoint3.z - minPoint13.z) };
				NShapes[nShapeNum].normals[2] = cv::normalize((cv::Vec3d)vecA);

				//cout << "nShapeNum " << nShapeNum << " 0: start  " << NShapes[nShapeNum].startPoints[0] << " , end " << NShapes[nShapeNum].endPoints[0] << " , normal " << NShapes[nShapeNum].normals[0] << endl;
				//cout << "nShapeNum " << nShapeNum << " 1: start  " << NShapes[nShapeNum].startPoints[1] << " , end " << NShapes[nShapeNum].endPoints[1] << " , normal " << NShapes[nShapeNum].normals[1] << endl;
				//cout << "nShapeNum " << nShapeNum << " 2: start  " << NShapes[nShapeNum].startPoints[2] << " , end " << NShapes[nShapeNum].endPoints[2] << " , normal " << NShapes[nShapeNum].normals[2] << endl;

				//finished filling one NShapes
				nShapeNum++;
				//move to the very end
				bool debug = false;
				if (debug)
					vIt = labelStatisticsImageFilter->GetValidLabelValues().end() - 1;
			}
		}
	}

	//N shape num not 3, some detection problems occur
	if (nShapeNum < 3)
	{
		return;
	}

	//supposedly, targetPoint is in the basicNode image space
	//the 3 N shape could be in random order, as connected component found


	constexpr unsigned numberOfContours = 3;
	constexpr unsigned numberOfPoints = 4;
	//should check the normal[1] of the 3 N shape, for the two aligned, get the center points of the two lines

	Point3d normal0 = NShapes[0].normals[1];
	Point3d normal1 = NShapes[1].normals[1];
	Point3d normal2 = NShapes[2].normals[1];

	//dot product between each pair

	int k1 = -1, k2 = -1, k3 = -1;

	if (GetDot(normal0,normal1) > 0.9)
	{
		k1 = 0; k2 = 1; k3 = 2;
	}
	else if (GetDot(normal0, normal2) > 0.9)
	{
		k1 = 0; k2 = 2; k3 = 1;
	}
	else if (GetDot(normal0, normal2) > 0.9)
	{
		k1 = 1; k2 = 2; k3 = 0;
	}

	Point3d fc[3];
	for (int k = 0; k < 3; k++)
	{
		fc[k] = (NShapes[k].startPoints[1] + NShapes[k].startPoints[2]) / 2.0;
	}


	if (k1 >= 0 && k2 >= 0)
	{
		frameCenter = (fc[k1] + fc[k2]) / 2.0;
		//frameCenter to AC is y axis, frameCenter to Left is x axis
		frameX = cv::normalize((cv::Vec3d)(fc[k2] - frameCenter));	//need to check whether k1 or k2 are on the right
		frameY = cv::normalize((cv::Vec3d)(fc[k3] - frameCenter));
		frameZ = frameX.cross(frameY);

		SaveHeadFrameInfo(frameCenter, "HeadFrameCenter");
		SaveHeadFrameInfo(frameX, "HeadFrameX");
		SaveHeadFrameInfo(frameY, "HeadFrameY");
		SaveHeadFrameInfo(frameZ, "HeadFrameZ");

		//iterate thru the fiberPosition vector
		/*std::vector<FiberPosition>::iterator iter;
		for (iter = fiberPosition.begin(); iter != fiberPosition.end(); iter++)
		{
			GetFiberInfoAfterSetHeadFrame(*iter);
		}*/
	}

	//now, convert frame coords to head-stage coords


	//what var to save
	//each "N" shape, the 3 norm, 3 starting points, and the 2 intersections
	//with 3N, 9 norm, 9 starting points, and the 6 intersections
	//also, conversion from pixel location to physical coords is needed
	mitk::Surface::Pointer surface = mitk::Surface::New();
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	points->SetNumberOfPoints(numberOfPoints * numberOfContours);

	Point3d pt;
	vtkIdType cpNumber = 0;
	//plot the 3 N shapes
	//add 3D STL model representing the head frame	
	//also, coord transform is needed			

	for (unsigned k = 0; k < numberOfContours; k++)
	{
		double point[3];
		vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
		polyLine->GetPointIds()->SetNumberOfIds(numberOfPoints);
		for (vtkIdType i = 0; i < numberOfPoints; i++)
		{
			pt = NShapes[k].startPoints[i];
			point[0] = pt.x; point[1] = pt.y; point[2] = pt.z;
			//cout << "point " << i << " : " << pt << endl;
			points->SetPoint(cpNumber, point);
			polyLine->GetPointIds()->SetId(i, cpNumber);
			++cpNumber;
		}
		cells->InsertNextCell(polyLine);
	}

	// Create a polydata to store everything in
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	// Add the points to the dataset
	polyData->SetPoints(points);
	// Add the lines to the dataset
	polyData->SetLines(cells);
	surface->SetVtkPolyData(polyData);

	mitk::DataNode::Pointer dn = mitk::DataNode::New();
	QString tmpName = QString("HeadFrame");

	auto node1 = dataStorage->GetNamedNode(tmpName.toStdString());
	if (node1) {
		dataStorage->Remove(node1);
	}
	dn->SetName(tmpName.toStdString().c_str());
	if (dataStorage->Exists(dn)) {
		return;
	}
	dn->SetName(tmpName.toStdString().c_str());
	dn->SetData(surface);
	dn->SetOpacity(0.5);
	//Set the head frame to hide, only displayed in 3D, which does not affect the position of the camera
	dn->SetVisibility(false);
	dn->SetVisibility(true, mitk::BaseRenderer::GetInstance(
		mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget4")));
	dataStorage->Add(dn);

	GLMessageDialog::ShowInformation(QStringLiteral("头架识别"), QStringLiteral("头架识别成功!"));
}


