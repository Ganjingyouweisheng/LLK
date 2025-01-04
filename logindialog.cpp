#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>
#include <QMessageBox>


#include <QMediaPlayer>
loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    setWindowTitle("Welcome to Lianliankan !");
    audioPlayer = new QMediaPlayer(this);
}


loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::closeEvent(QCloseEvent *e)
{
    exit(0);
}

void loginDialog::paintEvent(QPaintEvent *event)
{
//    QPainter painter(this);
//    painter.drawPixmap(rect(),QPixmap(":/res/image/loginPage.jpg"),QRect());
}

void loginDialog::on_loginBtn_clicked()
{
    QString name = ui->nameEL->text();
    QString password = ui->passwordEL->text();

    if(!name.isEmpty() && !password.isEmpty()){
        emit sendingUserMsg(1,name,password);
    }else{
        //提示用户未输入完整
        QMessageBox::warning(this,"Warning","Please check the input!");
    }
}


void loginDialog::on_registerBtn_clicked()
{
    QString name = ui->nameEL->text();
    QString password = ui->passwordEL->text();

    if(!name.isEmpty() && !password.isEmpty()){
        emit sendingUserMsg(0,name,password);
    }else{
        //提示用户未输入完整
        QMessageBox::warning(this,"Warning","Please check the input!");
    }
}

void loginDialog::on_loginState(bool state)
{
    if(state){
        audioPlayer->setSource(QUrl(":/res/sound/done.wav"));
        audioPlayer->play();
        //QSound::play(":/res/sound/done.wav");
        QMessageBox::information(this,"Message","Login in successfully!");
        hide();
    }else{
        QMessageBox::warning(this,"Warning","Wrong user name or password!");
        ui->passwordEL->clear();
    }
}

void loginDialog::on_registerState(bool state)
{
    if(state){
        audioPlayer->setSource(QUrl(":/res/sound/done.wav"));
        audioPlayer->play();
        //QSound::play(":/res/sound/done.wav");
        QMessageBox::information(this,"Message","Register successfully!");
    }else{
        QMessageBox::warning(this,"Warning","User already exists. Please check your input!");
        ui->nameEL->clear();
        ui->passwordEL->clear();
    }
}


void loginDialog::on_nameEL_textEdited(const QString &arg1)
{
    audioPlayer->setSource(QUrl(":/res/sound/tap.wav"));
    audioPlayer->play();
   // QSound::play(":/res/sound/tap.wav");
}

void loginDialog::on_passwordEL_textEdited(const QString &arg1)
{
    audioPlayer->setSource(QUrl(":/res/sound/tap.wav"));
    audioPlayer->play();
    //QSound::play(":/res/sound/tap.wav");
}



