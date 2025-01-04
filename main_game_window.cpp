#include <QDebug>
//#include <QSound>
#include <QAction>
#include <QMessageBox>
#include <QPainter>
#include <QLine>
#include "main_game_window.h"
#include "ui_main_game_window.h"
#include "QThread"
#include "overdialog.h"
#include <logindialog.h>

#include <QMediaPlayer>
#include <QRegularExpression >
#include<iostream>
using namespace std;


// --------- 全局变量 --------- //
const int kIconSize = 36;
const int kTopMargin = 100;
const int kLeftMargin = 50;

const QString kIconReleasedStyle = "";
const QString kIconClickedStyle = "background-color: rgba(255, 255, 12, 161)";
const QString kIconHintStyle = "background-color: rgba(255, 0, 0, 255)";


const int kGameTimeTotal =  30 * 1000; // 总时间
const int kGameTimerInterval = 1000;
const int kLinkTimerDelay = 700;
// -------------------------- //

// 游戏主界面
MainGameWindow::MainGameWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainGameWindow),
    preIcon(NULL),
    curIcon(NULL)
{
    ui->setupUi(this);
    // 重载eventfilter安装到当前window（其实如果不适用ui文件的话直接可以在window的paintevent里面画）
    ui->centralWidget->installEventFilter(this);

    connect(ui->actionBasic, SIGNAL(triggered(bool)), this, SLOT(createGameWithLevel()));
    connect(ui->actionMedium, SIGNAL(triggered(bool)), this, SLOT(createGameWithLevel()));
    connect(ui->actionHard, SIGNAL(triggered(bool)), this, SLOT(createGameWithLevel()));
    connect(ui->actionele, SIGNAL(triggered(bool)), this, SLOT(createGameWithLevel()));




    connect(ui->actionIntro, SIGNAL(triggered(bool)), this, SLOT(informationDisplay()));
    connect(ui->actionVers, SIGNAL(triggered(bool)), this, SLOT(informationDisplay()));
    connect(ui->actionGroup, SIGNAL(triggered(bool)), this, SLOT(informationDisplay()));

    //开启自动解题线程
    connect(&robot,SIGNAL(permission()),this,SLOT(on_permission()));
    robot.start();

    MusicEnabled = true;

    //连接数据库
    sysData = System_database("system_database");
    sysData.openDb();
    sysData.createTable();

    userDate = User_database();
    userDate.openDb();

    loginDialog pg(this);
    connect(&pg,SIGNAL(sendingUserMsg(bool,QString,QString)),this,SLOT(on_sendingUserMsg(bool,QString,QString)));
    connect(this,SIGNAL(loginState(bool)),&pg,SLOT(on_loginState(bool)));
    connect(this,SIGNAL(registerState(bool)),&pg,SLOT(on_registerState(bool)));
    pg.exec();

    // 初始化游戏
    initGame(HARD);
}

MainGameWindow::~MainGameWindow()
{
    if (game)
        delete game;

    delete ui;
}

void MainGameWindow::initGame(GameLevel level)
{
    // 启动游戏
    game = new GameModel;
    game->startGame(level);

    curLevel = level;

    ui->scoreLab->setText(QString::number(game->getScore()));

    ui->hintsLastLab->setText(QString::number(game->getHintsLast()));

    allFunBtnEnable(true);

    // 根据imagemap初始化imagebutton
    init_imageBtn(1);

    // 进度条
    ui->timeBar->setMaximum(kGameTimeTotal);
    ui->timeBar->setMinimum(0);
    ui->timeBar->setValue(kGameTimeTotal);

    // 游戏计时器
    gameTimer = new QTimer(this);
    connect(gameTimer, SIGNAL(timeout()), this, SLOT(gameTimerEvent()));
    //设置发出timeout()信号的间隔
    gameTimer->start(kGameTimerInterval);

    // 连接状态值
    isLinking = false;

    isReallylinked = 0;

    // 播放背景音乐(QMediaPlayer只能播放绝对路径文件),确保res文件在程序执行文件目录里而不是开发目录
    audioPlayer = new QMediaPlayer(this);

    m_audioPlayer =new QMediaPlayer(this);

    QString curDir = QCoreApplication::applicationDirPath(); // 这个api获取路径在不同系统下不一样,mac 下需要截取路径
    QStringList sections = curDir.split(QRegularExpression ("[/]"));
    QString musicPath;

    for (int i = 0; i < sections.size() - 2; i++)
        musicPath += sections[i] + "/";

    //游戏背景音乐
    audioPlayer->setSource(QUrl::fromLocalFile(musicPath + "QtLianliankan-master/res/sound/background.wav"));
    if(MusicEnabled) audioPlayer->play();
}

