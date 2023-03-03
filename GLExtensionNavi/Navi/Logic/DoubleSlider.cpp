#include "DoubleSlider.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkBinaryThinningImageFilter3D.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkFlatStructuringElement.h"
#include "vtkLine.h"
#include <random>
#include <algorithm>
#include <iterator>

struct NShapeTest
{
	Point3D startPoints[4];
	Point3D normals[3];
	Point3D endPoints[3];
};
Q_GLOBAL_STATIC(DoubleSlider, doubleSlider);

DoubleSlider* DoubleSlider::GetInstance()
{
	return doubleSlider();
}

DoubleSlider::DoubleSlider() {
	
}

DoubleSlider::~DoubleSlider() {
}
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

double MathDotProduct(Point3I point1, Point3D point2)
{
	double result = 0.0;
	result += point1.x*point2.x;
	result += point1.y*point2.y;
	result += point1.z*point2.z;
	return abs(result);
}

double MathGetNorm(Point3D point1)
{
	double result = 0.0;
	result += pow(point1.x, 2);
	result += pow(point1.y, 2);
	result += pow(point1.z, 2);
	return sqrt(result);
}

Point3D MathNormalize(Point3D point1)
{
	double norm = MathGetNorm(point1);
	Point3D result;
	result.x = point1.x / norm;
	result.y = point1.y / norm;
	result.z = point1.z / norm;
	return result;
}

Point3D convertIndexToPoint(itk::Image<short, 3>::Pointer volume, Point3D cvPoint)
{
	itk::Image<short, 3>::IndexType index = { int(cvPoint.x), int(cvPoint.y), int(cvPoint.z) };
	itk::Image<short, 3>::PointType point;
	volume->TransformIndexToPhysicalPoint(index, point);
	itk::Image<short, 3>::PointType outPoint = point;
	//somehow, the origin has to be removed for the STL to show up correctly
	//not sure about coords transform for target and entry point, double check on that 
	//Point3D pt = { point[0] - volume->GetOrigin()[0], point[1] - volume->GetOrigin()[1], point[2] - volume->GetOrigin()[2] };
	Point3D pt = { outPoint[0] - volume->GetOrigin()[0], outPoint[1] - volume->GetOrigin()[0], outPoint[2] - volume->GetOrigin()[0] };

	return pt;
}

Point3D convertIndexToPoint(itk::Image<short, 3>::Pointer volume, itk::Image<short, 3>::Pointer refVolume, Point3D cvPoint)
{
	itk::Image<short, 3>::IndexType index = { int(cvPoint.x), int(cvPoint.y), int(cvPoint.z) };
	itk::Image<short, 3>::PointType point;
	volume->TransformIndexToPhysicalPoint(index, point);
	itk::Image<short, 3>::PointType outPoint = point;
	//somehow, the origin has to be removed for the STL to show up correctly
	//not sure about coords transform for target and entry point, double check on that 
	Point3D pt = { outPoint[0], outPoint[1], outPoint[2] };
	//Point3D pt = { outPoint[0] - refVolume->GetOrigin()[0], outPoint[1] - refVolume->GetOrigin()[0], outPoint[2] - refVolume->GetOrigin()[0] };

	return pt;
}

Point3D convertIndexToPoint(itk::Image<short, 3>::Pointer volume, Point3I cvPoint)
{
	itk::Image<short, 3>::IndexType index = { int(cvPoint.x), int(cvPoint.y), int(cvPoint.z) };
	itk::Image<short, 3>::PointType point;
	volume->TransformIndexToPhysicalPoint(index, point);
	itk::Image<short, 3>::PointType outPoint = point;
	//somehow, the origin has to be removed for the STL to show up correctly
	//not sure about coords transform for target and entry point, double check on that 
	//Point3D pt = { point[0] - volume->GetOrigin()[0], point[1] - volume->GetOrigin()[1], point[2] - volume->GetOrigin()[2] };
	Point3D pt = { outPoint[0] - volume->GetOrigin()[0], outPoint[1] - volume->GetOrigin()[0], outPoint[2] - volume->GetOrigin()[0] };

	return pt;
}

