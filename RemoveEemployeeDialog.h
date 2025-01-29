#ifndef REMOVEEEMPLOYEEDIALOG_H
#define REMOVEEEMPLOYEEDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>

namespace Ui {
class RemoveEmployee;
}

class RemoveEmployee : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveEmployee(QWidget *parent = nullptr);
    ~RemoveEmployee();

signals:
    void EmployeeDeleted();

private slots:
    void on_RemoveEmployeesPushButton_clicked(); // Нажатие на кнопку "Удалить"
    void on_CancelInManagementPage3PushButton_clicked();  // Закрытие окна
    void on_comboBoxEmployees_currentIndexChanged(const QString &text);  // Заполнение поля ввода combobox

private:
    Ui::RemoveEmployee *ui;

    void LoadEmployees();
    void DeleteEmployee(const QString &login);
};

#endif // REMOVEEEMPLOYEEDIALOG_H
