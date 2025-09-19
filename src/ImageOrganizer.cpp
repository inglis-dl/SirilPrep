#include "ImageOrganizer.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

ImageOrganizer::ImageOrganizer(QObject* parent)
	: QObject(parent)
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