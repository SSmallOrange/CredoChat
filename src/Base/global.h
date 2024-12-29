#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QStyle>


/**
 * @brief repolish 刷新QSS
 */
extern std::function<void(QWidget*)> repolish;

#define REPOLISH_P(widget, type, state) \
widget->setProperty(type, state);  \
repolish(widget);

#endif // GLOBAL_H
