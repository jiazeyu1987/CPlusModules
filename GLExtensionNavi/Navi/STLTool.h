#pragma once



#include <qobject.h>
//#include "mitkDataNode.h"
//#include "mitkDataStorage.h"
//#include "org_gl_navi_Export.h"

#include <regex>

// vtk headers
#include <vtkMath.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

//#include "mitkSurface.h"
//#include "mitkPointSet.h"
//#include "mitkRenderingManager.h"
#include "FiberData.h"
//#include "mitkLine.h"
//#include "vtkConeSource.h"
class  STLTool :public QObject
{
	Q_OBJECT

public:
	STLTool();
	//STLTool(mitk::DataStorage*);
	~STLTool() override;


protected:
	//mitk::DataStorage::Pointer m_Storage;
private:
	const double PI = 3.1415926;
	/*bool TransformSurface(mitk::Surface::Pointer surface,
		itk::Matrix<double, 3, 3> TransformationR,
		itk::Vector<double, 3> TransformationT);
	mitk::Point3D TransformPoint(mitk::Point3D point,
		itk::Matrix<double, 3, 3> TransformationR,
		itk::Vector<double, 3> TransformationT);
	itk::Matrix<double, 3, 3> ConvertEulerAnglesToRotationMatrix(double alpha,
		double beta,
		double gamma);*/
protected Q_SLOTS:
	
public:
	/*mitk::Surface::Pointer DoAddCircle(mitk::Point3D tumor_point, mitk::Point3D in_point, mitk::Image* image,
		double fiber_length, double radius, FiberData* fiber_data);
	mitk::Surface::Pointer DoAddCylinder(mitk::Point3D tumor_point, mitk::Point3D in_point, mitk::Image* image,
		double fiber_length, double radius, FiberData* fiber_data);*/
	void OnMove(FiberData* fdata);
	void OnAdjustFiber(FiberData* fdata, bool isInit = true);
	void OnAdjustFiber2(FiberData* fdata);
};
