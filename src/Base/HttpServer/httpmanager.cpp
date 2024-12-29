#include "httpmanager.h"

#include <QUrl>
#include <QObject>
#include <QString>
#include <functional>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QNetworkAccessManager>

HTTPManager::HTTPManager() {
    _httpManager = new QNetworkAccessManager;

    // -------------------- connect --------------------
    connect(this, &HTTPManager::SigHttpFinish, this, &HTTPManager::OnHandleHttpFinsih);
}

HTTPManager::~HTTPManager() {
    delete _httpManager;
}

void HTTPManager::PostHttpRequest(QUrl url, QJsonObject jsonObj, ReqId reqId, Modules mod) {
    QByteArray jsonData = QJsonDocument(jsonObj).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonData.length()));  // 长度转化为字节数组，自动按照大小端进行转换
    HTTPManager::ptr self = shared_from_this();  // 用来防止回调时对象失效
    QNetworkReply* reply = _httpManager->post(request, jsonData);  // 需要自己回收
    QObject::connect(reply, &QNetworkReply::finished, this, std::bind(&HTTPManager::OnHandleHttpPost, this, self, reply, reqId, mod));
}

// -----------------  SLOTS -----------------------
void HTTPManager::OnHandleHttpPost(HTTPManager::ptr self, QNetworkReply* reply, ReqId reqId, Modules mod) {

    reply->deleteLater();  // 该指针将在下次事件循环后被销毁

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        emit self->SigHttpFinish(reqId, "", ErrorCodes::ERR_NETWORK, mod);
        return;
    }
    QString res = reply->readAll();
    emit self->SigHttpFinish(reqId, res, ErrorCodes::SUCCESS, mod);  // 透传给其他模块
    return;
}

void HTTPManager::OnHandleHttpFinsih(ReqId reqId, QString res /* Json */, ErrorCodes errCode, Modules mod) {
    if (mod == Modules::REGISTERMOD) {
        emit SigRegisterModMsgFinish(reqId, res, errCode);
    }
}
