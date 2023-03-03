#include "qGLFilterTools.h"

#include "qcollator.h" 
#include "qdir.h"
#include "qfile.h"

void qGLFilterTools::SplitFileFold(QString path, QString toPath)
{
	QStringList tmpList = GetFileNameInDir(path, "*.Dcm");
	int count = tmpList.size();

	for (int i = count / 2; i < count; i++) {
		QString sourceFile = path + "/" + tmpList[i];
		QString targetFile = toPath + "/" + tmpList[i];
		bool result = QFile::copy(sourceFile, targetFile);
		QFile::remove(sourceFile);
	}
}

QStringList qGLFilterTools::GetFileNameInDir(const QString &path, QString tail, bool isNeedTotalPath)
{
	QStringList filesList;
	QDir dir(path);
	if (!dir.exists())
		return filesList;

	QStringList filters;
	filters << QString(tail);
	dir.setFilter(QDir::Files | QDir::NoSymLinks); 
	dir.setNameFilters(filters); 
	dir.setSorting(QDir::NoSort);
	auto entryList = dir.entryList();

	int fileNum = entryList.size();
	if (fileNum <= 0)
		return filesList;
	QCollator collator;
	collator.setNumericMode(true);

	std::sort(
		entryList.begin(),
		entryList.end(),
		[&collator](const QString &file1, const QString &file2)
	{
		return collator.compare(file1, file2) < 0;
	});

	if(!isNeedTotalPath) return entryList;
	for (int i = 0; i < fileNum; i++)
	{
		QString file_name = entryList[i];
		filesList.append(path + "/" + file_name);
	}
	return filesList;
}
