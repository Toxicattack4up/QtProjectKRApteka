#include "RemoveEemployeeDialog.h"
#include "ui_RemoveEmployeeDialog.h"

// Конструктор
RemoveEmployee::RemoveEmployee(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RemoveEmployee)
{
    ui->setupUi(this);
    LoadEmployees();
    ui->RemoveLineEdit->setPlaceholderText("Выберите сотрудника");
}

// Деструктор
RemoveEmployee::~RemoveEmployee()
{
    delete ui;
}

// Загрузка списка сотрудников
void RemoveEmployee::LoadEmployees()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/employees.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть JSON сотрудников!");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный JSON!");
        return;
    }

    QJsonArray employeesArray = doc.object().value("employees").toArray();

    ui->comboBoxEmployees->clear();
    for (const QJsonValue &value : employeesArray) {
        QJsonObject employee = value.toObject();
        ui->comboBoxEmployees->addItem(employee.value("login").toString());
    }
}

// Добавление в ComboBox полученных логинов из файла
void RemoveEmployee::on_comboBoxEmployees_currentIndexChanged(const QString &text)
{
    ui->RemoveLineEdit->setText(text);
}

// Метод удаления сотрудника
void RemoveEmployee::DeleteEmployee(const QString &login)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/employees.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть JSON!");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный JSON!");
        return;
    }

    QJsonObject rootObject = doc.object();
    QJsonArray employeesArray = rootObject.value("employees").toArray();

    bool removed = false;
    for (int i = 0; i < employeesArray.size(); ++i) {
        if (employeesArray[i].toObject().value("login").toString() == login) {
            employeesArray.removeAt(i);
            removed = true;
            break;
        }
    }

    if (!removed) {
        QMessageBox::warning(this, "Ошибка", "Сотрудник не найден!");
        return;
    }

    rootObject["employees"] = employeesArray;
    QJsonDocument newDoc(rootObject);

    // Открываем заново для записи
    QFile outFile(filePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось записать JSON!");
        return;
    }

    outFile.write(newDoc.toJson());
    outFile.close();

    QMessageBox::information(this, "Успех", "Сотрудник удалён!");
    emit EmployeeDeleted();

    LoadEmployees(); // Обновляем список сотрудников
    this->accept();
}

// Нажатие на кнопку "Удалить"
void RemoveEmployee::on_RemoveEmployeesPushButton_clicked()
{
    QString login = ui->RemoveLineEdit->text().trimmed();
    if (login.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин сотрудника!");
        return;
    }

    int reply = QMessageBox::question(this, "Подтверждение",
                                      "Удалить сотрудника " + login + "?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        DeleteEmployee(login);
    }
}

// Нажатие на кнопку "Отмена"
void RemoveEmployee::on_CancelInManagementPage3PushButton_clicked()
{
    this->reject();
}
