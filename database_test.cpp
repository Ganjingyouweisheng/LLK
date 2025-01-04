//#include <QCoreApplication>
//#include "user_database.h"
//#include <system_database.h>
//#include <QString>

//int main(int argc, char *argv[])
//{
//    QCoreApplication a(argc, argv);


//    //创建并打开SQLite数据库
//    User_database sqlTest;
//    sqlTest.openDb();

//    QSqlQuery t = sqlTest.queryTable("admin");
//    int sum = 0;
//    while(t.next()){
//        sum++;
//    }

//    qDebug()<<sum;


////    System_database sysDatabase("system_database");
////    sysDatabase.openDb();
////    sysDatabase.createTable();


////    usergame userTest1 = {"11111", "zhangSan"};
////    usergame userTest2 = {"22222", "lisi"};
////    sysDatabase.singleInsertData("system_database",userTest1);
////    sysDatabase.singleInsertData("system_database",userTest2);

////    sysDatabase.queryTable();

////    userTest2.account = "555";
////    qDebug()<<sysDatabase.isMatched(userTest1);
//    //qDebug()<<sysDatabase.isExist(userTest2);


////    // 创建数据表
////    sqlTest.createTable();

////    // 判断数据表是否存在
////    QString str1 = QString("user_database");
////    qDebug() << "isTabelExist：" <<sqlTest.isTableExist(str1);

////    // 插入单条数据
////    user userTest1 = {"1", "zhangSan", 24};
////    user userTest2 = {"2", "lisi", 28};
////    sqlTest.singleInsertData(userTest1);
////    sqlTest.singleInsertData(userTest2);

////    // 插入多条数据
////    QList<user> list;
////    user userTest3 = {"3", "liwu", 26};
////    user userTest4 = {"4", "niuer", 27};
////    list.append(userTest3);
////    list.append(userTest4);
////    sqlTest.moreInsertData(list);
////    // 查询全部数据
////    sqlTest.queryTable();
////    qDebug() << endl;

////    // 修改数据
////    sqlTest.modifyData("2", "modify", 10);
////    // 查询全部数据
////    sqlTest.queryTable();
////    qDebug() << endl;

////    // 删除数据
////    sqlTest.deleteData("2");
////    // 查询全部数据
////    sqlTest.queryTable();
////    qDebug() << endl;

////    // 删除数据表
////    QString str2 = QString("user_database");
////    //sqlTest.deleteTable(str2);

////    //关闭数据库
////    sqlTest.closeDb();

//    return a.exec();
//}
