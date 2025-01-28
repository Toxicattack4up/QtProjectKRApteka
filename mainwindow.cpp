#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    connect(ui->LogSellerpushButton, &QPushButton::clicked, this, &::MainWindow::on_LogAdminPushButton_clicked);
    connect(ui->BackButton, &QPushButton::clicked, this, &::MainWindow::on_BackButton_clicked);
    connect(ui->EmployeeManagement, &QPushButton::clicked, this, &MainWindow::on_EmployeeManagement_clicked);
    connect(ui->Cancel_2, &QPushButton::clicked, this, &MainWindow::on_Cancel_2_clicked);
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

// Проверка логина и пароля
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


// === Обработчики кнопок ===

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

// Кнопка выход
void MainWindow::on_BackButton_clicked()
{
    ui->LoginLineEdit->clear();
    ui->PasswordLineEdit->clear();
    ui->stackedWidget->setCurrentIndex(0);  // Переключаем на страницу входа (индекс 0)
}

void MainWindow::on_EmployeeManagement_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_Cancel_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

