#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class DatabaseManager : public QObject
{
	Q_OBJECT

public:
	explicit DatabaseManager(QObject* parent = nullptr);
	~DatabaseManager();

	bool openDatabase(const QString& dbPath);
	void closeDatabase();
	bool isOpen() const;

private:
	QSqlDatabase m_database;
};