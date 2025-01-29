#include "mainwindow.h"
#include "addemployeedialog.h"
#include "ui_mainwindow.h"
#include "addemployeedialog.h"
#include "RemoveEemployeeDialog.h"

#include <QMessageBox>
#include <QDialog>
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
    , ui(new Ui::Apteka)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    ui->SearcheEmployeeLineEdit->setPlaceholderText("Введите имя, логин, должность или зарплату сотрудника для поиска...");
    ui->DeleteMedicineLineEdit->setPlaceholderText("Введите название лекарства для удаления...");

    // Подключаем кнопки к слотам
    connect(ui->LogAdminPushButton, &QPushButton::clicked, this, &MainWindow::on_LogAdminPushButton_clicked);

    connect(ui->BackButton, &QPushButton::clicked, this, &MainWindow::on_BackButton_clicked);

    connect(ui->EmployeeManagement, &QPushButton::clicked, this, &MainWindow::on_EmployeeManagement_clicked);
    connect(ui->Cancel_Employee, &QPushButton::clicked, this, &MainWindow::on_Cancel_Employee_clicked);

    connect(ui->WarehouseManagement, &QPushButton::clicked, this, &MainWindow::on_WarehouseManagement_clicked);
    connect(ui->AddMedicinePushButton, &QPushButton::clicked, this, &MainWindow::on_AddMedicinePushButton_clicked);
    connect(ui->CancelPushButton, &QPushButton::clicked, this, &MainWindow::on_CancelPushButton_clicked);

    connect(ui->RemoveMedizinePushButton, &QPushButton::clicked, this, &MainWindow::on_RemoveMedizinePushButton_clicked);
}

// Деструктор
MainWindow::~MainWindow()
{
    delete ui;
}


// === ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ===

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

// Проверка логина и пароля сотрудника
bool MainWindow::ValidateUser(const QString &login, const QString &password, const QJsonObject &users)
{
    // Проверяем в списке администраторов
    QJsonArray adminsArray = users.value("admins").toArray();
    for (const QJsonValue &adminValue : adminsArray)
    {
        QJsonObject adminObject = adminValue.toObject();
        if (adminObject["login"].toString() == login &&
            adminObject["password"].toString() == hashPassword(password))
        {
            return true;
        }
    }

    // Проверяем в списке сотрудников
    QJsonArray employeesArray = users.value("employees").toArray();
    for (const QJsonValue &employeeValue : employeesArray)
    {
        QJsonObject employeeObject = employeeValue.toObject();
        if (employeeObject["login"].toString() == login &&
            employeeObject["password"].toString() == hashPassword(password))
        {
            return true; // Вход как сотрудник
        }
    }

    return false; // Пользователь не найден
}

// Сохранение данных о новом сотруднике
void MainWindow::saveEmployeeToJson(const QString &name, const QString &position, const QString &salary, const QString &login, const QString &password)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/users.json";
    QFile file(filePath);

    QJsonObject rootObj;
    QJsonArray employeesArray;

    // Загружаем существующий JSON, если файл есть
    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QByteArray fileData = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(fileData);
            if (!doc.isNull() && doc.isObject())
            {
                rootObj = doc.object();
                employeesArray = rootObj["employees"].toArray();
            }
        }
    }

    // Проверяем, существует ли логин
    for (const QJsonValue &value : employeesArray)
    {
        if (value.toObject()["login"].toString() == login)
        {
            QMessageBox::warning(this, "Ошибка", "Пользователь с таким логином уже существует!");
            return;
        }
    }

    // Хешируем пароль
    QString hashedPassword = hashPassword(password);

    // Создаем объект нового сотрудника
    QJsonObject newEmployee;
    newEmployee["name"] = name;
    newEmployee["position"] = position;
    newEmployee["salary"] = salary;
    newEmployee["login"] = login;
    newEmployee["password"] = hashedPassword;

    // Добавляем нового сотрудника
    employeesArray.append(newEmployee);

    // Обновляем JSON-объект
    rootObj["employees"] = employeesArray;

    // Записываем в файл
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QJsonDocument newDoc(rootObj);
        file.write(newDoc.toJson());
        file.close();
    }

    QMessageBox::information(this, "Успех", "Сотрудник успешно добавлен!");
}

// Метод обновления таблицы сотрудников
void MainWindow::UpdateEmployeeTable()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/users.json";
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

    // Заголовки колонок
    QStringList headers = {"Имя", "Должность", "Зарплата", "Логин"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Заполнение таблицы
    int row = 0;
    for (const QJsonValue &value : employeeArray)
    {
        QJsonObject employee = value.toObject();

        QString name = employee.value("name").toString();
        QString position = employee.value("position").toString();
        QString salary = employee.value("salary").toString();
        QString login = employee.value("login").toString();

        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(position));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(salary));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(login));

        row++;
    }
}


// === ДОБАВЛЕНИЕ ЛЕКАРСТВА В ФАЙЛЫ ===

