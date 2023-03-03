#ifndef Plugin_Const_h
#define Plugin_Const_h

#include "qstringlist.h"

class FiberOutPutInfo
{
public:
	//it should be in order, start->end-> start->end-> start->end, with correct normal direction
	QString fiberName;
	QString fiberType;
	double X, Y, Z;
	double arcAngle, ringAngle;
};

class PluginConst {
public:
	static QStringList m_FiberTyps;

	const static QString PrivacyPolicy;

	const static QString ProtocolInfo;
};
#endif // !Plugin_Const_h