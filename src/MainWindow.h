#pragma once

#include <QMainWindow>

namespace Ui { class MainWindow; }


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void onOpenDirectory();
	void onImageSelectionChanged(const QString& fileName);

private:
	Ui::MainWindow* ui = nullptr;
	void setupConnections();
};
