#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget> // Для работы с QStackedWidget
#include <QTableWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>



QT_BEGIN_NAMESPACE
namespace Ui {
class Apteka;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Конструктор
    MainWindow(QWidget *parent = nullptr);
    // Деструктор
    ~MainWindow();

private slots:
    // ВХОД АДМИНА
    void on_LogAdminPushButton_clicked(); // Кнопка для обработки входа
    void on_BackButton_clicked(); // Кнопка выход из меню выбора

    // УПРАВЛЕНИЕ СОТРУДНИКАМИ
    void on_EmployeeManagement_clicked(); // Кнопка входа "Управление сотрудниками"
    void on_Cancel_Employee_clicked(); // Кнопка выхода из "Управления сотрудниками"

    // МАНИПУЛЯЦИИ НАД СОТРУДНИКАМИ
    void on_AddEmployeePushButton_clicked(); // Кнопка открытия окна "Добавить сотрудник"
    void on_RemoveEmployeePushButton_clicked(); // Кнопка открытия окна "Удаление сотрудника"
    void on_SearcheEmployeePushButton_clicked(); // Кнопка поиска сотрудника

    // УПРАВЛЕНИЕ СКЛАДОМ
    void on_WarehouseManagement_clicked(); // Кнопка "Управление складом"
    void on_Cancel_Warehouse_clicked(); // Кнопка выхода из "Управления складом"



    void on_AddMedicinePushButton_clicked();

    void on_CancelPushButton_clicked();

    void on_DeleteMedicinePushButton_clicked();

    void on_SaveAddMedicinePushButton_clicked();

    void on_RemoveMedizinePushButton_clicked();

    void on_CancelDeleteMedicinePushButton_clicked();

private:
    Ui::Apteka *ui;

    // Загрузка JSON файла Администраторов
    QJsonObject loadUsers(const QString &filePath);

    // Хеширование пароля
    QString hashPassword(const QString &password);


    // === РАБОТА ФАЙЛОВ С СОТРУДНИКАМИ ===
    // Проверка введенных данных
    bool ValidateUser(const QString &login, const QString &password, const QJsonObject &users);

    // Сохранение данных о новом сотруднике
    void saveEmployeeToJson(const QString &name, const QString &position, const QString &salary, const QString &login, const QString &password);

    //Метод обновления таблицы сотрудников
    void UpdateEmployeeTable();
    // ====================================


    // === РАБОТА ФАЙЛОВ С ЛЕКАРСТВАМИ ===
    // Метод для сохранения данных о лекарстве в JSON
    void SaveMedicineToJson(const QString &name, const QString &country, bool requiresPrescription, int quantity);

    // Метод для обновления таблицы с лекарствами
    void UpdateMedicineTable();

    // Метод удаления лекарство из файла
    bool DeleteMedicineFromJson(const QString &name);

    // Метод обновления таблицы с лекарствами при удалении
    void UpdateDeleteMedicineTable(QTableWidget* tableWidget);
    // ====================================
};

#endif // MAINWINDOW_H
