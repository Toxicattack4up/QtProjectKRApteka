#include "user.h"

User::User(const QString& username, const QString& password)
    : username(username), passwordHash(hashPassword(password)) {}

bool User::authorize(const QString& username, const QString& password) {
    return this->username == username && this->passwordHash == hashPassword(password);
}

QString User::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}
