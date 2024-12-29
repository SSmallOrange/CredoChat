/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      QMainWindow
 *
 * @author     Orange
 * @date       2024/09/17
 * @history
 *****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Login/logindialog.h"
#include "Login/registerdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void switchRegisterDlg();

private:
    void init();

private:
    Ui::MainWindow *    ui;
    LoginDialog*        _loginDialog;
    RegisterDialog*     _registerDialog;
};
#endif // MAINWINDOW_H
