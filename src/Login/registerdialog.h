#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <functional>

#include "Base/HttpServer/httpmanager.h"

typedef std::unordered_map<ReqId, std::function<void(const QJsonObject&)>> MapReqIdToCallBack;

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

public:
    void showTip(QString str, bool ok = true);

private slots:
    void on_getCaptchaBtn_clicked();
    void OnHandleRegisterModuleFinish(ReqId reqId, QString res /* Json */, ErrorCodes errCode);

private: // Callback
    void callbackHandleGetVariftCode(const QJsonObject&);

private:
    void init();
    void initHandlersa();

private:
    Ui::RegisterDialog *ui;

    MapReqIdToCallBack _handles;
};

#endif // REGISTERDIALOG_H
