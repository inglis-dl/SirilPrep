#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDir>
#include <QVector>
#include <QString>
#include <QMap>

struct FitsMetaInfo {
	QString fileName;
	QString object;
	QString exptime;
	QString dateObs;
	QString ra;
	QString dec;
	// Add more fields as needed
};

class FileRenamer : public QWidget
{
	Q_OBJECT

public:
	explicit FileRenamer(QWidget* parent = nullptr);
	void loadFitsFiles(const QString& directoryPath);

private slots:
	void onRenameClicked(int row);

private:
	QTableWidget* tableWidget;
	QVector<FitsMetaInfo> fitsFiles;

	FitsMetaInfo parseFitsHeader(const QString& filePath);
	void populateTable();
};
