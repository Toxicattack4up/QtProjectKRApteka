#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget> // Для работы с QStackedWidget
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
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
    void on_LogAdminPushButton_clicked(); // Кнопка для обработки входа
    void on_BackButton_clicked(); // Кнопка выход из меню выбора


    void on_EmployeeManagement_clicked(); // Кнопка входа "Управление сотрудниками"
    void on_Cancel_Employee_clicked(); // Кнопка выхода из "Управления сотрудниками"


    void on_WarehouseManagement_clicked(); // Кнопка "Управление складом"
    void on_Cancel_Warehouse_clicked(); // Кнопка выхода из "Управления складом"


    void on_AddEmployeePushButton_clicked(); // Кнопка открытия окна "Добавить сотрудник"






    void on_RemoveEmployeePushButton_clicked();

private:
    Ui::MainWindow *ui;

    // Загрузка JSON файла Администраторов
    QJsonObject loadUsers(const QString &filePath);

    // Хеширование пароля
    QString hashPassword(const QString &password);

    // Проверка введенных данных
    bool validateAdmin(const QString &login, const QString &password, const QJsonObject &users);

    // Сохранение данных о новом сотруднике
    void saveEmployeeToJson(const QString &name, const QString &position, const QString &salary, const QString &login, const QString &password);

    //Метод обновления таблицы сотрудников
    void UpdateEmployeeTable();


};

#endif // MAINWINDOW_H
