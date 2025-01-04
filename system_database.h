#ifndef SYSTEM_DATABASE_H
#define SYSTEM_DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

typedef struct
{
    QString account;
    QString password;
}usergame;

class System_database
{
public:
    System_database(){};


    System_database(QString tableName);

    void setTableName(QString tableName);

    // 打开数据库
    bool openDb(void);
    // 创建数据表
    void createTable();
    // 判断数据表是否存在
    bool isTableExist();
    // 查询全部数据
    void queryTable();
    // 插入数据
    void singleInsertData(usergame &singleData); // 插入单条数据

    bool isMatched(usergame &singleData);

    bool isExist(usergame &singleData);

    // 关闭数据库
    void closeDb(void);

private:
    QSqlDatabase database;// 用于建立和数据库的连接
    QString tableName;
};


#endif // SYSTEM_DATABASE_H
