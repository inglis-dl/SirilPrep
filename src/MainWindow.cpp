#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "ImageOrganizer.h"
#include "SirilProcessor.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	// Setup UI from .ui file
	ui->setupUi(this);

	setupConnections();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::setupConnections()
{
}

void MainWindow::onOpenDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select FITS Directory"));
	if (!dir.isEmpty()) {
		ui->imageOrganizer->loadDirectory(dir);
	}
}

void MainWindow::onImageSelectionChanged(const QString& fileName)
{
}
