#include "seller.h"

Seller::Seller(const QString& username, const QString& password) : User(username, password) {}

bool Seller::sellMedicine(Warehouse& warehouse, const QString& name, int quantity, bool hasPrescription) {
    QMap<QString, Medicine> inventory = warehouse.getInventory();

    if (!inventory.contains(name)) return false;

    Medicine& medicine = inventory[name];

    if (medicine.requiresPrescription && !hasPrescription) return false;

    if (medicine.quantity >= quantity) {
        medicine.quantity -= quantity;
        return true;
    }
    return false;
}