void MainGameWindow::createGameWithLevel()
{
    gameTimer->disconnect(gameTimer, SIGNAL(timeout()), this, SLOT(gameTimerEvent()));

    // 先析构之前的
    if (game)
    {
        delete game;
        for (int i = 0;i < MAX_ROW * MAX_COL; i++)
        {
            if (imageButton[i])
               delete imageButton[i];
        }
    }

    // 停止音乐
    if(MusicEnabled) audioPlayer->stop();

    // 重绘
    update();

    QAction *actionSender = (QAction *)dynamic_cast<QAction *>(sender());
    if (actionSender == ui->actionBasic)
    {
        initGame(BASIC);
    }
    else if (actionSender == ui->actionele)
    {
        initGame(ELEMENTARY);
    }
    else if (actionSender == ui->actionMedium)
    {
        initGame(MEDIUM);
    }
    else if (actionSender == ui->actionHard)
    {
        initGame(HARD);
    }

}

void MainGameWindow::informationDisplay()
{
    QAction *actionSender = (QAction *)dynamic_cast<QAction *>(sender());
    if (actionSender == ui->actionIntro)
    {
        QMessageBox::information(this, "Introduction", "连连看");
    }
    else if (actionSender == ui->actionVers)
    {
        QMessageBox::information(this, "Version", "Version 1.1.1");
    }
    else if (actionSender == ui->actionGroup)
    {
        QMessageBox::information(this, "Group",
                "Core Member: \nxxxxx 111111\nxxxxx 2222222\nxxxxx 33333\nxxxxx 44444");
    }
}

void MainGameWindow::gameOver(bool mode)
{
    //停止背景音乐
    if(MusicEnabled) audioPlayer->stop();
    //停止进度条
    gameTimer->stop();

    //记录游戏数据
    QString l;
    switch(curLevel){
    case BASIC:
        l = "basic";
        break;
    case ELEMENTARY:
        l = "elementary";
        break;
    case MEDIUM:
        l = "medium";
        break;
    case HARD:
        l = "hard";
    }
    record re = {user,l,game->getScore()};
    userDate.singleInsertData(re);

    _sleep(500);

    //播放音效
    if(mode){

        m_audioPlayer->setSource(QUrl(":/res/sound/win.wav"));
        m_audioPlayer->play();
        //QSound::play(":/res/sound/win.wav");
    }
    else {
        m_audioPlayer->setSource(QUrl(":/res/sound/lose.wav"));
        m_audioPlayer->play();
        //QSound::play(":/res/sound/lose.wav");
    }

    //创建界面对象
    overDialog dia(mode,game->getScore(),this);
    connect(&dia,SIGNAL(again(GameLevel)),this,SLOT(on_again(GameLevel)));
    connect(&dia,SIGNAL(exit()),this,SLOT(on_exit()));
    dia.exec();
}

void MainGameWindow::allBlocksEnable(bool state)
{
    for(int i = 0; i<MAX_ROW * MAX_COL; i++){
        imageButton[i]->setEnabled(state);
    }
}

void MainGameWindow::allFunBtnEnable(bool state)
{
    ui->hintBtn->setEnabled(state);
    ui->robot_btn->setEnabled(state);
    ui->resetBtn->setEnabled(state);
    ui->pauseBtn->setEnabled(state);
}

void MainGameWindow::init_imageBtn(bool mode)
{
    for(int i = 0; i < MAX_ROW * MAX_COL; i++)
    {
        if(mode) imageButton[i] = new IconButton(this);
        imageButton[i]->setGeometry(kLeftMargin + (i % MAX_COL) * kIconSize, kTopMargin + (i / MAX_COL) * kIconSize, kIconSize, kIconSize);
        // 设置索引
        imageButton[i]->xID = i % MAX_COL;
        imageButton[i]->yID = i / MAX_COL;

        imageButton[i]->show();

        if (game->getGameMap()[i])
        {
            // 有方块就设置图片
            QPixmap iconPix;
            QString fileString=QString(":/res/image/%1.png").arg(game->getGameMap()[i]);
            //fileString.sprintf(":/res/image/%d.png", game->getGameMap()[i]);
            iconPix.load(fileString);
            QIcon icon(iconPix);
            imageButton[i]->setIcon(icon);
            imageButton[i]->setIconSize(QSize(kIconSize, kIconSize));

            // 添加按下的信号槽
            connect(imageButton[i], SIGNAL(pressed()), this, SLOT(onIconButtonPressed()));
        }
        else
            imageButton[i]->hide();
    }
}


