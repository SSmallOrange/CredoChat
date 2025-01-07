#pragma once

#include <mutex>
#include <memory>
#include <iostream>

template <typename T>
class Singleton {
    typedef std::shared_ptr<T> ptr;
public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag sFlag;
        std::call_once(sFlag, [&]() {
            // 因为这里的构造函数是protected的,make_shared无法访问受保护的构造函数,而new可以在类的内部直接调用
            _instance = std::shared_ptr<T>(new T);
            });
        return _instance;
    }

    void printAddress() {
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "This is Singleton destruct." << std::endl;
    }

protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = default;
    Singleton& operator=(const Singleton<T>&) = default;

private:
    static ptr _instance;
};

// 静态成员初始化时需要指定模板参数 T
template <typename T>
typename Singleton<T>::ptr Singleton<T>::_instance = nullptr;

