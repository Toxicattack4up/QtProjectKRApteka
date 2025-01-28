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
    void on_LogAdminPushButton_clicked(); // Метод для обработки входа
    void on_BackButton_clicked(); // Кнопка выход

    void on_EmployeeManagement_clicked();

    void on_BackButton1_clicked();

    void on_Cancel_2_clicked();

private:
    Ui::MainWindow *ui;

    QJsonObject loadUsers(const QString &filePath); // Загрузка JSON файла
    QString hashPassword(const QString &password); // Хеширование пароля
    // Проверка введенных данных
    bool validateAdmin(const QString &login, const QString &password, const QJsonObject &users);
};

#endif // MAINWINDOW_H
