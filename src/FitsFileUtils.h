#pragma once

#include <QString>
#include <QList>
#include "ImageOrganizer.h" // For FitsFileMeta

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
};