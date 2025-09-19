#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

// Enum for image types
enum class ImageType {
	Light,
	Dark,
	Flat,
	Bias,
	Unknown
};

class ImageOrganizer : public QObject
{
	Q_OBJECT

public:
	explicit ImageOrganizer(QObject* parent = nullptr);

	// Organize images in the given directory, returns a map of type to file list
	QMap<ImageType, QStringList> organizeImages(const QString& directoryPath);

	// Utility to determine image type from FITS header or filename
	ImageType determineImageType(const QString& filePath);

	// Move or copy files to subfolders based on type
	bool moveImagesToSubfolders(const QString& directoryPath, const QMap<ImageType, QStringList>& imagesByType);

	// Convert ImageType to string for folder names
	static QString imageTypeToString(ImageType type);
};
