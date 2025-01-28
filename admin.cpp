#include "admin.h"

Admin::Admin(const QString& username, const QString& password) : User(username, password) {}

void Admin::addMedicine(Warehouse& warehouse, const QString& name, int quantity, bool requiresPrescription) {
    warehouse.addMedicine(name, quantity, requiresPrescription);
}

void Admin::removeMedicine(Warehouse& warehouse, const QString& name) {
    warehouse.removeMedicine(name);
}
