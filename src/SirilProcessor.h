#pragma once

#include <QWidget>
#include <QStringList>
#include <QProcess>

#include "ui_SirilProcessor.h"

class SirilProcessor : public QWidget
{
	Q_OBJECT

public:
	explicit SirilProcessor(QWidget* parent = nullptr);
	~SirilProcessor();

	bool processFiles(const QStringList& sourceFiles, const QString& masterDark, const QString& masterFlat);

signals:
	void processingFinished(int exitCode, const QString& output, const QString& error);

private:
	QString createProcessFolder(const QString& sourceDir);
	QString createSirilScript(const QString& processDir, const QString& sourceDir, const QString& masterDark, const QString& masterFlat);
	void runSirilScript(const QString& scriptPath, const QString& workingDir);

	Ui::SirilProcessor ui;
	QProcess* sirilProcess = nullptr;
};
