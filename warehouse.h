#ifndef WAREHOUSE_H
#define WAREHOUSE_H
#include <QString>
#include <QMap>

// Структура Medicine хранит данные о лекарстве.
struct Medicine {
    QString name;
    int quantity;
    bool requiresPrescription;
};

// Класс Warehouse отвечает за управление запасами лекарств на складе.
class Warehouse {
private:
    QMap<QString, Medicine> inventory;

public:
    void addMedicine(const QString& name, int quantity, bool requiresPrescription);
    void removeMedicine(const QString& name);
    QMap<QString, Medicine> getInventory() const;
    // Метод сохраняет текущее состояние склада в JSON-файл.
    void saveToJson(const QString& filename) const;
    void loadFromJson(const QString& filename);
};

#endif // WAREHOUSE_H
