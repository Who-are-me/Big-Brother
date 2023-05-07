#pragma once

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDate>
#include <QDebug>


#define DATABASE_HOSTNAME           "BigBrotherDatabase"
#define DATABASE_NAME               "BigBrotherDatabase.db"

#define TABLE                       "Data"
#define TABLE_IMAGE                 "image"
#define TABLE_HASH                  "hash"
#define TABLE_SIMILARITY            "similarity_persent"


class Database {
private:
    QSqlDatabase db;

    bool openDatabase();
    bool restoreDatabase();
    bool createTable();

public:
    explicit Database();
    ~Database();

    bool connectToDatabase();
    bool insertIntoTable(const QVariantList &data);
    bool deleteFromTable(int index);
    bool closeDatabase();
};