// Метод сохранения файлов в JSON
void MainWindow::SaveMedicineToJson(const QString &name, const QString &country, bool requiresPrescription, int quantity)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/medicines.json";
    QFile file(filePath);

    // Создаем пустой массив для лекарств, если файл еще не существует
    QJsonArray medicinesArray;

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
        if (!doc.isNull() && doc.isObject())
        {
            QJsonObject rootObj = doc.object();
            if (rootObj.contains("medicines")) {
                medicinesArray = rootObj["medicines"].toArray();
            }
        }
    }

    // Создаем объект нового лекарства
    QJsonObject newMedicine;
    newMedicine["name"] = name;
    newMedicine["country"] = country;
    newMedicine["requiresPrescription"] = requiresPrescription;
    newMedicine["quantity"] = quantity;

    // Добавляем новое лекарство в массив
    medicinesArray.append(newMedicine);

    // Записываем JSON обратно в файл
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи!");
        return;
    }

    // Создаем корневой объект, если его нет
    QJsonObject rootObj;
    rootObj["medicines"] = medicinesArray;

    // Создаем документ с новым объектом и записываем его в файл
    QJsonDocument newDoc(rootObj);
    file.write(newDoc.toJson());
    file.close();

    QMessageBox::information(this, "Успех", "Лекарство успешно добавлено!");
}

// Метод обновления файла JSON
void MainWindow::UpdateMedicineTable()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/medicines.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл с лекарствами!");
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
    QJsonArray medicineArray = rootObject.value("medicines").toArray();

    // Очистка таблицы перед обновлением
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);

    // Заголовки колонок
    QStringList headers = {"Название", "Страна", "Рецепт", "Количество"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Заполнение таблицы
    int row = 0;
    for (const QJsonValue &value : medicineArray)
    {
        QJsonObject medicine = value.toObject();

        QString name = medicine.value("name").toString();
        QString country = medicine.value("country").toString();
        bool requiresPrescription = medicine.value("requiresPrescription").toBool();
        int quantity = medicine.value("quantity").toInt();

        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(country));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(requiresPrescription ? "Да" : "Нет"));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(quantity)));

        row++;
    }
}

// Функцию для удаления лекарства
bool MainWindow::DeleteMedicineFromJson(const QString &name)
{
    QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название лекарства для удаления!");
        return false;
    }

    QString filePath = QCoreApplication::applicationDirPath() + "/medicines.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл с лекарствами!");
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &jsonError);

    if (doc.isNull() || jsonError.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Ошибка", "Ошибка парсинга JSON: " + jsonError.errorString());
        return false;
    }

    QJsonObject rootObject = doc.object();
    QJsonArray medicineArray = rootObject.value("medicines").toArray();
    bool found = false;

    // Создаем новый массив без удаляемого лекарства
    QJsonArray newMedicineArray;
    for (const QJsonValue &value : medicineArray) {
        QJsonObject medicine = value.toObject();
        if (medicine["name"].toString().compare(trimmedName, Qt::CaseInsensitive) != 0) {
            newMedicineArray.append(medicine);
        } else {
            found = true;
        }
    }

    if (!found) {
        return false;  // Лекарство не найдено
    }

    // Перезаписываем файл с обновленным списком
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи!");
        return false;
    }

    rootObject["medicines"] = newMedicineArray;
    QJsonDocument newDoc(rootObject);
    file.write(newDoc.toJson());
    file.close();

    return true;

}

// Функция для обновления таблицы
void MainWindow::UpdateDeleteMedicineTable(QTableWidget* tableWidget)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/medicines.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл с лекарствами!");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    QJsonObject rootObject = doc.object();
    QJsonArray medicinesArray = rootObject.value("medicines").toArray();

    // Очищаем таблицу перед обновлением
    tableWidget->setRowCount(0);
    tableWidget->clear();
    tableWidget->setColumnCount(4);

    // Устанавливаем заголовки столбцов
    QStringList headers = {"Название", "Страна", "Рецепт", "Количество"};
    tableWidget->setHorizontalHeaderLabels(headers);

    if (medicinesArray.isEmpty())
    {
        QMessageBox::information(this, "Информация", "Список лекарств пуст!");
        return;
    }

    // Заполняем таблицу актуальными данными
    int row = 0;
    for (const QJsonValue &value : medicinesArray)
    {
        QJsonObject medicine = value.toObject();

        QString name = medicine.value("name").toString();
        QString country = medicine.value("country").toString();
        QString requiresPrescription = medicine.value("requiresPrescription").toBool() ? "Да" : "Нет";
        int quantity = medicine.value("quantity").toInt();

        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(name));
        tableWidget->setItem(row, 1, new QTableWidgetItem(country));
        tableWidget->setItem(row, 2, new QTableWidgetItem(requiresPrescription));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(quantity)));

        row++;
    }

    // Автоматическое подстраивание ширины столбцов
    tableWidget->resizeColumnsToContents();
}



// === ОБРАБОТКА КНОПОК СОТРУДНИКА ===