Point3D convertIndexToPoint(itk::Image<short, 3>::Pointer volume, itk::Image<short, 3>::Pointer refVolume, Point3I cvPoint)
{
	itk::Image<short, 3>::IndexType index = { int(cvPoint.x), int(cvPoint.y), int(cvPoint.z) };
	itk::Image<short, 3>::PointType point;
	volume->TransformIndexToPhysicalPoint(index, point);
	itk::Image<short, 3>::PointType outPoint = point;
	//somehow, the origin has to be removed for the STL to show up correctly
	//not sure about coords transform for target and entry point, double check on that 
	Point3D pt = { outPoint[0], outPoint[1], outPoint[2] };
	//Point3D pt = { outPoint[0] - refVolume->GetOrigin()[0], outPoint[1] - refVolume->GetOrigin()[0], outPoint[2] - refVolume->GetOrigin()[0] };

	return pt;
}

void lineRansac(std::vector<Point3I>& points, Point3D &normal, Point3D &startPoint, std::vector<Point3I> &pointsFit, float distTH)
{
	//using ransac to fit a line: the MSE or robust MSE not suitable for multiple lines detection
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
		Point3I point1 = points[id1];
		Point3I point2 = points[id2];
		Point3D vecA = { double(point1.x - point2.x), double(point1.y - point2.y), double(point1.z - point2.z) };
		Point3D vecA_norm = MathNormalize(vecA);

		//Distance from a point to a line
		std::vector<int>	pt_id_inliers;
		std::vector<int>	pt_id_outliers;

		for (int i = 0; i < N; i++)
		{
			Point3D vecB = { double(point1.x - points[i].x), double(point1.y - points[i].y), double(point1.z - points[i].z) };
			float distance = MathGetNorm(vecA_norm.cross(vecB));
			//Select indexes where distance is smaller than the threshold
			if (distance < distTH)
				pt_id_inliers.push_back(i);
		}
		if (pt_id_inliers.size() > best_inliers.size())
		{
			best_inliers = pt_id_inliers;
			normal = vecA_norm;
			startPoint = point1;
		}
	}
	//for the best line params, retrieve fitted points, which is the best_inliers
	//remove those points online, and fit another one
	//optimize later using a pointer
	//Since the ids in idsToRemove are known to be in items and in the same order, you can use a couple of iterators into items to 
	//keep track of the current compare element, the current destination, and walk thru idsToRemove and items, comparing both elements, 
	//moving the ones that you want to keep.At the end of that process, resize items to be the new smaller size.

	std::vector<Point3I> pointsRemained;
	std::vector<int>::iterator it = best_inliers.begin();
	std::vector<Point3I>::iterator iter = points.begin();
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

	//std::vector<Point3I> pointsRemained;
	//std::vector<Point3I>::iterator iter = points.begin();
	//for (int i = 0; iter != points.end(); iter++, i++)
	//{
	//	if (std::find(best_inliers.begin(), best_inliers.end(), i) == best_inliers.end())
	//		pointsRemained.push_back(*iter);
	//}
	points = pointsRemained;
}
	
