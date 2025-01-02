/******************************************************************************
 *
 * @file       httpmanager.h
 * @brief      HTTP Singleton  CRTP 奇异递归模板
 *
 * @author     Orange
 * @date       2024/10/15
 * @history
 *****************************************************************************/
#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>

#include "Utils/singleton.h"

class QUrl;
class QString;
class QJsonObject;
class QNetworkReply;
class QNetworkAccessManager;

enum ReqId {
    ID_GET_VARIFY_CODE = 1001,  // 获取验证码
    ID_REG_USER = 1002,         // 注册用户
};

enum Modules {
    REGISTERMOD = 0,
};

enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1,               // JSON ERR
    ERR_NETWORK = 2,            // NETWORK ERR
};

class HTTPManager : public QObject, public Singleton<HTTPManager>, public std::enable_shared_from_this<HTTPManager> {  // 单例有点难用啊，不能开箱即用
    Q_OBJECT
public:
    typedef std::shared_ptr<HTTPManager> ptr;

    ~HTTPManager();  // 方便单例析构时调用析构函数

public:
    void PostHttpRequest(QUrl url, QJsonObject jsonObj, ReqId reqId, Modules mod);

private:
    friend class Singleton<HTTPManager>;  // 方便单例调用构造函数进行构造
    HTTPManager();

signals:
    void SigHttpFinish(ReqId reqId, QString res /* Json */, ErrorCodes errCode, Modules mod);
    void SigRegisterModMsgFinish(ReqId reqId, QString res /* Json */, ErrorCodes errCode);

private slots:
    void OnHandleHttpPost(HTTPManager::ptr self, QNetworkReply* reply, ReqId reqId, Modules mod);
    void OnHandleHttpFinsih(ReqId reqId, QString res /* Json */, ErrorCodes errCode, Modules mod);

private:
    QNetworkAccessManager* _httpManager;
};

#endif // HTTPMANAGER_H
