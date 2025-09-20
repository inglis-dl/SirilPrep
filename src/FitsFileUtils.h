#pragma once

#include <QString>
#include <QList>

// --- Move these here ---
enum class ImageType {
	Light,
	Dark,
	Flat,
	Bias,
	Unknown
};

struct FitsFileMeta {
	QString filePath;
	QString object;
	QString dateObs;
	double exptime = 0.0;
	ImageType imageType = ImageType::Unknown;
	double ccdTemp = 0.0; // CCD-TEMP metadata
	double gain = 0.0;    // GAIN metadata
};
// ---

class FitsFileUtils
{
public:
	// Returns a list of FITS file paths in the given folder (recursively)
	static QList<QString> scanFitsFiles(const QString& folderPath);

	// Extracts metadata from a single FITS file
	static FitsFileMeta extractFitsMeta(const QString& filePath);

	// Returns a list of FitsFileMeta for all FITS files in the folder
	static QList<FitsFileMeta> scanFitsFilesWithMeta(const QString& folderPath);

	// Utility functions for renaming
	static QString extractObjectFromFileName(const QString& fileName);
	static QString replaceObjectInFileName(const QString& fileName, const QString& oldObject, const QString& newObject);

	static QString imageTypeToString(ImageType type);
};