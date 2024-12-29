#ifndef COMMONMODULE_H
#define COMMONMODULE_H

// 检测是否使用 Visual Studio 编译器
#ifdef _MSC_VER 

// 如果定义了 COMMONMODULE_EXPORTS，则表示编译 DLL
#ifdef COMMONMODULE_EXPORTS
#define COMMONMODULE_API __declspec(dllexport)
#else
// 如果没有定义 COMMONMODULE_EXPORTS，则表示使用 DLL
#define COMMONMODULE_API __declspec(dllimport)
#endif

#else
// 如果不是 MSVC 编译器，定义为空（兼容其他平台）
#define COMMONMODULE_API
#endif

#endif // COMMONMODULE_H
