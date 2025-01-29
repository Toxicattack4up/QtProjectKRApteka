#include "mainwindow.h"
#include "addemployeedialog.h"
#include "ui_mainwindow.h"
#include "addemployeedialog.h"
#include "RemoveEmployeeDialog.h"

#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>

// Конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Подключаем кнопки к слотам
    connect(ui->LogAdminPushButton, &QPushButton::clicked, this, &MainWindow::on_LogAdminPushButton_clicked);
    connect(ui->LogSellerPushButton, &QPushButton::clicked, this, &::MainWindow::on_LogAdminPushButton_clicked);
    connect(ui->BackButton, &QPushButton::clicked, this, &MainWindow::on_BackButton_clicked);
    connect(ui->EmployeeManagement, &QPushButton::clicked, this, &MainWindow::on_EmployeeManagement_clicked);
    connect(ui->WarehouseManagement, &QPushButton::clicked, this, &MainWindow::on_WarehouseManagement_clicked);
    connect(ui->Cancel_Employee, &QPushButton::clicked, this, &MainWindow::on_Cancel_Employee_clicked);

}

// Деструктор
MainWindow::~MainWindow()
{
    delete ui;
}




// === Вспомогательные функции ===


// Загрузка JSON
QJsonObject MainWindow::loadUsers(const QString &filePath)
{
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << "Ошибка при открытии файла: " << file.errorString();
        //qDebug() << "Путь к файлу: " << fullPath;
        return QJsonObject();
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject())
    {
        qDebug() << "Ошибка: Некорректный JSON.";
        return QJsonObject();
    }

    return doc.object();
}


// Хеширование пароля
QString MainWindow::hashPassword(const QString &password)
{
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}


// Проверка логина и пароля администратора
bool MainWindow::validateAdmin(const QString &login, const QString &password, const QJsonObject &users)
{
    QJsonArray adminsArray = users.value("admins").toArray();

    for (const QJsonValue &adminValue : adminsArray)
    {
        QJsonObject adminObject = adminValue.toObject();

        QString storedLogin = adminObject.value("login").toString();
        QString storedHash = adminObject.value("password").toString();

        //qDebug() << "Сравниваем логин:" << storedLogin << "с" << login;
        //qDebug() << "Сравниваем хешированный пароль:" << hashPassword(password) << "с" << storedHash;

        if (storedLogin == login && hashPassword(password) == storedHash)
        {
            return true;
        }
    }
    return false;
}


void MainWindow::saveEmployeeToJson(const QString &name, const QString &position, const QString &salary, const QString &login, const QString &password)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/employees.json";
    QFile file(filePath);

    QJsonArray employeesArray;

    // Проверяем, существует ли файл
    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для чтения!");
            return;
        }

        QByteArray fileData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        if (!doc.isNull() && doc.isArray())
        {
            employeesArray = doc.array();
        }
    }

    // Хешируем пароль перед сохранением
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString hashedPasswordHex = QString(hashedPassword.toHex());

    // Создаём объект нового сотрудника
    QJsonObject newEmployee;
    newEmployee["name"] = name;
    newEmployee["position"] = position;
    newEmployee["salary"] = salary;
    newEmployee["login"] = login;
    newEmployee["password"] = hashedPasswordHex;  // Сохраняем хешированный пароль

    // Добавляем нового сотрудника в массив
    employeesArray.append(newEmployee);

    // Записываем JSON обратно в файл
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи!");
        return;
    }

    QJsonDocument newDoc(employeesArray);
    file.write(newDoc.toJson());
    file.close();

    QMessageBox::information(this, "Успех", "Сотрудник успешно добавлен!");
}


// Метод обновления таблицы сотрудников
void MainWindow::UpdateEmployeeTable()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/employees.json";
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл сотрудников!");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &jsonError);

    if (doc.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        QMessageBox::warning(this, "Ошибка", "Ошибка парсинга JSON: " + jsonError.errorString());
        return;
    }

    QJsonObject rootObject = doc.object();
    QJsonArray employeeArray = rootObject.value("employees").toArray();

    // Очистка таблицы перед обновлением
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"Логин", "Имя", "Должность", "Зарплата"});

    // Заголовки колонок
    QStringList headers = {"Имя", "Должность", "Зарплата", "Логин"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Заполнение таблицы
    int row = 0;
    for (const QJsonValue &value : employeeArray)
    {
        QJsonObject employee = value.toObject();

        QString login = employee.value("login").toString();
        QString name = employee.value("name").toString();
        QString position = employee.value("position").toString();
        QString salary = employee.value("salary").toString();

        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(position));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(salary));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(login));

        row++;
    }
}

// === Обработчики кнопок администратора ===


// Кнопка Вход администратора
void MainWindow::on_LogAdminPushButton_clicked()
{
    QString login = ui->LoginLineEdit->text(); // Получаем логин
    QString password = ui->PasswordLineEdit->text(); // Получаем пароль

    // Загружаем файл
    QJsonObject users = loadUsers("users.json");

    // Проверка логина и пароля
    if(validateAdmin(login, password, users))
    {
        ui->stackedWidget->setCurrentIndex(1); // Перелистываем страницу
    }
    else
    {
        ui->Operation->setText("Неверный логин или пароль");
    }
}


// Кнопка выход из выбора администратора
void MainWindow::on_BackButton_clicked()
{
    ui->LoginLineEdit->clear();
    ui->PasswordLineEdit->clear();
    ui->stackedWidget->setCurrentIndex(0);  // Переключаем на страницу входа (индекс 0)
}


// Кнопка "Управление сотрудниками"
void MainWindow::on_EmployeeManagement_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    UpdateEmployeeTable();
}


// Кнопка назад из "Управление сотрудниками"
void MainWindow::on_Cancel_Employee_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


// Кнопка "Управление складом"
void MainWindow::on_WarehouseManagement_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


// Кнопка назад из "Управление складом"
void MainWindow::on_Cancel_Warehouse_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


// Кнопка открытия окна для добавления данных о новом сотруднике
void MainWindow::on_AddEmployeePushButton_clicked()
{
    AddEmployeeDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString name = dialog.getName();
        QString position = dialog.getPosition();
        QString salary = dialog.getSalary();
        QString login = dialog.getLogin();
        QString password = dialog.getPassword();

        if (name.isEmpty() || position.isEmpty() || salary.isEmpty() || login.isEmpty() || password.isEmpty())
        {
            QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        }
        else
        {
            saveEmployeeToJson(name, position, salary, login, password);
            UpdateEmployeeTable();
        }
    }
}


// Кнопка удаление сотрудника
void MainWindow::on_RemoveEmployeePushButton_clicked()
{
    RemoveEmployee removeDialog(this);

    // Обновляем таблицу после удаления
    connect(&removeDialog, &RemoveEmployee::EmployeeDeleted, this, &MainWindow::UpdateEmployeeTable);

    removeDialog.exec();
}

