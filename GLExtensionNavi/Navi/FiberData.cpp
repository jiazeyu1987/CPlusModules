#include "FiberData.h"
//#include "mitkPointSet.h"
//
////日志
//#include <mbilog.h>
//#include <mitkLogMacros.h>
//#include <mitkLog.h>
//#include <vtkTransformFilter.h>
//#include <vtkTransformPolyDataFilter.h>
//FiberData::FiberData(QString name, mitk::DataNode::Pointer point_node_1) {
//	m_TagName = name; 
//	point_node = point_node_1;
//	ObserveNewNode1(point_node);
//}
//
//
//


int FiberData::fiber_type = 1;
FiberData::FiberData() 
{
	
};
FiberData::~FiberData() 
{
	
}
//
//void FiberData::SetDataNodeInfo(mitk::DataNode* in_stl) {
//	MITK_INFO << "光纤数据-设置数据Node信息 " << __FUNCTION__;
//	stl_node = in_stl;
//}
//
//mitk::Surface::Pointer FiberData::RotateSurface(mitk::Surface::Pointer surface,mitk::Point3D center_point, mitk::Vector3D orientationVector, mitk::Vector3D defaultVector) {
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
//	transform->PostMultiply();
//	transform->Identity();
//	transform->Translate(-center_point[0], -center_point[1], -center_point[2]);
//	transform->RotateWXYZ(rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
//	transform->Translate(center_point[0], center_point[1], center_point[2]);
//	
//
//	vtkSmartPointer<vtkTransformPolyDataFilter> TrafoFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
//	TrafoFilter->SetTransform(transform);
//	TrafoFilter->SetInputData(surface->GetVtkPolyData());
//	TrafoFilter->Update();
//	surface->SetVtkPolyData(TrafoFilter->GetOutput());
//	return surface;
//}
//
//
//void FiberData::ObserveNewNode1(mitk::DataNode *node)
//{
//	MITK_INFO << "--USER|仪器设置模块-光纤数据-Node1添加的新的监视（事务模型） " << __FUNCTION__;
//	if (m_DataInteractor1.IsNotNull())
//		m_DataInteractor1->SetDataNode(node);
//
//	// remove old observer
//	if (point_node.IsNotNull())
//	{
//		if (m_DataInteractor1)
//		{
//			m_DataInteractor1 = nullptr;
//		}
//
//		point_node->RemoveObserver(m_NodeObserverTag1);
//		m_NodeObserverTag1 = 0;
//	}
//
//	point_node = node;
//	
//	// add new observer if necessary
//	if (point_node)
//	{
//		itk::ReceptorMemberCommand<FiberData>::Pointer command =
//			itk::ReceptorMemberCommand<FiberData>::New();
//		command->SetCallbackFunction(this, &FiberData::OnNodeDeleted);
//		m_NodeObserverTag1 = point_node->AddObserver(itk::DeleteEvent(), command);
//
//		
//		
//		
//	}
//	else
//	{
//		m_NodeObserverTag1 = 0;
//	}
//}
//
//
//void FiberData::OnNodeDeleted(const itk::EventObject &)
//{
//	MITK_INFO << "光纤数据-Node删除 " << __FUNCTION__;
//	if (point_node.IsNotNull() && !m_NodeObserverTag1)
//		point_node->RemoveObserver(m_NodeObserverTag1);
//	m_NodeObserverTag1 = 0;
//	point_node = nullptr;
//}