void MainGameWindow::onIconButtonPressed()
{
    qDebug()<<"onIconButtonPressed";
    // 如果当前有方块在连接，不能点击方块
    // 因为涉及到多线，可能还要维护队列，有点复杂，就先这么简单处理一下
    if (isLinking)
    {
        // 播放音效
        m_audioPlayer->setSource(QUrl(":/res/sound/release.wav"));
        m_audioPlayer->play();
       // QSound::play(":/res/sound/release.wav");
        return;
    }

    // 记录当前点击的icon
    curIcon = dynamic_cast<IconButton *>(sender());

    if(!preIcon)
    {
        // 播放音效
        m_audioPlayer->setSource(QUrl(":/res/sound/select.wav"));
        m_audioPlayer->play();
        //QSound::play(":/res/sound/select.wav");

        // 如果单击一个icon
        curIcon->setStyleSheet(kIconClickedStyle);
        preIcon = curIcon;
    }
    else
    {
        if(curIcon != preIcon)
        {
            // 如果不是同一个button就都标记,尝试连接
            curIcon->setStyleSheet(kIconClickedStyle);
            if(game->linkTwoTiles(preIcon->xID, preIcon->yID, curIcon->xID, curIcon->yID))
            {
                // 锁住当前状态
                isLinking = true;

                // 播放音效
                m_audioPlayer->setSource(QUrl(":/res/sound/pair.wav"));
                m_audioPlayer->play();
                //QSound::play(":/res/sound/pair.wav");

                //重绘//画出连接线
                update();

                // 延迟后实现连接效果
                QTimer::singleShot(kLinkTimerDelay, this, SLOT(handleLinkEffect()));

                ui->timeBar->setValue(kGameTimeTotal);
                //每连接五次奖励一次提示
                                game->setTimes(game->getTimes() + 1);
                                if(game->getTimes()%5==0 && game->getTimes()!=0)
                                    game->setHintsLast(game->getHintsLast()+1);
                                    ui->hintsLastLab->setText(QString::number(game->getHintsLast()));

                //每次消除一对分数加三                   
                game->setScore(game->getScore()+3);
                ui->scoreLab->setText(QString::number(game->getScore()));
                isReallylinked = false;

                // 检查是否胜利
                if (game->isWin()) gameOver(1);
            }
            else
            {
                // 播放音效
                m_audioPlayer->setSource(QUrl(":/res/sound/release.wav"));
                m_audioPlayer->play();
                //QSound::play(":/res/sound/release.wav");

                // 消除失败，恢复
                preIcon->setStyleSheet(kIconReleasedStyle);
                curIcon->setStyleSheet(kIconReleasedStyle);

                // 指针置空，用于下次点击判断
                preIcon = NULL;
                curIcon = NULL;
            }
        }
        else if(curIcon == preIcon)
        {
            //时间重置

            // 播放音效
            m_audioPlayer->setSource(QUrl(":/res/sound/release.wav"));
            m_audioPlayer->play();
            //QSound::play(":/res/sound/release.wav");

            preIcon->setStyleSheet(kIconReleasedStyle);
            curIcon->setStyleSheet(kIconReleasedStyle);
            preIcon = NULL;
            curIcon = NULL;
        }
    }
}



void MainGameWindow::handleLinkEffect()
{
    // 消除成功，隐藏掉，并析构
    game->paintPoints.clear();
    preIcon->hide();
    curIcon->hide();

    preIcon = NULL;
    curIcon = NULL;

    // 重绘
    //因为paintPoints已经清空，因此再次重绘只会清除画线
    update();

    // 恢复状态
    isLinking = false;
}

