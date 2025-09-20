#pragma once

#include <QWidget>
#include <QString>
#include <QList>
#include <QMap>
#include <QStandardItemModel>

#include "ui_ImageOrganizer.h"

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
};

class ImageOrganizer : public QWidget
{
	Q_OBJECT

public:
	explicit ImageOrganizer(QWidget* parent = nullptr);
	~ImageOrganizer();

	QList<QString> scanFitsFiles(const QString& folderPath);
	FitsFileMeta extractFitsMeta(const QString& filePath);
	QList<FitsFileMeta> loadFitsFilesWithMeta(const QString& folderPath);
	bool allFilesSameObject(const QList<FitsFileMeta>& files, QString* foundObject = nullptr, QStringList* differingFiles = nullptr);
	bool updateFitsObject(const QList<FitsFileMeta>& files, const QString& newObject);
	void setMasterDark(const QString& filePath);
	void setMasterFlat(const QString& filePath);
	QString masterDark() const;
	QString masterFlat() const;
	QMap<ImageType, QStringList> organizeImages(const QString& directoryPath);
	bool moveImagesToSubfolders(const QString& directoryPath, const QMap<ImageType, QStringList>& imagesByType);
	QString imageTypeToString(ImageType type);
	ImageType determineImageType(const QString& filePath);
	bool renameFitsFiles(const QList<FitsFileMeta>& files, const QString& newObject, QStringList* renamedFiles = nullptr);

	void loadDirectory(const QString& directoryPath);

signals:
	void directoryLoaded(const QString& directoryPath);

private slots:
	void onBrowseFolderClicked();
	void onFolderEdited();

private:
	QString m_masterDark;
	QString m_masterFlat;
	Ui::ImageOrganizer ui;
	QString m_currentDirectory;
	QStandardItemModel* m_fitsModel = nullptr;
	QList<FitsFileMeta> m_fitsMetaList;

	void updateFitsTable();
};
