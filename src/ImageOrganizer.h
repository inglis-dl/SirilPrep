#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

// Enum to represent image types
enum class ImageType {
	Light,
	Dark,
	Flat,
	Bias,
	Unknown
};

// Struct to hold FITS file metadata
struct FitsFileMeta {
	QString filePath;
	QString object;
	QString dateObs;
	double exptime = 0.0;
};

class ImageOrganizer : public QObject
{
	Q_OBJECT

public:
	explicit ImageOrganizer(QObject* parent = nullptr);
	~ImageOrganizer();

	// 1. Scan a folder for FITS files (*.fits, *.fit)
	QList<QString> scanFitsFiles(const QString& folderPath);

	// 2. Extract metadata from a single FITS file
	FitsFileMeta extractFitsMeta(const QString& filePath);

	// 2. Scan folder and extract metadata for all FITS files
	QList<FitsFileMeta> loadFitsFilesWithMeta(const QString& folderPath);

	// 3. Analyze metadata to ensure all files are of the same target OBJECT
	bool allFilesSameObject(const QList<FitsFileMeta>& files, QString* foundObject = nullptr, QStringList* differingFiles = nullptr);

	// 4. Update OBJECT header and rename files to match the correct target name
	bool updateFitsObjectAndRenameFiles(const QList<FitsFileMeta>& files, const QString& newObject, QStringList* renamedFiles = nullptr);

	// 5. Set master calibration files (user selection)
	void setMasterDark(const QString& filePath);
	void setMasterFlat(const QString& filePath);
	QString masterDark() const;
	QString masterFlat() const;

	// Organize images by type
	QMap<ImageType, QStringList> organizeImages(const QString& directoryPath);

	// Move images to subfolders by type
	bool moveImagesToSubfolders(const QString& directoryPath, const QMap<ImageType, QStringList>& imagesByType);

	// Convert ImageType to string
	QString imageTypeToString(ImageType type);

	// Determine image type from file path
	ImageType determineImageType(const QString& filePath);

private:
	QString m_masterDark;
	QString m_masterFlat;
};
