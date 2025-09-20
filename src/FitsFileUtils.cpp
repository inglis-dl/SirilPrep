#include "FitsFileUtils.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <fitsio.h>

QList<QString> FitsFileUtils::scanFitsFiles(const QString& folderPath)
{
	QList<QString> fitsFiles;
	QDirIterator it(folderPath, QStringList() << "*.fits" << "*.fit", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		fitsFiles.append(it.next());
	}
	return fitsFiles;
}

FitsFileMeta FitsFileUtils::extractFitsMeta(const QString& filePath)
{
	FitsFileMeta meta;
	meta.filePath = filePath;
	fitsfile* fptr = nullptr;
	int status = 0;
	char value[FLEN_VALUE], comment[FLEN_COMMENT];

	if (fits_open_file(&fptr, filePath.toStdString().c_str(), READONLY, &status) == 0) {
		// OBJECT
		if (fits_read_key(fptr, TSTRING, "OBJECT", value, comment, &status) == 0)
			meta.object = QString::fromLatin1(value);
		status = 0;
		// DATE-OBS
		if (fits_read_key(fptr, TSTRING, "DATE-OBS", value, comment, &status) == 0)
			meta.dateObs = QString::fromLatin1(value);
		status = 0;
		// EXPTIME
		double exptime = 0.0;
		if (fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, comment, &status) == 0)
			meta.exptime = exptime;
		status = 0;
		fits_close_file(fptr, &status);
	}
	return meta;
}

QList<FitsFileMeta> FitsFileUtils::scanFitsFilesWithMeta(const QString& folderPath)
{
	QList<FitsFileMeta> metaList;
	auto fitsFiles = scanFitsFiles(folderPath);
	for (const auto& filePath : fitsFiles) {
		metaList.append(extractFitsMeta(filePath));
	}
	return metaList;
}

QString FitsFileUtils::extractObjectFromFileName(const QString& fileName)
{
	// Example: "M31_Light_001.fit" -> "M31"
	QStringList parts = fileName.split('_');
	if (!parts.isEmpty())
		return parts.first();
	return QString();
}

QString FitsFileUtils::replaceObjectInFileName(const QString& fileName, const QString& oldObject, const QString& newObject)
{
	if (oldObject.isEmpty())
		return fileName;
	QString newName = fileName;
	int idx = newName.indexOf(oldObject);
	if (idx != -1)
		newName.replace(idx, oldObject.length(), newObject);
	return newName;
}