bool MainGameWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 重绘时会调用，可以手动调用
    if (event->type() == QEvent::Paint)
    {
        QPainter painter(ui->centralWidget);
        QPen pen;
        QColor color(rand() % 256, rand() % 256, rand() % 256);
        pen.setColor(color);
        pen.setWidth(5);
        painter.setPen(pen);

        // 连接各点画线（注，qt中用标砖vector的size好像有点问题，需要类型转换，否则溢出）
        for (int i = 0; i < int(game->paintPoints.size()) - 1; i++)
        {
            PaintPoint p1 = game->paintPoints[i];
            PaintPoint p2 = game->paintPoints[i + 1];

            // 拿到各button的坐标,注意边缘点坐标
            QPoint btn_pos1;
            QPoint btn_pos2;

            // p1
            // 边界情况会导致某点的坐标是-1或者最大值，不可通过数组的线性查找找到对应点，需进行冗余计算
            if (p1.x == -1)
            {
                btn_pos1 = imageButton[p1.y * MAX_COL + 0]->pos();
                btn_pos1 = QPoint(btn_pos1.x() - kIconSize, btn_pos1.y());
            }
            else if (p1.x == MAX_COL)
            {
                btn_pos1 = imageButton[p1.y * MAX_COL + MAX_COL - 1]->pos();
                btn_pos1 = QPoint(btn_pos1.x() + kIconSize, btn_pos1.y());
            }
            else if (p1.y == -1)
            {
                btn_pos1 = imageButton[0 + p1.x]->pos();
                btn_pos1 = QPoint(btn_pos1.x(), btn_pos1.y() - kIconSize);
            }
            else if (p1.y == MAX_ROW)
            {
                btn_pos1 = imageButton[(MAX_ROW - 1) * MAX_COL + p1.x]->pos();
                btn_pos1 = QPoint(btn_pos1.x(), btn_pos1.y() + kIconSize);
            }
            else
                btn_pos1 = imageButton[p1.y *MAX_COL + p1.x]->pos();

            // p2
            if (p2.x == -1)
            {
                btn_pos2 = imageButton[p2.y * MAX_COL + 0]->pos();
                btn_pos2 = QPoint(btn_pos2.x() - kIconSize, btn_pos2.y());
            }
            else if (p2.x == MAX_COL)
            {
                btn_pos2 = imageButton[p2.y * MAX_COL + MAX_COL - 1]->pos();
                btn_pos2 = QPoint(btn_pos2.x() + kIconSize, btn_pos2.y());
            }
            else if (p2.y == -1)
            {
                btn_pos2 = imageButton[0 + p2.x]->pos();
                btn_pos2 = QPoint(btn_pos2.x(), btn_pos2.y() - kIconSize);
            }
            else if (p2.y == MAX_ROW)
            {
                btn_pos2 = imageButton[(MAX_ROW - 1) * MAX_COL + p2.x]->pos();
                btn_pos2 = QPoint(btn_pos2.x(), btn_pos2.y() + kIconSize);
            }
            else
                btn_pos2 = imageButton[p2.y * MAX_COL + p2.x]->pos();

            // 中心点
            QPoint pos1(btn_pos1.x() + kIconSize / 2, btn_pos1.y() - kIconSize / 2);
            QPoint pos2(btn_pos2.x() + kIconSize / 2, btn_pos2.y() - kIconSize / 2);

            painter.drawLine(pos1, pos2);
        }
        //如果僵局则重绘
                if(game->isFrozen()){
                    game->reset();
                    // 添加button
                    init_imageBtn(0);
                }
        return true;
    }
    else
        return QMainWindow::eventFilter(watched, event);
}

void MainGameWindow::gameTimerEvent()
{
    // 进度条计时效果
    if(ui->timeBar->value() <= 0)
    {
        gameTimer->stop();
        gameOver(0);
    }
    else
    {
        ui->timeBar->setValue(ui->timeBar->value() - kGameTimerInterval);
    }
}

// 提示
void MainGameWindow::on_hintBtn_clicked()
{
    if(!game->getHintsLast()) return;

    // 初始时不能获得提示
    for (int i = 0; i < 4;i++)
        if (game->getHint()[i] == -1)
            return;

    if(!isReallylinked){
            game->setHintsLast(game->getHintsLast()-1);
            isReallylinked = true;
    }
    ui->hintsLastLab->setText(QString::number(game->getHintsLast()));

    int srcX = game->getHint()[0];
    int srcY = game->getHint()[1];
    int dstX = game->getHint()[2];
    int dstY = game->getHint()[3];

    IconButton *srcIcon = imageButton[srcY * MAX_COL + srcX];
    IconButton *dstIcon = imageButton[dstY * MAX_COL + dstX];
    srcIcon->setStyleSheet(kIconHintStyle);
    dstIcon->setStyleSheet(kIconHintStyle);

}

void MainGameWindow::on_robot_btn_clicked()
{
    if(robot.getFlag()){
        robot.setFlag(false);
        ui->robot_btn->setText("auto");
        ui->pauseBtn->setEnabled(true);
        ui->hintBtn->setEnabled(true);
        ui->resetBtn->setEnabled(true);
    }else{
        robot.setFlag(true);
        ui->robot_btn->setText("stop");
        ui->pauseBtn->setEnabled(false);
        ui->hintBtn->setEnabled(false);
        ui->resetBtn->setEnabled(false);
    }
}

