#ifndef USER_H
#define USER_H
#include <QString>
#include <QCryptographicHash>

// Класс User является базовым для всех пользователей системы.
// Он включает функционал авторизации и хэширования паролей.
class User {
protected:
    QString username;
    QString passwordHash;

public:
    User(const QString& username, const QString& password);
    bool authorize(const QString& username, const QString& password);
    // Метод возвращает SHA-256 хэш пароля для безопасного хранения.
    static QString hashPassword(const QString& password);
};

#endif // USER_H
