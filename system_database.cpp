#include "system_database.h"

// 构造函数中初始化数据库对象，并建立数据库
System_database::System_database(QString tableName)
{
    this->tableName = tableName;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");

    }
    else
    {
        // 建立和SQlite数据库的连接
        database = QSqlDatabase::addDatabase("QSQLITE");
        // 设置数据库文件的名字
        database.setDatabaseName("game_project.db");
    }
}

void System_database::setTableName(QString tableName)
{
    this->tableName = tableName;
}

// 打开数据库
bool System_database::openDb()
{
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    return true;
}

// 创建数据表
void System_database::createTable()
{
    if(isTableExist()) return;
    else{
        // 用于执行sql语句的对象
        QSqlQuery sqlQuery;
        // 构建创建数据库的sql语句字符串
        QString createSql = QString("CREATE TABLE "+tableName+" (\
                              account TEXT PRIMARY KEY NOT NULL,\
                              password TEXT NOT NULL)");
        sqlQuery.prepare(createSql);
        // 执行sql语句
        if(!sqlQuery.exec())
        {
            qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
        }
        else
        {
            qDebug() << "Table created!";
        }
    }
}

// 判断数据库中某个数据表是否存在
bool System_database::isTableExist()
{
    QSqlDatabase database = QSqlDatabase::database();
    if(database.tables().contains(tableName))
    {
        return true;
    }
    return false;
}

// 查询全部数据
void System_database::queryTable()
{
    QSqlQuery sqlQuery;
    if(!sqlQuery.exec("SELECT * FROM "+tableName))
    {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    }
    else
    {
        while(sqlQuery.next())
        {
            QString account = sqlQuery.value(0).toString();
            QString password = sqlQuery.value(1).toString();
            qDebug()<<account<<" "<<password;
        }
    }
}

// 插入单条数据
void System_database::singleInsertData(usergame &singledb)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO "+tableName+" VALUES(:account,:password)");
    sqlQuery.bindValue(":account", singledb.account);
    sqlQuery.bindValue(":password", singledb.password);
    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to insert data. " << sqlQuery.lastError();
    }
}

bool System_database::isMatched(usergame &singledb){
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT * FROM "+tableName+" WHERE account=?");
    sqlQuery.addBindValue(singledb.account);
    if(!sqlQuery.exec()){
        qDebug() << "Error: Fail to select data. " << sqlQuery.lastError();
    }

    sqlQuery.next();
    //比较密码是否匹配
    if(sqlQuery.value(1).toString() == singledb.password) return true;
    else return false;

}

bool System_database::isExist(usergame &singleData)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT * FROM "+tableName+" WHERE account=?");
    sqlQuery.addBindValue(singleData.account);
    if(!sqlQuery.exec()){
        qDebug() << "Error: Fail to select data. " << sqlQuery.lastError();
    }

    if(sqlQuery.next()) return true;
    else return false;
}


void System_database::closeDb(void)
{
    database.close();
}
