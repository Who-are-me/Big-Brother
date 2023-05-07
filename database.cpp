#include "database.h"


Database::Database() {

}


Database::~Database() {
    this->closeDatabase();
}


bool Database::connectToDatabase() {
    if (!QFile(DATABASE_NAME).exists() ) {
        this->restoreDatabase();
    } else {
        this->openDatabase();
    }

    return true;
}


bool Database::restoreDatabase() {
    if (this->openDatabase()){
        if(!this->createTable()){
            return false;
        } else {
            return true;
        }
    } else {
        qDebug() << "Error: is not create database";
        return false;
    }

    return false;
}


bool Database::openDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(DATABASE_HOSTNAME);
    db.setDatabaseName(DATABASE_NAME);

    if (db.open()){
        return true;
    } else {
        return false;
    }
}


bool Database::closeDatabase() {
    db.close();

    return true;
}


bool Database::createTable() {
    QSqlQuery query;

    if (!query.exec( "CREATE TABLE " TABLE " ( id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            TABLE_IMAGE             " BLOB                  NOT NULL, "
                            TABLE_HASH              " TEXT                  NOT NULL, "
                            TABLE_SIMILARITY        " REAL                  NOT NULL ) "
    )) {
        qDebug() << "Database: error of create " << TABLE;
        qDebug() << query.lastError().text();

        return false;
    }
    else {
        return true;
    }

    return false;
}


bool Database::insertIntoTable(const QVariantList &data) {

    QSqlQuery query;

    query.prepare("INSERT INTO " TABLE " ( "    TABLE_IMAGE " , "
                                                TABLE_HASH " , "
                                                TABLE_SIMILARITY " ) "
                    " VALUES ( :image, :hesh, :similarity ) " );

    query.bindValue(":image",               data[0] );
    query.bindValue(":hash",                data[1] );
    query.bindValue(":similarity",          data[2] );

    if (!query.exec()) {
        qDebug() << "error insert into " << TABLE;
        qDebug() << query.lastError().text();

        return false;
    } else {
        return true;
    }

    return false;
}


bool Database::deleteFromTable(int index) {
    QSqlQuery query_m;
    query_m.prepare("DELETE FROM " TABLE " WHERE id = :index");
    query_m.bindValue(":index", index);
    query_m.exec();

    return true;
}
