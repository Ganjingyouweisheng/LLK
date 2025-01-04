#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QPaintEvent>
#include <QMediaPlayer>
namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT


public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();
    void closeEvent(QCloseEvent *e);

protected:
    void paintEvent(QPaintEvent *event);        //重绘事件

private slots:
    void on_loginBtn_clicked();

    void on_registerBtn_clicked();

    void on_loginState(bool state);
    void on_registerState(bool state);

    void on_nameEL_textEdited(const QString &arg1);

    void on_passwordEL_textEdited(const QString &arg1);


signals:

    void sendingUserMsg(bool mode,QString name,QString password);

private:
    Ui::loginDialog *ui;

    QMediaPlayer* audioPlayer;
};

#endif // LOGINDIALOG_H
