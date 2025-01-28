#ifndef ADMIN_H
#define ADMIN_H
#include "User.h"
#include "Warehouse.h"

// Класс Admin предназначен для управления складом и сотрудниками.
class Admin : public User {
public:
    Admin(const QString& username, const QString& password);
    void addMedicine(Warehouse& warehouse, const QString& name, int quantity, bool requiresPrescription);
    void removeMedicine(Warehouse& warehouse, const QString& name);
};

#endif // ADMIN_H