// Кнопка "Вход"
void MainWindow::on_LogAdminPushButton_clicked()
{
    QString login = ui->LoginLineEdit->text().trimmed(); // Получаем логин
    QString password = ui->PasswordLineEdit->text().trimmed(); // Получаем пароль

    // Загружаем JSON-файл с пользователями
    QJsonObject users = loadUsers("users.json");

    if (users.isEmpty()) {
        ui->Operation->setText("Ошибка загрузки пользователей.");
        return;
    }

    // Проверка сотрудника
    if (ValidateUser(login, password, users)) {
        ui->stackedWidget->setCurrentIndex(1); // Переход на страницу пользователя
        ui->Operation->setText("Вход выполнен: Сотрудник");
        return;
    }

    // Ошибка авторизации
    ui->Operation->setText("Неверный логин или пароль");
}

// Кнопка "Выход" из выбора
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

// Кнопка "Назад" из "Управление сотрудниками"
void MainWindow::on_Cancel_Employee_clicked()
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

// Кнопка поиска сотрудника по имени, логину, зарплате или должности
void MainWindow::on_SearcheEmployeePushButton_clicked()
{
    QString searchText = ui->SearcheEmployeeLineEdit->text().trimmed();

    if (searchText.isEmpty()) {
        UpdateEmployeeTable(); // Если строка поиска пуста, показываем всех сотрудников
        return;
    }

    QString filePath = QCoreApplication::applicationDirPath() + "/employees.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл сотрудников.";
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Ошибка загрузки JSON.";
        return;
    }

    QJsonObject rootObj = doc.object();
    QJsonArray employeesArray = rootObj["employees"].toArray(); // Получаем массив сотрудников

    ui->tableWidget->setRowCount(0); // Очищаем таблицу перед добавлением новых данных

    for (const QJsonValue &value : employeesArray)
    {
        QJsonObject obj = value.toObject();

        QString name = obj["name"].toString();
        QString position = obj["position"].toString();
        QString salary = obj["salary"].toString();
        QString login = obj["login"].toString();

        // Проверяем, содержит ли поле введённый текст
        if (name.contains(searchText, Qt::CaseInsensitive) ||
            login.contains(searchText, Qt::CaseInsensitive) ||
            position.contains(searchText, Qt::CaseInsensitive) ||
            salary.contains(searchText, Qt::CaseInsensitive)) {

            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);

            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(position));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(salary));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(login));
        }
    }
}



// === ОБРАБОТКА КНОПОК ДЛЯ УПРАВЛЕНИЯ СКЛАДОМ ===

// Кнопка "Управление складом"
void MainWindow::on_WarehouseManagement_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    UpdateDeleteMedicineTable(ui->tableWidget_2);
}

// Кнопка "Назад" из "Управление складом"
void MainWindow::on_Cancel_Warehouse_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// Кнопка перехода в меню "Добавить лекарство"
void MainWindow::on_AddMedicinePushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);

}

// Кнопка "Отменить" для выхода из меню "Добавить лекарство"
void MainWindow::on_CancelPushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    UpdateDeleteMedicineTable(ui->tableWidget_2);
}

// Кнопка "Сохранить" для добавления лекартсва в файл JSON
void MainWindow::on_SaveAddMedicinePushButton_clicked()
{
    QString name = ui->NameMedicineLineEdit->text();
    QString country = ui->CountryLineEdit->text();
    bool requiresPrescription = ui->RecipeRadioButtonYes->isChecked();
    int quantity = ui->QuantitySpinBox->value();

    // Проверка на пустые поля
    if (name.isEmpty() || country.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }

    // Сохраняем новое лекарство в JSON
    SaveMedicineToJson(name, country, requiresPrescription, quantity);

    ui->NameMedicineLineEdit->clear();
    ui->CountryLineEdit->clear();
    ui->RecipeRadioButtonYes->setChecked(false);  // Снимаем выбор с радиокнопки
    ui->RecipeRadioButtonNo->setChecked(false);   // Если есть другая радиокнопка, также сбрасываем
    ui->QuantitySpinBox->setValue(0);

    QMessageBox::information(this, "Успех", "Лекарство добавлено!");

}

// Кнопка "Удалить" в меню "Удалить лекарство"
void MainWindow::on_DeleteMedicinePushButton_clicked()
{
    QString name = ui->DeleteMedicineLineEdit->text();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название лекарства для удаления!");
        return;
    }

    // Передаем `name` в `DeleteMedicineFromJson()`
    if (DeleteMedicineFromJson(name))
    {
        QMessageBox::information(this, "Успех", "Лекарство удалено!");

        // Очищаем поле ввода
        ui->DeleteMedicineLineEdit->clear();

        // Обновляем таблицу
        UpdateDeleteMedicineTable(ui->DeleteMedicineTableWidget);
    }
    else
    {
        QMessageBox::warning(this, "Ошибка", "Лекарство не найдено!");
    }
}

// Кнопка "Удалить лекарство" для перехода к меню удаления
void MainWindow::on_RemoveMedizinePushButton_clicked()
{
    UpdateDeleteMedicineTable(ui->DeleteMedicineTableWidget);
    ui->stackedWidget->setCurrentIndex(5);
}

// Кнопка "Назад" из меню "Удалить лекарство"
void MainWindow::on_CancelDeleteMedicinePushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    UpdateDeleteMedicineTable(ui->tableWidget_2);
}

