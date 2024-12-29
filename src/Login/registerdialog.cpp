#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "Base/global.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <unordered_map>
#include <QRegularExpression>

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::init() {
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->okEdit->setEchoMode(QLineEdit::Password);

    ui->errorTipLabel->setProperty("state", "normal");
    repolish(ui->errorTipLabel);

    // connect
    connect(HTTPManager::GetInstance().get(), &HTTPManager::SigRegisterModMsgFinish, this,  &RegisterDialog::OnHandleRegisterModuleFinish);

    // init
    initHandlersa();
}

void RegisterDialog::initHandlersa() {
    // Callback to get varify
    // _handles.insert(ReqId::ID_GET_VARIFY_CODE, std::bind(&RegisterDialog::callbackHandleGetVariftCode, this, std::placeholders::_1));
    _handles[ReqId::ID_GET_VARIFY_CODE] = std::bind(&RegisterDialog::callbackHandleGetVariftCode, this, std::placeholders::_1);
}

void RegisterDialog::showTip(QString str, bool ok) {
    if (ok) {
        REPOLISH_P(ui->errorTipLabel, "state", "normal");
    } else {
        REPOLISH_P(ui->errorTipLabel, "state", "error");
    }
    ui->errorTipLabel->setText(str);
}

// -------------------------  Slot  -------------------------

void RegisterDialog::on_getCaptchaBtn_clicked()
{
    auto email = ui->emailEdit->text();
    QRegularExpression regex("R((\w+)(\.|_)?(\w*)@(\w+)(\.(\w-))+)");
    bool match = regex.match(email).hasMatch();
    if (regex.match(email).hasMatch()) {
        // TODO Send Http Captcha

    } else {
        // showTip(tr("邮箱地址不正确"), false);
      // showTip(tr("邮箱地址不正确"), false);
        showTip(tr("Email Address Error"), false);

    }
}

void RegisterDialog::OnHandleRegisterModuleFinish(ReqId reqId, QString res /* Json */, ErrorCodes errCode) {
    if (errCode != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        return;
    }
    //Json解析
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());  // UTF8可以兼容中文
    if (jsonDoc.isNull() || jsonDoc.isObject()) {
        showTip(tr("Json解析失败"), false);
        return;
    }
    _handles[reqId](jsonDoc.object());  // callback
    return;
}

// -------------------------  CallBack Function  -------------------------
void RegisterDialog::callbackHandleGetVariftCode(const QJsonObject& jsonObj) {
    int error = jsonObj["error"].toInt();
    if (error != ErrorCodes::SUCCESS) {
        showTip(tr("参数错误"), false);
        return;
    }
    QString emailStr = jsonObj["email"].toString();
    showTip(tr("Email Send Success"), true);
    qDebug() << "email is " << emailStr;
}











