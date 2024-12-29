#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    init();
}

void LoginDialog::init() {
    // 跳转注册
    connect(ui->btnRegister, &QPushButton::clicked, this, &LoginDialog::switchRegister);

    // Other
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
