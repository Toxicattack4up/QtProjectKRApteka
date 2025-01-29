#include "addemployeedialog.h"
#include "ui_addemployeedialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QCryptographicHash>

AddEmployeeDialog::AddEmployeeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEmployeeDialog)
{
    ui->setupUi(this);
}

AddEmployeeDialog::~AddEmployeeDialog()
{
    delete ui;
}

// Геттеры для получения введенных данных
QString AddEmployeeDialog::getName() const {
    return ui->NameLineEditAddEmployee->text();
}

QString AddEmployeeDialog::getPosition() const {
    return ui->PositionLineEditAddEmployee->text();
}

QString AddEmployeeDialog::getSalary() const {
    return ui->SalaryLineEditAddEmployee->text();
}

QString AddEmployeeDialog::getLogin() const {
    return ui->LoginLineEditAddEmployee->text();
}

QString AddEmployeeDialog::getPassword() const {
    return ui->PasswordLineEditAddEmployee->text();
}

// Нажатие кнопки "Сохранить"
void AddEmployeeDialog::on_SaveAddEmployeePushButton_clicked()
{
    QString name = getName();
    QString position = getPosition();
    QString salary = getSalary();
    QString login = getLogin();
    QString password = getPassword();

    // Проверяем, что все поля заполнены
    if (name.isEmpty() || position.isEmpty() || salary.isEmpty() || login.isEmpty() || password.isEmpty())
    {
        ui->ErrorLabelAddEmployee->setText("Ошибка, все поля должны быть заполнены!");
        return;
    }

    // Сохранение данных в JSON с хешированием пароля
    saveEmployeeToJson(name, position, salary, login, password);

    // Закрываем окно, чтобы вернуться в основное меню
    accept();
}

// Нажатие кнопки "Отмена"
void AddEmployeeDialog::on_CancelAddEmployeePushButton_clicked()
{
    qDebug() << "Кнопка отмены нажата";
    close();  // Закрыть окно без сохранения

}

// === Функция сохранения данных в JSON ===
void AddEmployeeDialog::saveEmployeeToJson(const QString &name, const QString &position, const QString &salary, const QString &login, const QString &password)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/employees.json";
    QFile file(filePath);

    QJsonArray employeesArray;
    QJsonObject root;

    // Читаем файл, если он существует
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для чтения!");
            return;
        }

        QByteArray fileData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        if (!doc.isNull() && doc.isObject()) {
            root = doc.object();
            employeesArray = root.value("employees").toArray();
        }
    }

    // Хешируем пароль
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString hashedPasswordHex = QString(hashedPassword.toHex());

    // Создаём объект сотрудника
    QJsonObject newEmployee;
    newEmployee["name"] = name;
    newEmployee["position"] = position;
    newEmployee["salary"] = salary;
    newEmployee["login"] = login;
    newEmployee["password"] = hashedPasswordHex;

    // Добавляем сотрудника
    employeesArray.append(newEmployee);
    root["employees"] = employeesArray;

    // Открываем файл для записи
    QFile writeFile(filePath);
    if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи!");
        return;
    }

    // Записываем JSON обратно
    QJsonDocument newDoc(root);
    writeFile.write(newDoc.toJson());
    writeFile.close();

    QMessageBox::information(this, "Успех", "Сотрудник успешно добавлен!");
    on_CancelAddEmployeePushButton_clicked();
}

