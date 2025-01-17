﻿#ifndef OVERDIALOG_H
#define OVERDIALOG_H

#include <QDialog>
#include "game_model.h"

namespace Ui {
class overDialog;
}

class overDialog : public QDialog
{
    Q_OBJECT


public:
    overDialog(int mode,int score,QWidget *parent = nullptr);
    ~overDialog();

    void closeEvent(QCloseEvent *e);

private slots:
    void on_exitBtn_clicked();

    void on_hardBtn_clicked();

    void on_basicBtn_clicked();

    void on_mediumBtn_clicked();


    void on_EleBtn_clicked();

private:
    Ui::overDialog *ui;

signals:
    void again(GameLevel mode);
    void exit();
};

#endif // OVERDIALOG_H