vtkMRMLModelNode* DoubleSlider::OnIdentifyHeadFramePositionNew(vtkMRMLScene* scene,InputImageType::Pointer volume, const char* transform_id)
{
	constexpr unsigned int Dimension = 3;
	using InputImageType = itk::Image<short, Dimension>;
	using BinaryImageType = itk::Image<unsigned char, Dimension>;	//float
	using OutputImageType = itk::Image<unsigned short, Dimension>;	//float
	InputImageType::RegionType inputRegion = volume->GetBufferedRegion();
	InputImageType::SizeType   size = inputRegion.GetSize();
	float distTh = 1.5;
	float scale = size[0] / 512;
	distTh = distTh * scale;
	short lowerThreshold = 350;
	short upperThreshold = 2000;
	//fixed length 120mm, N shape, number of pixels(2 + sqrt(2)) * 120mm*(1 / resolution3)
	unsigned short regionLoThreshold = 2200*scale; //around 6000 - 7000
	unsigned short regionHiThreshold = 13000*scale;	//20000 originally

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
	//std::cout << "Number of labels: " << connectedComponentImageFilter->GetNumberOfIndexedOutputs() << std::endl;
	NShapeTest NShapes[3];
	int nShapeNum = 0;

	using LabelPixelType = LabelStatisticsImageFilterType::LabelPixelType;
	QVector<int> list;
	for (auto vIt = labelStatisticsImageFilter->GetValidLabelValues().begin();
		vIt != labelStatisticsImageFilter->GetValidLabelValues().end();
		++vIt)
	{
		int pixelCount = labelStatisticsImageFilter->GetCount(*vIt);
		bool bHeadFrame = pixelCount > regionLoThreshold && pixelCount < regionHiThreshold && labelStatisticsImageFilter->GetMean(*vIt) > lowerThreshold;
		//if (pixelCount > 13000) bHeadFrame = false;	//temp hack for outliers
		if (bHeadFrame) list.append(pixelCount);
	}
	qSort(list.begin(), list.end());
	regionLoThreshold = list[list.size() - 3]-1;
	for (auto vIt = labelStatisticsImageFilter->GetValidLabelValues().begin();
		vIt != labelStatisticsImageFilter->GetValidLabelValues().end();
		++vIt)
	{
		int pixelCount = labelStatisticsImageFilter->GetCount(*vIt);
		bool bHeadFrame = pixelCount > regionLoThreshold && pixelCount < regionHiThreshold && labelStatisticsImageFilter->GetMean(*vIt) > lowerThreshold;
		//if (pixelCount > 13000) bHeadFrame = false;	//temp hack for outliers
		if (bHeadFrame)
		{
			LabelPixelType labelValue = *vIt;
			/*std::cout << labelValue << " , min: " << labelStatisticsImageFilter->GetMinimum(labelValue) << " , max: " <<
				labelStatisticsImageFilter->GetMaximum(labelValue) << ", median: " << labelStatisticsImageFilter->GetMedian(labelValue) <<
				" , mean: " << labelStatisticsImageFilter->GetMean(labelValue) << ", count: " << labelStatisticsImageFilter->GetCount(labelValue) <<
				" , region: " << labelStatisticsImageFilter->GetRegion(labelValue) << std::endl;*/
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

			//fill into a cv::point3d vector
			std::vector<Point3I> points;
			itk::ImageRegionConstIterator<BinaryImageType> inputIterator(segThin, segThin->GetLargestPossibleRegion());
			while (!inputIterator.IsAtEnd())
			{
				if (inputIterator.Get())
				{
					BinaryImageType::IndexType index = inputIterator.GetIndex();
					points.push_back({ int(index[0]), int(index[1]), int(index[2]) });
				}
				++inputIterator;
			}

			std::vector<Point3I>	best_inliers, best_inliers2, best_inliers3;
			Point3D startPoint1, startPoint2, startPoint3;
			Point3D normal1, normal2, normal3;
			std::vector<Point3I> pointsRemain = points;
			//extract 1st line
			lineRansac(pointsRemain, normal1, startPoint1, best_inliers, distTh);
			//extract 2nd line 
			lineRansac(pointsRemain, normal2, startPoint2, best_inliers2, distTh);
			//extract 3rd line 
			lineRansac(pointsRemain, normal3, startPoint3, best_inliers3, distTh);
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
			Point3I minPoint12, minPoint13, minPoint23;
			std::vector<Point3I>::iterator iter = points.begin();
			for (; iter != points.end(); ++iter)
			{
				Point3I point = *iter;
				Point3D vecDiff = { point.x - startPoint1.x, point.y - startPoint1.y, point.z - startPoint1.z };
				float distance = MathGetNorm(vecDiff.cross(normal1));
				vecDiff = Point3D { point.x - startPoint2.x, point.y - startPoint2.y, point.z - startPoint2.z };
				float distance2 = MathGetNorm(vecDiff.cross(normal2));
				vecDiff = Point3D { point.x - startPoint3.x, point.y - startPoint3.y, point.z - startPoint3.z };
				float distance3 = MathGetNorm(vecDiff.cross(normal3));
				if ((distance + distance2) < minDist12)
				{
					minDist12 = distance + distance2;
					minPoint12 = point;
				}
				if ((distance + distance3) < minDist13)
				{
					minDist13 = distance + distance3;
					minPoint13 = point;
				}
				if ((distance2 + distance3) < minDist23)
				{
					minDist23 = distance2 + distance3;
					minPoint23 = point;
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

				for (std::vector<Point3I>::const_iterator iter = best_inliers2.begin(); iter != best_inliers2.end(); iter++)
				{
					Point3I vecDiff = { iter->x - minPoint12.x, iter->y - minPoint12.y, iter->z - minPoint12.z };
					float distance = MathDotProduct(vecDiff, normal2);;// abs(vecDiff.dot(normal2));
					if (distance > maxDistance1)
					{
						maxDistance1 = distance;
						startPoint2 = *iter;
					}
				}
				//loop thru segment3, and find the point farthest from minPoint13
				for (std::vector<Point3I>::const_iterator iter = best_inliers3.begin(); iter != best_inliers3.end(); iter++)
				{
					Point3I vecDiff = { iter->x - minPoint13.x, iter->y - minPoint13.y,iter->z - minPoint13.z };
					float distance = MathDotProduct(vecDiff, normal3);
					if (distance > maxDistance2)
					{
						maxDistance2 = distance;
						startPoint3 = *iter;
					}
				}

				InputImageType::Pointer volRef = volume;
				//if (nShapeNum >= 3)
				//{
				//	nShapeNum++;
				//	continue;
				//	/*GLMessageDialog::ShowInformation(QStringLiteral("头架识别"), QStringLiteral("头架识别失败, shape num有误!"));
				//	return;*/
				//}
				//assume startPoints 0, 1, 2 is the starting points of N segment
				NShapes[nShapeNum].startPoints[0] = convertIndexToPoint(volume, volRef, startPoint2);	// startPoint2;
				NShapes[nShapeNum].endPoints[0] = convertIndexToPoint(volume, volRef, minPoint12);
				Point3D vecA = { double(minPoint12.x - startPoint2.x), double(minPoint12.y - startPoint2.y), double(minPoint12.z - startPoint2.z) };
				NShapes[nShapeNum].normals[0] = MathNormalize(vecA);

				NShapes[nShapeNum].startPoints[1] = convertIndexToPoint(volume, volRef, minPoint12);
				NShapes[nShapeNum].endPoints[1] = convertIndexToPoint(volume, volRef, minPoint13);
				vecA = Point3D{ double(minPoint13.x - minPoint12.x), double(minPoint13.y - minPoint12.y), double(minPoint13.z - minPoint12.z) };
				NShapes[nShapeNum].normals[1] = MathNormalize(vecA);

				NShapes[nShapeNum].startPoints[2] = convertIndexToPoint(volume, volRef, minPoint13);
				NShapes[nShapeNum].endPoints[2] = convertIndexToPoint(volume, volRef, startPoint3);
				NShapes[nShapeNum].startPoints[3] = NShapes[nShapeNum].endPoints[2];	//also fill in the last startPoint
				vecA = Point3D{ double(startPoint3.x - minPoint13.x), double(startPoint3.y - minPoint13.y), double(startPoint3.z - minPoint13.z) };
				NShapes[nShapeNum].normals[2] = MathNormalize(vecA);

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
		//GLMessageDialog::ShowInformation(QStringLiteral("头架识别"), QStringLiteral("识别失败, 该序列不能做头架识别!"));
		return nullptr;
	}

	//supposedly, targetPoint is in the basicNode image space
	//the 3 N shape could be in random order, as connected component found


	constexpr unsigned numberOfContours = 3;
	constexpr unsigned numberOfPoints = 4;
	//should check the normal[1] of the 3 N shape, for the two aligned, get the center points of the two lines

	Point3D normal0 = NShapes[0].normals[1];
	Point3D normal1 = NShapes[1].normals[1];
	Point3D normal2 = NShapes[2].normals[1];

	//dot product between each pair

	int k1 = -1, k2 = -1, k3 = -1;

	if (abs(normal0.dot(normal1)) > 0.9)
	{
		k1 = 0; k2 = 1; k3 = 2;
	}
	else if (abs(normal0.dot(normal2)) > 0.9)
	{
		k1 = 0; k2 = 2; k3 = 1;
	}
	else if (abs(normal1.dot(normal2)) > 0.9)
	{
		k1 = 1; k2 = 2; k3 = 0;
	}

	Point3D fc[3];
	for (int k = 0; k < 3; k++)
	{
		fc[k] = (NShapes[k].startPoints[1] + NShapes[k].startPoints[2]) / 2.0;
	}


	if (k1 >= 0 && k2 >= 0)
	{
		frameCenter = (fc[k1] + fc[k2]) / 2.0;
		//frameCenter to AC is y axis, frameCenter to Left is x axis
		frameX = MathNormalize((fc[k2] - frameCenter));	//need to check whether k1 or k2 are on the right
		frameY = MathNormalize((fc[k3] - frameCenter));
		frameZ = frameX.cross(frameY);
		frameX = frameX + frameCenter;
		frameY = frameY + frameCenter;
		frameZ = frameZ + frameCenter;
		frameX.printSelf();
		frameY.printSelf();
		frameZ.printSelf();
		frameCenter.printSelf();
		//iterate thru the fiberPosition vector
		/*std::vector<FiberPosition>::iterator iter;
		for (iter = fiberPosition.begin(); iter != fiberPosition.end(); iter++)
		{
			GetFiberInfoAfterSetHeadFrame(*iter);
		}*/
	}

	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	points->SetNumberOfPoints(numberOfPoints* numberOfContours);

	Point3D pt;
	vtkIdType cpNumber = 0;
	//plot the 3 N shapes
	//add 3D STL model representing the head frame	
	//also, coord transform is needed			
	auto leksellpointset = vtkMRMLMarkupsFiducialNode::SafeDownCast(scene->GetFirstNodeByName("leksellpointset"));
	if (!leksellpointset) {
		leksellpointset = vtkMRMLMarkupsFiducialNode::SafeDownCast(scene->AddNewNodeByClass("vtkMRMLMarkupsFiducialNode", "leksellpointset"));
	}
	leksellpointset->AddFiducial(frameX.x, frameX.y, frameX.z, "x");
	leksellpointset->AddFiducial(frameY.x, frameY.y, frameY.z, "y");
	leksellpointset->AddFiducial(frameZ.x, frameZ.y, frameZ.z, "z");
	leksellpointset->AddFiducial(frameCenter.x, frameCenter.y, frameCenter.z, "c");

	for (unsigned k = 0; k < numberOfContours; k++)
	{
		double point[3];
		vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
		polyLine->GetPointIds()->SetNumberOfIds(numberOfPoints);
		for (vtkIdType i = 0; i < numberOfPoints; i++)
		{
			pt = NShapes[k].startPoints[i];
			point[0] = pt.x; point[1] = pt.y; point[2] = pt.z;
			cout << "point " << k * numberOfContours + i << " : " << pt.x <<","<< pt.y<<","<< pt.z << endl;
			leksellpointset->AddFiducial(pt.x, pt.y, pt.z, QString("%1").arg(cpNumber).toStdString());
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
	vtkMRMLModelNode* modelNode = m_ModelsLogic->AddModel(polyData);
	modelNode->SetAndObserveTransformNodeID(transform_id);
	vtkMRMLNode* config_node = scene->GetFirstNodeByName("saveconfig");
	if (config_node == nullptr) {
		return nullptr;
	}
	
	
	auto displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(leksellpointset->GetDisplayNode());
	displayNode->SetSelectedColor(1, 0, 1);
	displayNode->SetVisibility(false);
	leksellpointset->SetLocked(true);
	/*displayNode->SetUseGlyphScale(true);
	displayNode->SetGlyphScale(3);
	displayNode->SetGlyphSize(3);
	displayNode->SetTextScale(5.0);*/
	leksellpointset->SetAndObserveTransformNodeID(transform_id);
	config_node->SetAttribute("leksell_frame_auto_info", leksellpointset->GetID());
	/*QString x1 = QString("%1,%2,%3").arg(frameX.x).arg(frameX.y).arg(frameX.z);
	config_node->SetAttribute("leksell_frameX", x1.toStdString().c_str());
	QString x2 = QString("%1,%2,%3").arg(frameY.x).arg(frameY.y).arg(frameY.z);
	config_node->SetAttribute("leksell_frameY", x2.toStdString().c_str());
	QString x3 = QString("%1,%2,%3").arg(frameZ.x).arg(frameZ.y).arg(frameZ.z);
	config_node->SetAttribute("leksell_frameZ", x3.toStdString().c_str());
	QString c1 = QString("%1,%2,%3").arg(frameCenter.x).arg(frameCenter.y).arg(frameCenter.z);
	config_node->SetAttribute("leksell_center", c1.toStdString().c_str());*/
	return modelNode;
}
