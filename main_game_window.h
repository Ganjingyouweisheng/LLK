#ifndef MAIN_GAME_WINDOW_H
#define MAIN_GAME_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QMediaPlayer>

#include "game_model.h"
#include <auto_conductor.h>

#include <user_database.h>
#include <system_database.h>



namespace Ui {
class MainGameWindow;
}

// 继承自button，存储坐标值
struct IconButton : QPushButton
{
public:
    IconButton(QWidget *parent = Q_NULLPTR) :
        QPushButton(parent),
        xID(-1),
        yID(-1)
    {
    }
    int xID; // x 坐标
    int yID; // y 坐标
};

class MainGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGameWindow(QWidget *parent = 0);
    virtual ~MainGameWindow();

    virtual bool eventFilter(QObject *watched, QEvent *event); // 事件过滤

private:
    Ui::MainGameWindow *ui;
    GameModel *game; // 游戏模型
    IconButton *imageButton[MAX_ROW * MAX_COL]; // 图片button数组
    QTimer *gameTimer; // 游戏计时器
    IconButton *preIcon, *curIcon; // 记录点击的icon
    bool isLinking; // 维持一个连接状态的标志
    bool isReallylinked;
    bool MusicEnabled;  //标志背景音乐是否打开

    QMediaPlayer *audioPlayer; // 音乐播放器

    QMediaPlayer *m_audioPlayer;
    void initGame(GameLevel level); // 初始化游戏

    auto_conductor robot;

    //本用户的账号
    QString user;

    GameLevel curLevel;

    System_database sysData;
    User_database userDate;

    //为游戏区域划定边框
    void init_border();

    //游戏结束后执行的动作
    void gameOver(bool mode);

    void allBlocksEnable(bool state);

    void allFunBtnEnable(bool state);

    void init_imageBtn(bool mode);

private slots:
    void onIconButtonPressed(); // icon点击到响应
    void gameTimerEvent(); // 游戏计时回调
    void handleLinkEffect(); // 实现连接效果
    void on_hintBtn_clicked(); // 提示按钮
    void on_robot_btn_clicked(); // 机器人自动刷
    void createGameWithLevel(); // 选中难度开始
    void informationDisplay();
    void on_permission();
    void on_pauseBtn_clicked();
    void on_resetBtn_clicked();
    void on_again(GameLevel mode);
    void on_exit();
    void on_muteBtn_clicked();
    void on_sendingUserMsg(bool mode,QString name,QString password);
    void on_recordBtn_clicked();

signals:
    //false 代表登录成功 true 代表登录失败
    void loginState(bool state);
    //true 代表注册成功 false 代表用户已存在，注册失败
    void registerState(bool state);
};

#endif // MAIN_GAME_WINDOW_H
