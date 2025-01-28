#ifndef SELLER_H
#define SELLER_H

#include "User.h"
#include "Warehouse.h"

// Класс Seller предназначен для обработки продаж и проверки рецептов.
class Seller : public User {
public:
    Seller(const QString& username, const QString& password);
    bool sellMedicine(Warehouse& warehouse, const QString& name, int quantity, bool hasPrescription);
};

#endif // SELLER_H