void MainGameWindow::on_permission()
{
        // 连接生成提示
        qDebug()<<"on_permission";
        int srcX = game->getHint()[0];
        int srcY = game->getHint()[1];
        int dstX = game->getHint()[2];
        int dstY = game->getHint()[3];

            // 播放音效
        m_audioPlayer->setSource(QUrl(":/res/sound/pair.wav"));
        m_audioPlayer->play();
           // QSound::play(":/res/sound/pair.wav");


            game->linkTwoTiles(srcX, srcY, dstX, dstY);

            preIcon = imageButton[srcY * MAX_COL + srcX];
            curIcon = imageButton[dstY * MAX_COL + dstX];

            //重绘
            update();

            //实现连接效果
            QTimer::singleShot(kLinkTimerDelay, this, SLOT(handleLinkEffect()));

            // 检查是否胜利
            if (game->isWin()){
                //让自动线程停止
                robot.setFlag(false);
                //还原按钮状态
                ui->robot_btn->setText("auto");
                gameOver(1);
            }

}

void MainGameWindow::on_pauseBtn_clicked()
{
    QFont ft;
    ft.setFamily("Rockwell Extra Bold");
    ft.setBold(true);
    if(game->gameStatus == PLAYING){
        game->gameStatus = PAUSE;
        gameTimer->stop();
        if(MusicEnabled) audioPlayer->pause();
        allBlocksEnable(false);
        allFunBtnEnable(false);
        ui->pauseBtn->setEnabled(true);
        ft.setPointSize(8);
        ui->pauseBtn->setFont(ft);
        ui->pauseBtn->setText("continue");
    }else{
        game->gameStatus = PLAYING;
        gameTimer->start(kLinkTimerDelay);
        if(MusicEnabled) audioPlayer->play();
        allBlocksEnable(true);
        allFunBtnEnable(true);
        ft.setPointSize(10);
        ui->pauseBtn->setFont(ft);
        ui->pauseBtn->setText("pause");
    }

}


void MainGameWindow::on_resetBtn_clicked()
{
    //如果有解则扣分
       if(!game->isFrozen()){
           game->setScore(game->getScore()-6);
           ui->scoreLab->setText(QString::number(game->getScore()));
       }
    game->reset();
    // 添加button
    init_imageBtn(0);

}

void MainGameWindow::on_again(GameLevel mode)
{
    gameTimer->disconnect(gameTimer, SIGNAL(timeout()), this, SLOT(gameTimerEvent()));
    // 先析构之前的
    if (game)
    {
        delete game;
        for (int i = 0;i < MAX_ROW * MAX_COL; i++)
        {
            if (imageButton[i])
               delete imageButton[i];
        }
    }

    // 停止音乐
    if(MusicEnabled) audioPlayer->stop();

    // 重绘
    update();

    initGame(mode);
}

void MainGameWindow::on_exit()
{
    //关闭数据库
    sysData.closeDb();
    userDate.closeDb();
    exit(0);
}


void MainGameWindow::on_muteBtn_clicked()
{
    if(MusicEnabled) {
        //将音乐播放的权限修改为false
        MusicEnabled = false;
        audioPlayer->pause();
        ui->muteBtn->setIcon(QIcon(":/res/image/close.png"));
    }else{
        MusicEnabled = true;
        if(game->gameStatus == PLAYING)audioPlayer->play();
        ui->muteBtn->setIcon(QIcon(":/res/image/open.png"));
    }
}

void MainGameWindow::on_sendingUserMsg(bool mode, QString name, QString password)
{
    usergame user = {name,password};
    if(mode){
        if(sysData.isExist(user) && sysData.isMatched(user)){
            this->user = name;
            emit loginState(true);
        }else{
            emit loginState(false);
        }
    }else{
        if(sysData.isExist(user)){
            emit registerState(false);
        }else{
            sysData.singleInsertData(user);
            emit registerState(true);
            //为该用户创建游戏记录表
            userDate.createTable(name);
        }
    }
}


void MainGameWindow::on_recordBtn_clicked()
{
    QString records = "";
    records += "Hello! "+user+"\n";
    records += "Your personal Score:\n";
    records += "No.\tlevel\tscore\t\n";
    QSqlQuery temp = userDate.queryTable(user);
    while(temp.next()){
        records += QString::number(temp.value(0).toInt())+"\t";
        records += temp.value(1).toString()+"\t";
        records += QString::number(temp.value(2).toInt())+"\t";
        records += "\n";
    }
    QMessageBox::information(this,"message",records);
}

