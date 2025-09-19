#include "ImageOrganizer.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QDirIterator>
#include "fitsio.h"

ImageOrganizer::ImageOrganizer(QObject* parent)
	: QObject(parent)
{
}

ImageOrganizer::~ImageOrganizer()
{
}

QMap<ImageType, QStringList> ImageOrganizer::organizeImages(const QString& directoryPath)
{
	QMap<ImageType, QStringList> imagesByType;
	QDir dir(directoryPath);
	QStringList filters = { "*.fit", "*.fits" };
	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

	for (const QFileInfo& fileInfo : fileList) {
		ImageType type = determineImageType(fileInfo.absoluteFilePath());
		imagesByType[type].append(fileInfo.absoluteFilePath());
	}
	return imagesByType;
}

ImageType ImageOrganizer::determineImageType(const QString& filePath)
{
	// Stub: In a real implementation, read FITS header to determine type
	// For now, use filename heuristics
	QString lower = filePath.toLower();
	if (lower.contains("light")) return ImageType::Light;
	if (lower.contains("dark")) return ImageType::Dark;
	if (lower.contains("flat")) return ImageType::Flat;
	if (lower.contains("bias")) return ImageType::Bias;
	return ImageType::Unknown;
}

bool ImageOrganizer::moveImagesToSubfolders(const QString& directoryPath, const QMap<ImageType, QStringList>& imagesByType)
{
	QDir dir(directoryPath);
	bool allSuccess = true;

	for (auto it = imagesByType.begin(); it != imagesByType.end(); ++it) {
		QString subfolder = dir.filePath(imageTypeToString(it.key()));
		if (!dir.exists(subfolder)) {
			dir.mkdir(subfolder);
		}
		for (const QString& filePath : it.value()) {
			QFileInfo fileInfo(filePath);
			QString destPath = QDir(subfolder).filePath(fileInfo.fileName());
			if (!QFile::rename(filePath, destPath)) {
				qWarning() << "Failed to move" << filePath << "to" << destPath;
				allSuccess = false;
			}
		}
	}
	return allSuccess;
}

QString ImageOrganizer::imageTypeToString(ImageType type)
{
	switch (type) {
		case ImageType::Light: return "lights";
		case ImageType::Dark:  return "darks";
		case ImageType::Flat:  return "flats";
		case ImageType::Bias:  return "bias";
		default:               return "unknown";
	}
}

QList<QString> ImageOrganizer::scanFitsFiles(const QString& folderPath)
{
	QList<QString> fitsFiles;
	QDirIterator it(folderPath, QStringList() << "*.fits" << "*.fit", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		fitsFiles.append(it.next());
	}
	return fitsFiles;
}

FitsFileMeta ImageOrganizer::extractFitsMeta(const QString& filePath)
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

QList<FitsFileMeta> ImageOrganizer::loadFitsFilesWithMeta(const QString& folderPath)
{
	QList<FitsFileMeta> metaList;
	auto files = scanFitsFiles(folderPath);
	for (const auto& file : files) {
		metaList.append(extractFitsMeta(file));
	}
	return metaList;
}