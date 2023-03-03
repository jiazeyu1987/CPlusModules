#include "STLTool.h"
//
////日志
//#include <mbilog.h>
//#include <mitkLogMacros.h>
//#include <mitkLog.h>
//#include "itkImage.h"
//#include "itkImageRegionIterator.h"
//#include "mitkImageCast.h"
//#include "vtkPolygon.h"
//#include "mitkCylinder.h"
//#include "vtkCylinderSource.h"
//#include "vtkTransformPolyDataFilter.h"
//#include "mitkMatrixConvert.h"
//#include "mitkMatrixConvert.h"
//#include "vtkSphereSource.h"
//#include "vtkAppendPolyData.h"
//#include "mitkEllipsoid.h"
STLTool::STLTool()
{
	//MITK_INFO << "STL工具-构造函数1 " << __FUNCTION__;
}
//STLTool::STLTool(mitk::DataStorage* ds)
//{
//	//MITK_INFO << "STL工具-构造函数2 " << __FUNCTION__;
//	m_Storage = ds;
//}
//
STLTool::~STLTool()
{
	//MITK_INFO << "STL工具-析构函数 " << __FUNCTION__;
}
//
//
//bool STLTool::TransformSurface(mitk::Surface::Pointer surface,
//	itk::Matrix<double, 3, 3> TransformationR,
//	itk::Vector<double, 3> TransformationT)
//{
//	//MITK_INFO << "STL工具-改变表面" << __FUNCTION__;
//	// apply transformation
//	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//	points->ShallowCopy(surface->GetVtkPolyData()->GetPoints());
//	auto ps = points->GetNumberOfPoints();
//	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
//	{
//		double p[3];
//		points->GetPoint(i, p);
//		mitk::Point3D point;
//		point[0] = p[0];
//		point[1] = p[1];
//		point[2] = p[2];
//		point = TransformPoint(point, TransformationR, TransformationT);
//		p[0] = point[0];
//		p[1] = point[1];
//		p[2] = point[2];
//		points->SetPoint(i, p);
//	}
//	surface->GetVtkPolyData()->SetPoints(points);
//
//	return true;
//}
//
//mitk::Point3D STLTool::TransformPoint(mitk::Point3D point,
//	itk::Matrix<double, 3, 3> TransformationR,
//	itk::Vector<double, 3> TransformationT)
//{
//	//MITK_INFO << "STL工具-改变点 " << __FUNCTION__;
//	mitk::Point3D returnValue = TransformationR * point + TransformationT;
//	return returnValue;
//}
//
//itk::Matrix<double, 3, 3> STLTool::ConvertEulerAnglesToRotationMatrix(double alpha,
//	double beta,
//	double gamma)
//{ 
//	//MITK_INFO << "STL工具-欧拉三角转换到旋转矩阵 " << __FUNCTION__;
//	alpha = alpha * PI / 180;
//	beta = beta * PI / 180;
//	gamma = gamma * PI / 180;
//
//	// convert angles to matrix:
//	itk::Matrix<double, 3, 3> matrix;
//
//	/* x-Konvention (Z, X, Z)
//	matrix[0][0] = cos(alpha) * cos(gamma) - sin(alpha) * cos(beta) * sin(gamma);
//	matrix[0][1] = -cos(alpha) * sin(gamma)- sin(alpha) * cos(beta) * cos(gamma);
//	matrix[0][2] = sin(alpha) * sin(beta);
//
//	matrix[1][0] = sin(alpha) * cos(gamma) + cos(alpha) * cos(beta) * sin(gamma);
//	matrix[1][1] = cos(alpha) * cos(beta) * cos(gamma) - sin(alpha) * sin(gamma);
//	matrix[1][2] = -cos(alpha) * sin(beta);
//
//	matrix[2][0] = sin(beta) * sin(gamma);
//	matrix[2][1] = sin(beta) * cos(gamma);
//	matrix[2][2] = cos(beta); */
//
//	// Luftfahrtnorm (DIN 9300) (Yaw-Pitch-Roll, Z, Y, X)
//	matrix[0][0] = cos(beta) * cos(alpha);
//	matrix[0][1] = cos(beta) * sin(alpha);
//	matrix[0][2] = -sin(beta);
//
//	matrix[1][0] = sin(gamma) * sin(beta) * cos(alpha) - cos(gamma) * sin(alpha);
//	matrix[1][1] = sin(gamma) * sin(beta) * sin(alpha) + cos(gamma) * cos(alpha);
//	matrix[1][2] = sin(gamma) * cos(beta);
//
//	matrix[2][0] = cos(gamma) * sin(beta) * cos(alpha) + sin(gamma) * sin(alpha);
//	matrix[2][1] = cos(gamma) * sin(beta) * sin(alpha) - sin(gamma) * cos(alpha);
//	matrix[2][2] = cos(gamma) * cos(beta);
//
//	return matrix;
//}
//
//
//void STLTool::OnMove(FiberData* fdata) {
//	//MITK_INFO << "STL工具-移动 " << __FUNCTION__;
//	auto pointSet = dynamic_cast<mitk::PointSet *>(fdata->point_node->GetData());
//
//	
//	
//	mitk::Point3D point1 = pointSet->GetPoint(0);
//	double modelLength = fdata->m_Depth;	// 暂时写死
//	double distanceX, distanceY, distanceZ;
//	double x3 = point1[0];
//	double y3 = point1[1];
//	double z3 = point1[2];
//	distanceX =modelLength * sin(fdata->pitch*PI / 180)*sin(fdata->yaw*PI / 180);
//	distanceY = modelLength * sin(fdata->pitch*PI / 180)*cos(fdata->yaw*PI / 180);
//	distanceZ = modelLength * cos(fdata->pitch*PI / 180);
//	itk::Matrix<double, 3, 3> matrix = ConvertEulerAnglesToRotationMatrix(0, 0, 0);
//	itk::Matrix<double, 3, 3> TransformationR = matrix;
//	itk::Vector<double, 3> TransformationT;
//	TransformationT[0] = distanceX;
//	TransformationT[1] = distanceY;
//	TransformationT[2] = distanceZ;
//
//	if (fdata->stl_node.IsNotNull()){
//		auto surface = dynamic_cast<mitk::Surface *>(fdata->stl_node->GetData());
//	if(surface){
//	TransformSurface(
//		surface, TransformationR, TransformationT);
//	}
//	}
//
//	
//	for (int i = 0; i < fdata->surfaces_total_list.size(); i++) {
//		auto sub_list = fdata->surfaces_total_list.at(i);
//		for (int i = 0; i < sub_list.size(); i++) {
//			auto surface = dynamic_cast<mitk::Surface*>(sub_list.at(i)->GetData());
//			if (surface) {
//				TransformSurface(
//					surface, TransformationR, TransformationT);
//			}
//		}
//	}
//	
//	// update view
//	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
//}
//
//mitk::Surface::Pointer STLTool::DoAddCircle(mitk::Point3D tumor_point,mitk::Point3D in_point, mitk::Image* image,
//	 double fiber_length,double radius,FiberData* fiber_data) {
//	mitk::Cylinder::Pointer cylinder = mitk::Cylinder::New();
//	mitk::Ellipsoid::Pointer sphere = mitk::Ellipsoid::New();
//	/*auto pointSet = dynamic_cast<mitk::PointSet *>(fdata->point_node->GetData());
//	mitk::Point3D point2 = pointSet->GetPoint(1);
//	mitk::Point3D point1 = pointSet->GetPoint(0);*/
//	double center_point[3];
//	center_point[0] = (tumor_point[0] + tumor_point[0]) / 2;
//	center_point[1] = (tumor_point[1] + tumor_point[1]) / 2;
//	center_point[2] = (tumor_point[2] + tumor_point[2]) / 2;
//
//	vtkCylinderSource *vcylinder = vtkCylinderSource::New();
//	vcylinder->SetCenter(center_point);
//	vcylinder->SetRadius(radius);
//	vcylinder->SetResolution(100);
//	vcylinder->SetHeight(fiber_length);
//	vcylinder->Update();
//	cylinder->SetVtkPolyData(vcylinder->GetOutput());
//	vcylinder->Delete();
//	mitk::Surface::Pointer surface3 = cylinder;
//
//	vtkSphereSource *vsphere = vtkSphereSource::New();
//	vsphere->SetCenter(center_point);
//	vsphere->SetRadius(radius);
//	vsphere->SetThetaResolution(8);
//	vsphere->SetPhiResolution(8);
//	vsphere->SetStartTheta(0);
//	vsphere->SetEndTheta(180);
//	vsphere->SetStartPhi(0);
//	vsphere->SetEndPhi(180);
//	vsphere->Update();
//	sphere->SetVtkPolyData(vsphere->GetOutput());
//	vsphere->Delete();
//	mitk::Surface::Pointer surface2 = sphere;
//
//	
//
//	mitk::Vector3D orientationVector = tumor_point - in_point;
//	mitk::Vector3D defaultVector;
//	FillVector3D(defaultVector, 0.0, 1.0, 0.0);
//	surface3 = fiber_data->RotateSurface(surface3,center_point, orientationVector,defaultVector);
//	
//	return surface3;
//}
//
//mitk::Surface::Pointer STLTool::DoAddCylinder(mitk::Point3D tumor_point, mitk::Point3D in_point, mitk::Image* image,
//	double fiber_length, double radius, FiberData* fiber_data) {
//	mitk::Cylinder::Pointer cylinder = mitk::Cylinder::New();
//	mitk::Ellipsoid::Pointer sphere = mitk::Ellipsoid::New();
//	/*auto pointSet = dynamic_cast<mitk::PointSet *>(fdata->point_node->GetData());
//	mitk::Point3D point2 = pointSet->GetPoint(1);
//	mitk::Point3D point1 = pointSet->GetPoint(0);*/
//	double center_point[3];
//	center_point[0] = (tumor_point[0] + tumor_point[0]) / 2;
//	center_point[1] = (tumor_point[1] + tumor_point[1]) / 2;
//	center_point[2] = (tumor_point[2] + tumor_point[2]) / 2;
//
//	vtkCylinderSource *vcylinder = vtkCylinderSource::New();
//	vcylinder->SetCenter(center_point);
//	vcylinder->SetRadius(radius);
//	vcylinder->SetResolution(100);
//	vcylinder->SetHeight(fiber_length);
//	vcylinder->Update();
//	cylinder->SetVtkPolyData(vcylinder->GetOutput());
//	vcylinder->Delete();
//	mitk::Surface::Pointer surface3 = cylinder;
//
//	vtkSphereSource *vsphere = vtkSphereSource::New();
//	vsphere->SetCenter(center_point);
//	vsphere->SetRadius(radius);
//	vsphere->SetThetaResolution(8);
//	vsphere->SetPhiResolution(8);
//	vsphere->SetStartTheta(0);
//	vsphere->SetEndTheta(180);
//	vsphere->SetStartPhi(0);
//	vsphere->SetEndPhi(180);
//	vsphere->Update();
//	sphere->SetVtkPolyData(vsphere->GetOutput());
//	vsphere->Delete();
//	mitk::Surface::Pointer surface2 = sphere;
//
//
//
//	mitk::Vector3D orientationVector = tumor_point - in_point;
//	mitk::Vector3D defaultVector;
//	FillVector3D(defaultVector, 0.0, 1.0, 0.0);
//	surface2 = fiber_data->RotateSurface(surface2, center_point, orientationVector, defaultVector);
//
//	return surface2;
//}
//
//
//void STLTool::OnAdjustFiber2(FiberData* fdata) {
//	auto surface = dynamic_cast<mitk::Surface*>(fdata->stl_node->GetData());
//	auto pointset = dynamic_cast<mitk::PointSet*>(fdata->point_node->GetData());
//	mitk::Vector3D orientationVector = pointset->GetPoint(0) - pointset->GetPoint(1);
//	mitk::Vector3D defaultVector;
//	FillVector3D(defaultVector, 0.0, 0.0, 1.0);
//
//	auto bounds = surface->GetGeometry()->GetBounds();
//	auto origin = surface->GetGeometry()->GetOrigin();
//	vtkTransform *transform = vtkTransform::New();
//	mitk::Vector3D rotationAxis = itk::CrossProduct(orientationVector, defaultVector);
//	itk::Matrix<double, 3, 3> matrix;
//	int t = -1;
//	defaultVector.Normalize();
//	orientationVector.Normalize();
//	double rotationAngle = t * acos(defaultVector[0] * orientationVector[0] + defaultVector[1] * orientationVector[1] + defaultVector[2] * orientationVector[2]);
//	//double rotationAngle = atan2((double)defaultVector.GetNorm(), (double)orientationVector.GetNorm());
//	//double rotationAngle = t*asin(rotationAxis.GetNorm()/ defaultVector.Normalize()/ orientationVector.Normalize());
//	rotationAngle *= 180.0 / vnl_math::pi;
//
//	double center_point[3];
//	center_point[0] = 0;
//	center_point[1] = 0;
//	center_point[2] = -101;
//	double fiber_length = 202;
//	transform->PostMultiply();
//	transform->Identity();
//	transform->Translate(-center_point[0], -center_point[1], -center_point[2]);
//	transform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
//	transform->Translate(center_point[0], center_point[1], center_point[2]);
//	transform->Translate(-fiber_length * orientationVector[0] / 2, -fiber_length * orientationVector[1] / 2, -fiber_length * orientationVector[2] / 2);
//
//	vtkSmartPointer<vtkTransformPolyDataFilter> TrafoFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
//	TrafoFilter->SetTransform(transform);
//	TrafoFilter->SetInputData(surface->GetVtkPolyData());
//	TrafoFilter->Update();
//	//surface->SetVtkPolyData(TrafoFilter->GetOutput());
//	fdata->stl_node->SetData(surface);
//	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
//}
//
//void STLTool::OnAdjustFiber(FiberData* fdata, bool isInit) {
//	MITK_INFO << "STL工具-调整光纤 " << __FUNCTION__;
//	
//	auto surface = dynamic_cast<mitk::Surface *>(fdata->stl_node->GetData());
//	auto pointSet = dynamic_cast<mitk::PointSet *>(fdata->point_node->GetData());
//	
//	// 计算添加的两个点所形成的trajectory
//	// point1为目标点
//	mitk::Point3D point1 = pointSet->GetPoint(1);
//	mitk::Point3D point2 = pointSet->GetPoint(0);
//	/*double x3 = point1[0];
//	double y3 = point1[1];
//	double z3 = point1[2];
//	double x4 = point2[0];
//	double y4 = point2[1];
//	double z4 = point2[2];*/
//
//	double x4 = point1[0];
//	double y4 = point1[1];
//	double z4 = point1[2];
//	double x3 = point2[0];
//	double y3 = point2[1];
//	double z3 = point2[2];
//
//	double x2 = x3 - x4, y2 = y3 - y4, z2 = z3 - z4;
//	double x1 = 0, y1 = 0, z1 = 0;
//
//	// 计算yaw和pitch
//	double yaw = 90 - atan((y2 - y1) / ((x2) - x1)) * 180 / 3.1415926;	// in degree
//	double temp = (y2 - y1)*(y2 - y1) + ((x2) - x1)*((x2) - x1);
//	double pitch = 90 - atan((z2 - z1) / (sqrt(temp))) * 180 / 3.1415926;
//
//	if (x2 < 0) 
//	{ 
//		yaw = yaw; pitch =-pitch; 
//	}
//	fdata->yaw = yaw;
//	fdata->pitch = pitch;
//
//	if (isInit == false) {
//		return;
//	}
//	MITK_INFO << "yaw=" << yaw << ",pitch=" << pitch;
//
//	// convert angles to matrix:
//	itk::Matrix<double, 3, 3> matrix = ConvertEulerAnglesToRotationMatrix(0, 0, pitch);
//	itk::Matrix<double, 3, 3> TransformationR = matrix;
//	itk::Vector<double, 3> TransformationT;
//	TransformationT[0] = 0;
//	TransformationT[1] = 0;
//	TransformationT[2] = 0;
//	
//	// transform surface
//	//itk::Matrix<double, 3, 3> actualTransformationR;
//	//itk::Vector<double, 3> actualTransformationT;
//	//myModifier->TransformSurfaceCoGCoordinates(
//	//	surface, TransformationR, TransformationT, actualTransformationR, actualTransformationT);
//	TransformSurface(
//		surface, TransformationR, TransformationT);
//	
//	matrix = ConvertEulerAnglesToRotationMatrix(yaw, 0, 0);
//	TransformationR = matrix;
//	TransformSurface(
//		surface, TransformationR, TransformationT);
//	
//	double modelLength = 202.0;	// 暂时写死
//	double distanceX, distanceY, distanceZ;
//	distanceX = x3 - modelLength * sin(pitch*PI / 180)*sin(yaw*PI / 180);
//	distanceY = y3 - modelLength * sin(pitch*PI / 180)*cos(yaw*PI / 180);
//	distanceZ = z3 - modelLength * cos(pitch*PI / 180);
//	matrix = ConvertEulerAnglesToRotationMatrix(0, 0, 0);
//	TransformationR = matrix;
//	TransformationT[0] = distanceX;
//	TransformationT[1] = distanceY;
//	TransformationT[2] = distanceZ;
//	TransformSurface(
//		surface, TransformationR, TransformationT);
//
//	// update view
//	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
//}
//
//void STLTool::OnAdjustSTLModelClicked()
//{
//	//MITK_INFO << "STL工具-点击调整STL模型 " << __FUNCTION__;
//	mitk::DataNode* dataNode;
//	// 前提为已经建立PointSet（名称为PointSet）且添加了两个点
//	// 默认第1个点为光纤尖端
//	mitk::PointSet* pointSet;
//	// 前提为已经导入了模型（名称为STL）
//	mitk::Surface* surface;
//	// MITK_INFO << surface->GetGeometry()->GetAxisVector(0).Length << surface->GetGeometry()->GetAxisVector(1).Length << surface->GetGeometry()->GetAxisVector(2).Length;
//
//	mitk::DataStorage::SetOfObjects::ConstPointer allObjects = m_Storage->GetAll();
//	for (int i = 0; i < allObjects->size(); i++)
//	{
//		if (allObjects->at(i)->GetConstProperty("name")->GetValueAsString() == "STL")
//		{
//			dataNode = allObjects->GetElement(i);
//			surface = dynamic_cast<mitk::Surface *>(dataNode->GetData());
//		}
//		else if (allObjects->at(i)->GetConstProperty("name")->GetValueAsString() == "PointSet")
//		{
//			dataNode = allObjects->GetElement(i);
//			pointSet = dynamic_cast<mitk::PointSet *>(dataNode->GetData());
//		}
//	}
//
//	// 计算添加的两个点所形成的trajectory
//	// point1为目标点
//	mitk::Point3D point1 = pointSet->GetPoint(0);
//	mitk::Point3D point2 = pointSet->GetPoint(1);
//	double x3 = point1[0];
//	double y3 = point1[1];
//	double z3 = point1[2];
//	double x4 = point2[0];
//	double y4 = point2[1];
//	double z4 = point2[2];
//
//	double x2 = x3 - x4, y2 = y3 - y4, z2 = z3 - z4;
//	double x1 = 0, y1 = 0, z1 = 0;
//
//	// 计算yaw和pitch
//	double yaw = 90 - atan((y2 - y1) / (abs(x2) - x1)) * 180 / 3.1415926;	// in degree
//	double temp = (y2 - y1)*(y2 - y1) + (abs(x2) - x1)*(abs(x2) - x1);
//	double pitch = 90 - atan((z2 - z1) / (sqrt(temp))) * 180 / 3.1415926;
//
//	if (x2 > 0 && y2 > 0 && z2 > 0) { yaw = yaw; pitch = pitch; }      //第一象限√
//	else if (x2 < 0 && y2 >0 && z2 > 0) { yaw = -yaw; pitch = pitch; }     //第二象限√
//	else if (x2 < 0 && y2 < 0 && z2 > 0) { yaw = 360 - yaw; pitch = pitch; }//第三象限√
//	else if (x2 > 0 && y2 < 0 && z2 > 0) { yaw = yaw; pitch = pitch; }      //第四象限√
//	else if (x2 > 0 && y2 > 0 && z2 < 0) { yaw = yaw; pitch = pitch; }      //第五象限√
//	else if (x2 < 0 && y2 > 0 && z2 < 0) { yaw = -yaw; pitch = pitch; }     //第六象限√
//	else if (x2 < 0 && y2 < 0 && z2 < 0) { yaw = -yaw; pitch = pitch; }     //第七象限√
//	else if (x2 > 0 && y2 < 0 && z2 < 0) { yaw = yaw; pitch = pitch; }      //第八象限√
//
//	MITK_INFO << "yaw=" << yaw << ",pitch=" << pitch;
//
//	// convert angles to matrix:
//	itk::Matrix<double, 3, 3> matrix = ConvertEulerAnglesToRotationMatrix(0, 0, pitch);
//	itk::Matrix<double, 3, 3> TransformationR = matrix;
//	itk::Vector<double, 3> TransformationT;
//	TransformationT[0] = 0;
//	TransformationT[1] = 0;
//	TransformationT[2] = 0;
//
//	// transform surface
//	//itk::Matrix<double, 3, 3> actualTransformationR;
//	//itk::Vector<double, 3> actualTransformationT;
//	//myModifier->TransformSurfaceCoGCoordinates(
//	//	surface, TransformationR, TransformationT, actualTransformationR, actualTransformationT);
//	TransformSurface(
//		surface, TransformationR, TransformationT);
//
//	matrix = ConvertEulerAnglesToRotationMatrix(yaw, 0, 0);
//	TransformationR = matrix;
//	TransformSurface(
//		surface, TransformationR, TransformationT);
//
//	double modelLength = 202.0;	// 暂时写死
//	double distanceX, distanceY, distanceZ;
//	distanceX = x3 - modelLength * sin(pitch*PI / 180)*sin(yaw*PI / 180);
//	distanceY = y3 - modelLength * sin(pitch*PI / 180)*cos(yaw*PI / 180);
//	distanceZ = z3 - modelLength * cos(pitch*PI / 180);
//	matrix = ConvertEulerAnglesToRotationMatrix(0, 0, 0);
//	TransformationR = matrix;
//	TransformationT[0] = distanceX;
//	TransformationT[1] = distanceY;
//	TransformationT[2] = distanceZ;
//	TransformSurface(
//		surface, TransformationR, TransformationT);
//
//	// update view
//	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
//}