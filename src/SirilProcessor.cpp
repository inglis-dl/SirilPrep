#include "SirilProcessor.h"
#include "ui_SirilProcessor.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

SirilProcessor::SirilProcessor(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

SirilProcessor::~SirilProcessor() {
}

bool SirilProcessor::processFiles(const QStringList& sourceFiles, const QString& masterDark, const QString& masterFlat)
{
	if (sourceFiles.isEmpty())
		return false;

	QFileInfo firstFile(sourceFiles.first());
	QString sourceDir = firstFile.absolutePath();
	QString processDir = createProcessFolder(sourceDir);
	if (processDir.isEmpty())
		return false;

	QString scriptPath = createSirilScript(processDir, sourceDir, masterDark, masterFlat);
	if (scriptPath.isEmpty())
		return false;

	runSirilScript(scriptPath, processDir);
	return true;
}

QString SirilProcessor::createProcessFolder(const QString& sourceDir)
{
	QDir dir(sourceDir);
	QString processPath = dir.filePath("process");
	if (!dir.exists("process")) {
		if (!dir.mkdir("process"))
			return QString();
	}
	return processPath;
}

QString SirilProcessor::createSirilScript(const QString& processDir, const QString& sourceDir, const QString& masterDark, const QString& masterFlat)
{
	QString scriptPath = QDir(processDir).filePath("process.ssf");
	QFile scriptFile(scriptPath);
	if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return QString();

	QTextStream out(&scriptFile);
	// Write your Siril commands here, as before
	out << "# Example Siril script\n";
	out << "cd " << sourceDir << "\n";
	// ... more commands ...
	scriptFile.close();
	return scriptPath;
}

void SirilProcessor::runSirilScript(const QString& scriptPath, const QString& workingDir)
{
	if (sirilProcess)
		delete sirilProcess;

	sirilProcess = new QProcess(this);
	connect(sirilProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			[this](int exitCode, QProcess::ExitStatus) {
				emit processingFinished(exitCode,
					sirilProcess->readAllStandardOutput(),
					sirilProcess->readAllStandardError());
			});

	QStringList args;
	args << "-s" << scriptPath;
	sirilProcess->setWorkingDirectory(workingDir);
	sirilProcess->start("siril", args);
}