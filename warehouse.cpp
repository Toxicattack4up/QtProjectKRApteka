#include "warehouse.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

void Warehouse::addMedicine(const QString& name, int quantity, bool requiresPrescription) {
    if (inventory.contains(name)) {
        inventory[name].quantity += quantity;
    } else {
        inventory[name] = {name, quantity, requiresPrescription};
    }
}

void Warehouse::removeMedicine(const QString& name) {
    if (inventory.contains(name)) {
        inventory.remove(name);
    }
}

QMap<QString, Medicine> Warehouse::getInventory() const {
    return inventory;
}

void Warehouse::saveToJson(const QString& filename) const {
    QJsonArray jsonArray;
    for (const auto& key : inventory.keys()) {
        const auto& medicine = inventory[key];
        QJsonObject obj;
        obj["name"] = medicine.name;
        obj["quantity"] = medicine.quantity;
        obj["requiresPrescription"] = medicine.requiresPrescription;
        jsonArray.append(obj);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(jsonArray);
        file.write(doc.toJson());
        file.close();
    }
}

void Warehouse::loadFromJson(const QString& filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray jsonArray = doc.array();
        for (const auto& value : jsonArray) {
            QJsonObject obj = value.toObject();
            addMedicine(
                obj["name"].toString(),
                obj["quantity"].toInt(),
                obj["requiresPrescription"].toBool()
                );
        }
        file.close();
    }
}
