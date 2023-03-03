#ifndef q_GLFilterTools_h
#define q_GLFilterTools_h
 
#include "qstringlist.h"

#include "qSlicerFilterRightModuleWidgetsExport.h"
class  qGLFilterTools
{
public:

	//split one fold to two folds and divide files by file number
	static void SplitFileFold(QString path, QString toPath);

	//get all files which hava specified tail
	static QStringList GetFileNameInDir(const QString &path, QString tail, bool isNeedTotalPath=false);
};

#endif // !q_GLFilterTools_h


