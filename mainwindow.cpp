#include "mainwindow.h"
#include "ui_mainwindow.h"

#define SAFE_DELETE_PTR(ptr) \
do {  \
    if(ptr) {  \
            delete ptr;  \
            ptr = nullptr;  \
    }\
} while(0)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 登录相关
    _loginDialog = new LoginDialog(this);
    _registerDialog = new RegisterDialog(this);
    init();
}

void MainWindow::init() {
    // 设置登录界面主窗口
    _loginDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _registerDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    connect(_loginDialog, &LoginDialog::switchRegister, this, &MainWindow::switchRegisterDlg);

    setCentralWidget(_loginDialog);
    _registerDialog->hide();
    // _loginDialog->show();
}

void MainWindow::switchRegisterDlg() {
    setCentralWidget(_registerDialog);
    _loginDialog->hide();
    _registerDialog->show();
}

MainWindow::~MainWindow()
{
    delete ui;
//    SAFE_DELETE_PTR(_loginDialog);
//    SAFE_DELETE_PTR(_registerDialog);
}

