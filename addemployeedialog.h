#ifndef ADDEMPLOYEEDIALOG_H
#define ADDEMPLOYEEDIALOG_H

#include <QDialog>


namespace Ui {
class AddEmployeeDialog;
}

class AddEmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEmployeeDialog(QWidget *parent = nullptr);
    ~AddEmployeeDialog();

    QString getName() const;
    QString getPosition() const;
    QString getSalary() const;
    QString getLogin() const;
    QString getPassword() const;

private slots:
    // Кнопка "Отмена"
    void on_CancelAddEmployeePushButton_clicked();

    // Кнопка "Сохранить"
    void on_SaveAddEmployeePushButton_clicked();

signals:
    void employeeAdded(const QString &name, const QString &position, const QString &salary, const QString &login, const QString &password);

private:
    Ui::AddEmployeeDialog *ui;
};

#endif // ADDEMPLOYEEDIALOG_H
