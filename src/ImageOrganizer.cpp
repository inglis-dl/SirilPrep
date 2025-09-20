#include "ImageOrganizer.h"
#include "ui_ImageOrganizer.h"
#include "FitsFileUtils.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <QStandardItemModel>
#include <fitsio.h>

ImageOrganizer::ImageOrganizer(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_fitsModel = new QStandardItemModel(this);
	m_fitsModel->setColumnCount(7);
	m_fitsModel->setHeaderData(0, Qt::Horizontal, "File");
	m_fitsModel->setHeaderData(1, Qt::Horizontal, "Object");
	m_fitsModel->setHeaderData(2, Qt::Horizontal, "Date Obs");
	m_fitsModel->setHeaderData(3, Qt::Horizontal, "Exp Time");
	m_fitsModel->setHeaderData(4, Qt::Horizontal, "CCD-TEMP");
	m_fitsModel->setHeaderData(5, Qt::Horizontal, "IMAGETYP");
	m_fitsModel->setHeaderData(6, Qt::Horizontal, "GAIN");
	ui.tableViewFitsFiles->setModel(m_fitsModel);

	connect(ui.pushButtonBrowseFolder, &QPushButton::clicked,
			this, &ImageOrganizer::onBrowseFolderClicked);
	connect(ui.lineEditFolder, &QLineEdit::editingFinished,
			this, &ImageOrganizer::onFolderEdited);
}

ImageOrganizer::~ImageOrganizer()
{
}

void ImageOrganizer::onBrowseFolderClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select FITS Directory"));
	if (!dir.isEmpty()) {
		ui.lineEditFolder->setText(dir);
		loadDirectory(dir);
	}
}

void ImageOrganizer::onFolderEdited()
{
	QString dir = ui.lineEditFolder->text();
	if (!dir.isEmpty() && QDir(dir).exists()) {
		loadDirectory(dir);
	}
}

void ImageOrganizer::loadDirectory(const QString& directoryPath)
{
	m_fitsMetaList = loadFitsFilesWithMeta(directoryPath);
	updateFitsTable();
	emit directoryLoaded(directoryPath);
}

void ImageOrganizer::updateFitsTable()
{
	m_fitsModel->removeRows(0, m_fitsModel->rowCount());
	int row = 0;
	for (const auto& meta : m_fitsMetaList) {
		m_fitsModel->insertRow(row);
		m_fitsModel->setData(m_fitsModel->index(row, 0), QFileInfo(meta.filePath).fileName());
		m_fitsModel->setData(m_fitsModel->index(row, 1), meta.object);
		m_fitsModel->setData(m_fitsModel->index(row, 2), meta.dateObs);
		m_fitsModel->setData(m_fitsModel->index(row, 3), meta.exptime);
		m_fitsModel->setData(m_fitsModel->index(row, 4), meta.ccdTemp);
		m_fitsModel->setData(m_fitsModel->index(row, 5), FitsFileUtils::imageTypeToString(meta.imageType));
		m_fitsModel->setData(m_fitsModel->index(row, 6), meta.gain);
		++row;
	}
	ui.tableViewFitsFiles->resizeColumnsToContents();
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
		QString subfolder = dir.filePath(FitsFileUtils::imageTypeToString(it.key()));
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

QList<FitsFileMeta> ImageOrganizer::loadFitsFilesWithMeta(const QString& folderPath)
{
	QList<FitsFileMeta> metaList;
	auto fitsFiles = FitsFileUtils::scanFitsFiles(folderPath);
	for (const auto& info : fitsFiles) {
		FitsFileMeta meta = FitsFileUtils::extractFitsMeta(info);
		metaList.append(meta);
	}
	// Do not call updateFitsTable here, as it is now handled in loadDirectory
	return metaList;
}
