#pragma once

#include <QObject>
#include <QStringList>
#include <QProcess>

class SirilProcessor : public QObject
{
	Q_OBJECT

public:
	explicit SirilProcessor(QObject* parent = nullptr);

	bool processFiles(const QStringList& sourceFiles, const QString& masterDark, const QString& masterFlat);

signals:
	void processingFinished(int exitCode, const QString& output, const QString& error);

private:
	QString createProcessFolder(const QString& sourceDir);
	QString createSirilScript(const QString& processDir, const QString& sourceDir, const QString& masterDark, const QString& masterFlat);
	void runSirilScript(const QString& scriptPath, const QString& workingDir);

	QProcess* sirilProcess = nullptr;
};
