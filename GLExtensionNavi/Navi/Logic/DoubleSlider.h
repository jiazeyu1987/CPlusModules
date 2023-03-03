#pragma once

#include <QWidget>
#include <qobject.h>
#include "itkImage.h"
#include <iostream>
#include "vtkMRMLScene.h"
#include "vtkCellArray.h"
#include "vtkPolyLine.h"
#include "vtkPolyData.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkSlicerModelsLogic.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLSegmentationNode.h"
using namespace std;
using InputImageType = itk::Image<short, 3>;
using BinaryImageType = itk::Image<unsigned char, 3>;	//float
using OutputImageType = itk::Image<unsigned short, 3>;



class Point3I {
public:
	int x;
	int y;
	int z;
	Point3I() {
		x = 0; y = 0; z = 0;
	}
	Point3I(int _x, int _y, int _z) {
		x = _x; y = _y; z = _z;
	}
	Point3I& operator= (const Point3I &other) {
		if (this == &other)  return *this;
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
};

class Point3D {
public:
	double x;
	double y;
	double z;
	Point3D() {
		x = 0; y = 0; z = 0;
	}
	Point3D(double _x, double _y, double _z) {
		x = _x; y = _y; z = _z;
	}
	Point3D& operator= (const Point3D &other) {
		if (this == &other)  return *this;
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
	Point3D& operator= (const Point3I &other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	Point3D operator+(const Point3D &point2) {
		Point3D res;
		res.x = x + point2.x;
		res.y = y + point2.y;
		res.z = z + point2.z;
		return res;
	}

	Point3D operator-(const Point3D &point2) {
		Point3D res;
		res.x = x - point2.x;
		res.y = y - point2.y;
		res.z = z - point2.z;
		return res;
	}
	Point3D operator/(int num) {
		Point3D res;
		if (num == 0) return res;
		res.x = x/num;
		res.y = y/num;
		res.z = z/num;
		return res;
	}
	double dot(const Point3D &point2) {
		double result = 0.0;
		result += x * point2.x;
		result += y * point2.y;
		result += z * point2.z;
		return result;
	}
	Point3D cross(const Point3D &vector2) {
		Point3D res;
		res.x = y * vector2.z - z * vector2.y;
		res.y = z * vector2.x - x * vector2.z;
		res.z = x * vector2.y - y * vector2.x;
		return res;
	}
	void printSelf() {
		cout << "x= " << x << " y= " << y << " z= " << z;
	}
};

class DoubleSlider : public QWidget
{
	

public:
	static DoubleSlider* GetInstance();

public:
	DoubleSlider();
	~DoubleSlider();
	void SetLogic(vtkMRMLAbstractLogic* logic) { m_ModelsLogic = vtkSlicerModelsLogic::SafeDownCast(logic); };
	vtkSlicerModelsLogic* m_ModelsLogic = nullptr;
	vtkMRMLModelNode* OnIdentifyHeadFramePositionNew(vtkMRMLScene* ,InputImageType::Pointer volume, const char* transform_id);
	Point3D frameCenter, frameX, frameY, frameZ;
};
