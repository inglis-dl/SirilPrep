#include "DatabaseManager.h"
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject* parent)
	: QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
	closeDatabase();
}

bool DatabaseManager::openDatabase(const QString& dbPath)
{
	if (m_database.isOpen()) {
		return true;
	}

	m_database = QSqlDatabase::addDatabase("QSQLITE");
	m_database.setDatabaseName(dbPath);

	if (!m_database.open()) {
		qWarning() << "Failed to open database:" << m_database.lastError().text();
		return false;
	}
	return true;
}

void DatabaseManager::closeDatabase()
{
	if (m_database.isOpen()) {
		m_database.close();
	}
}

bool DatabaseManager::isOpen() const
{
	return m_database.isOpen();
}