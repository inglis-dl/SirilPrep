#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	// Set up window properties here (title, size, etc.)
	setWindowTitle("SirilPrep");
	resize(800, 600);
}

MainWindow::~MainWindow()
{
}