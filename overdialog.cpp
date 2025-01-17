﻿#include "overdialog.h"
#include "ui_overdialog.h"
#include "QMessageBox"

overDialog::overDialog(int mode,int score,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::overDialog)
{
    ui->setupUi(this);


    QPixmap p;
    QSize picSize;
    picSize.setHeight(240);
    picSize.setWidth(560);

    if(mode == 1) p.load(":/res/dialog/victory.png");
    else p.load(":/res/dialog/gameover.png");

    ui->label->setPixmap(p.scaled(picSize,Qt::IgnoreAspectRatio));

    ui->score->setText(QString::number(score));

    setFixedSize(geometry().size());
    setWindowTitle("Game over");
}

overDialog::~overDialog()
{
    delete ui;
}


void overDialog::on_exitBtn_clicked()
{
    emit exit();
    this->hide();
}


void overDialog::on_hardBtn_clicked()
{
    emit again(HARD);
    this->hide();
}

void overDialog::on_basicBtn_clicked()
{
    emit again(BASIC);
    this->hide();
}

void overDialog::on_mediumBtn_clicked()
{
    emit again(MEDIUM);
    this->hide();
}

void overDialog::closeEvent(QCloseEvent *e){
    //若关闭窗口，默认从基础模式开始
    emit again(BASIC);
}




void overDialog::on_EleBtn_clicked()
{
    emit again(ELEMENTARY);
    this->hide();
}
