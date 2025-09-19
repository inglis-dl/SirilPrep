#include "FileRenamer.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QPushButton>

FileRenamer::FileRenamer(QWidget* parent)
	: QWidget(parent)
{
	tableWidget = new QTableWidget(this);
	tableWidget->setColumnCount(6);
	tableWidget->setHorizontalHeaderLabels(
		{ "File", "OBJECT", "EXPTIME", "DATE-OBS", "RA", "DEC" }
	);
	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	auto* layout = new QHBoxLayout(this);
	layout->addWidget(tableWidget);
	setLayout(layout);
}

void FileRenamer::loadFitsFiles(const QString& directoryPath)
{
	fitsFiles.clear();
	QDir dir(directoryPath);
	QStringList filters = { "*.fit", "*.fits" };
	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

	for (const QFileInfo& fileInfo : fileList) {
		FitsMetaInfo info = parseFitsHeader(fileInfo.absoluteFilePath());
		info.fileName = fileInfo.fileName();
		fitsFiles.append(info);
	}
	populateTable();
}

FitsMetaInfo FileRenamer::parseFitsHeader(const QString& filePath)
{
	FitsMetaInfo info;
	info.fileName = QFileInfo(filePath).fileName();

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		return info;
	}

	// Simple ASCII header parsing for demonstration (not robust)
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		if (line.contains("OBJECT")) info.object = line.section('=', 1).trimmed();
		if (line.contains("EXPTIME")) info.exptime = line.section('=', 1).trimmed();
		if (line.contains("DATE-OBS")) info.dateObs = line.section('=', 1).trimmed();
		if (line.contains("RA")) info.ra = line.section('=', 1).trimmed();
		if (line.contains("DEC")) info.dec = line.section('=', 1).trimmed();
		if (line.contains("END")) break;
	}
	file.close();
	return info;
}

void FileRenamer::populateTable()
{
	tableWidget->setRowCount(fitsFiles.size());
	for (int i = 0; i < fitsFiles.size(); ++i) {
		const FitsMetaInfo& info = fitsFiles[i];
		tableWidget->setItem(i, 0, new QTableWidgetItem(info.fileName));
		tableWidget->setItem(i, 1, new QTableWidgetItem(info.object));
		tableWidget->setItem(i, 2, new QTableWidgetItem(info.exptime));
		tableWidget->setItem(i, 3, new QTableWidgetItem(info.dateObs));
		tableWidget->setItem(i, 4, new QTableWidgetItem(info.ra));
		tableWidget->setItem(i, 5, new QTableWidgetItem(info.dec));

		// Add a rename button for each row (optional, not implemented in this stub)
		// QPushButton *renameBtn = new QPushButton("Rename");
		// connect(renameBtn, &QPushButton::clicked, [this, i]() { onRenameClicked(i); });
		// tableWidget->setCellWidget(i, 6, renameBtn);
	}
}

void FileRenamer::onRenameClicked(int row)
{
	// Stub: Implement logic to prompt for new OBJECT value and rename file accordingly
}