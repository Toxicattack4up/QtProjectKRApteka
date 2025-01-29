#include "addemployeedialog.h"
#include "ui_addemployeedialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QCryptographicHash>


// Конструктор
AddEmployeeDialog::AddEmployeeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEmployeeDialog)
{
    ui->setupUi(this);
}

// Деструктор
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

// Нажатие кнопки "Сохранить" c оправкой данных в основную функцию сохранения
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

    // Посылаем сигнал с данными
    emit employeeAdded(name, position, salary, login, password);

    // Закрываем окно
    accept();
}

// Нажатие кнопки "Отмена"
void AddEmployeeDialog::on_CancelAddEmployeePushButton_clicked()
{
    //qDebug() << "Кнопка отмены нажата";
    close();  // Закрыть окно без сохранения

}
