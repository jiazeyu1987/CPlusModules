#ifndef HEAD_FRAME_VIEW_h
#define HEAD_FRAME_VIEW_h

#include "qSlicerNaviModuleExport.h"
#include "qwidget.h"

struct NShape
{
	//it should be in order, start->end-> start->end-> start->end, with correct normal direction
	double startPoints[4];
	double normals[3];
	double endPoints[3];
};

class Point3i {
public:

	int x;
	int y;
	int z;

	Point3i::Point3i() {
		x = 0;
		y = 0;
		z = 0;
	}

	Point3i::Point3i(int x, int y, int z) {
		x = x;
		y = y;
		z = z;
	}
};

class Point3d {
public:
	int x;
	int y;
	int z;


	Point3d::Point3d() {
		x = 0;
		y = 0;
		z = 0;
	}

	Point3d::Point3d(int x, int y, int z) {
		x = x;
		y = y;
		z = z;
	}
};

class Q_SLICER_QTMODULES_NAVI_EXPORT HeadFrameView : public QWidget
{
	Q_OBJECT

public:
	HeadFrameView(QWidget *parent = nullptr);
	~HeadFrameView() override;
	using VectorType = itk::Vector<double, 3>;
	void InitSeriesList();
	HeadFrameView::VectorType GetVector(Point3d p, HeadFrameView::VectorType& vecDiff);
	double GetDistance(Point3d p1, Point3d p2);
	double GetDot(Point3d p1, Point3d p2);
	double GetDot(Point3i p1, Point3d p2);
private slots:
	void OnIdentifyHeadFramePositionNew();
	void ShowSelectSeries(int num);

private:
	//void SaveHeadFrameInfo(double* point, std::string saveKey);

private:
	bool isContainCT;

	double* frameCenter, frameX, frameY, frameZ;


};

#endif // !HEAD_FRAME_VIEW_h



