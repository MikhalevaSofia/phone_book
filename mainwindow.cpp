#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QInputDialog>
#include "MainWindow.h"
#include <iostream>
#include <regex>
#include <iomanip>
#include<sstream>

class InputValidator {
public:
    static bool ValidateName(const std::string& name) {
        std::string trimmedName = std::regex_replace(name, std::regex("^\\s+|\\s+$"), "");
        std::regex nameRegex(R"(^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё\s-]*[A-Za-zА-Яа-яЁё]$)");
        return std::regex_match(trimmedName, nameRegex);
    }

    static bool ValidateDate(const std::string& date) {
        // Проверка на формат даты: dd-mm-yyyy
           std::regex dateRegex(R"(\d{2}-\d{2}-\d{4})");
           if (!std::regex_match(date, dateRegex)) {
               return false;
           }

           int day, month, year;

           // Разделяем строку вручную, по разделителю '-'
           size_t firstDash = date.find('-');
           size_t secondDash = date.find('-', firstDash + 1);

           if (firstDash == std::string::npos || secondDash == std::string::npos) {
               return false; // Невозможно найти разделители
           }

           // Извлекаем день, месяц и год как подстроки
           std::string dayStr = date.substr(0, firstDash);
           std::string monthStr = date.substr(firstDash + 1, secondDash - firstDash - 1);
           std::string yearStr = date.substr(secondDash + 1);

           // Преобразуем строки в числа
           try {
               day = std::stoi(dayStr);
               month = std::stoi(monthStr);
               year = std::stoi(yearStr);
           } catch (const std::invalid_argument& e) {
               return false; // Некорректный формат числа
           }

           // Проверка на корректность дня, месяца и года
           if (month < 1 || month > 12 || day < 1 || day > 31) {
               return false;
           }

           // Проверка на количество дней в месяце
           if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
               return false;
           }
           if (month == 2) {
               // Проверка на високосный год
               bool leapYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
               if ((leapYear && day > 29) || (!leapYear && day > 28)) {
                   return false;
               }
           }

           // Проверка на корректность года
           if (year < 1900 || year > 2100) {
               return false;
           }

           // Получаем текущую дату
           std::time_t now = std::time(nullptr);
           std::tm currentTime = *std::localtime(&now);

           // Проверка, чтобы дата не была в будущем
           if (year > currentTime.tm_year + 1900) {
               return false;
           } else if (year == currentTime.tm_year + 1900) {
               if (month > currentTime.tm_mon + 1) {
                   return false;
               } else if (month == currentTime.tm_mon + 1) {
                   if (day > currentTime.tm_mday) {
                       return false;
                   }
               }
           }

           return true;
    }

    static bool ValidateEmail(const std::string& email) {
        std::string trimmedEmail = std::regex_replace(email, std::regex("\\s+"), "");
        std::regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return std::regex_match(trimmedEmail, emailRegex);
    }

    static bool ValidatePhone(const std::string& phone) {
        std::regex phoneRegex(R"((\+?[78])?(\(?\d{3}\)?[\s\-]?)?\d{3}[\s\-]?\d{2}[\s\-]?\d{2})");
        return std::regex_match(phone, phoneRegex);
    }
};

// ContactEntry implementation
ContactEntry::ContactEntry(const std::string& fName, const std::string& mName,
                           const std::string& lName, const std::string& dob,
                           const std::string& mail, const std::string& phoneNum)
    : firstName(fName), middleName(mName), lastName(lName),
      dateOfBirth(dob), email(mail), phone(phoneNum) {}

void ContactEntry::Display() const {
    std::cout << "Name: " << firstName << " " << middleName << " " << lastName
              << "\nDate of Birth: " << dateOfBirth
              << "\nEmail: " << email
              << "\nPhone: " << phone << "\n";
}

const std::string& ContactEntry::GetFirstName() const { return firstName; }
const std::string& ContactEntry::GetMiddleName() const { return middleName; }
const std::string& ContactEntry::GetLastName() const { return lastName; }
const std::string& ContactEntry::GetDateOfBirth() const { return dateOfBirth; }
const std::string& ContactEntry::GetEmail() const { return email; }
const std::string& ContactEntry::GetPhone() const { return phone; }

// DataHandler implementation
void DataHandler::AddEntry(const ContactEntry& entry) {
    entries.push_back(entry);
}

void DataHandler::RemoveEntry(int index) {
    if (index >= 0 && index < entries.size()) {
        entries.erase(entries.begin() + index);
    }
}

void DataHandler::Sort(const std::string& field) {
    if (field == "firstName") {
        std::sort(entries.begin(), entries.end(), [](const ContactEntry& a, const ContactEntry& b) {
            return a.GetFirstName() < b.GetFirstName();
        });
    } else if (field == "lastName") {
        std::sort(entries.begin(), entries.end(), [](const ContactEntry& a, const ContactEntry& b) {
            return a.GetLastName() < b.GetLastName();
        });
    } else if (field == "dob") {
        std::sort(entries.begin(), entries.end(), [](const ContactEntry& a, const ContactEntry& b) {
            return a.GetDateOfBirth() < b.GetDateOfBirth();
        });
    }
}

std::vector<ContactEntry> DataHandler::Search(const std::string& keyword) const {
    std::vector<ContactEntry> results;
    for (const auto& entry : entries) {
        if (entry.GetFirstName().find(keyword) != std::string::npos ||
            entry.GetMiddleName().find(keyword) != std::string::npos ||
            entry.GetLastName().find(keyword) != std::string::npos ||
            entry.GetEmail().find(keyword) != std::string::npos ||
            entry.GetPhone().find(keyword) != std::string::npos) {
            results.push_back(entry);
        }
    }
    return results;
}

void DataHandler::Save(const std::string& filePath) const {
    QFile file(QString::fromStdString(filePath));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& entry : entries) {
            out << QString::fromStdString(entry.GetFirstName()) << ","
                << QString::fromStdString(entry.GetMiddleName()) << ","
                << QString::fromStdString(entry.GetLastName()) << ","
                << QString::fromStdString(entry.GetDateOfBirth()) << ","
                << QString::fromStdString(entry.GetEmail()) << ","
                << QString::fromStdString(entry.GetPhone()) << "\n";
        }
        file.close();
    }
}

void DataHandler::Load(const std::string& filePath) {
    QFile file(QString::fromStdString(filePath));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            if (fields.size() == 6) {
                entries.emplace_back(fields[0].toStdString(), fields[1].toStdString(),
                                     fields[2].toStdString(), fields[3].toStdString(),
                                     fields[4].toStdString(), fields[5].toStdString());
            }
        }
        file.close();
    }
}

void DataHandler::SaveToDatabase() {


    // Подключение к базе данных SQLite
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("contacts.db");

    // Проверка открытия базы данных
    if (!db.open()) {
        qDebug() << "Error: Unable to connect to database";
        return;
    }



    // Создание таблицы, если она не существует
    QSqlQuery createTableQuery;
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            firstName TEXT NOT NULL,
            middleName TEXT,
            lastName TEXT NOT NULL,
            dob TEXT,
            email TEXT,
            phone TEXT
        )
    )";

    if (!createTableQuery.exec(createTableSQL)) {
        qDebug() << "Error creating table: " << createTableQuery.lastError().text();
        db.close();
        return;
    } else {
        qDebug() << "Table 'contacts' is ready.";
    }

    // Подготовка запроса для добавления записей
    QSqlQuery query;
    query.prepare("INSERT INTO contacts (firstName, middleName, lastName, dob, email, phone) VALUES (?, ?, ?, ?, ?, ?)");

    // Добавление данных из списка `entries`
    for (const auto& entry : entries) {
        query.addBindValue(QString::fromStdString(entry.GetFirstName()));
        query.addBindValue(QString::fromStdString(entry.GetMiddleName()));
        query.addBindValue(QString::fromStdString(entry.GetLastName()));
        query.addBindValue(QString::fromStdString(entry.GetDateOfBirth()));
        query.addBindValue(QString::fromStdString(entry.GetEmail()));
        query.addBindValue(QString::fromStdString(entry.GetPhone()));

        if (!query.exec()) {
            qDebug() << "Error inserting data: " << query.lastError().text();
        } else {
            qDebug() << "Record inserted successfully.";
        }
    }

    // Закрытие базы данных после записи всех данных
    db.close();
    qDebug() << "Database connection closed.";
}
void DataHandler::LoadFromDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("contacts.db");

    // Убедитесь, что база данных открыта
    if (!db.open()) {
        qDebug() << "Error: Unable to connect to database";
        return;
    }

    qDebug() << "Database opened successfully.";

    QSqlQuery query("SELECT * FROM contacts");

    entries.clear();  // Clear existing entries

    // Выполнение запроса до закрытия базы данных
    if (!query.exec()) {
        qDebug() << "Error executing query: " << query.lastError().text();
        db.close();
        return;
    }

    // Процесс получения данных из базы
    while (query.next()) {
        std::string firstName = query.value(0).toString().toStdString();
        std::string middleName = query.value(1).toString().toStdString();
        std::string lastName = query.value(2).toString().toStdString();
        std::string dob = query.value(3).toString().toStdString();
        std::string email = query.value(4).toString().toStdString();
        std::string phone = query.value(5).toString().toStdString();

        entries.push_back(ContactEntry(firstName, middleName, lastName, dob, email, phone));
    }

    // Выполнение запроса для удаления всех записей из таблицы

    if (!query.exec("DELETE FROM contacts")) {
        qDebug() << "Error deleting records: " << query.lastError().text();
    } else {
        qDebug() << "All records deleted from 'contacts' table.";
    }

    // Закрытие базы данных после завершения всех операций
    db.close();
    qDebug() << "Database connection closed.";
}

const std::vector<ContactEntry>& DataHandler::GetEntries() const { return entries; }

// MainWindow implementation
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    tableWidget = new QTableWidget(this);
        tableWidget->setColumnCount(6);
        tableWidget->setHorizontalHeaderLabels({"First Name", "Middle Name", "Last Name", "DOB", "Email", "Phone"});

        searchLineEdit = new QLineEdit(this);
        QPushButton *createButton = new QPushButton("Create Entry", this);
        QPushButton *displayButton = new QPushButton("Display Entries", this);
        QPushButton *updateButton = new QPushButton("Update Entry", this);
        QPushButton *deleteButton = new QPushButton("Delete Entry", this);
        QPushButton *sortButton = new QPushButton("Sort Entries", this);
        QPushButton *searchButton = new QPushButton("Search Entries", this);
        QPushButton *exportButton = new QPushButton("Export to File", this);
        QPushButton *importButton = new QPushButton("Import from File", this);
        QPushButton *saveToDatabaseButton = new QPushButton("Save to Database", this);
        QPushButton *loadFromDatabaseButton = new QPushButton("Load from Database", this);

        QVBoxLayout *mainLayout = new QVBoxLayout();
        QHBoxLayout *buttonLayout = new QHBoxLayout();

        buttonLayout->addWidget(createButton);
        buttonLayout->addWidget(displayButton);
        buttonLayout->addWidget(updateButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addWidget(sortButton);
        buttonLayout->addWidget(searchButton);
        buttonLayout->addWidget(exportButton);
        buttonLayout->addWidget(importButton);
        buttonLayout->addWidget(saveToDatabaseButton);
        buttonLayout->addWidget(loadFromDatabaseButton);

        mainLayout->addWidget(searchLineEdit);
        mainLayout->addWidget(tableWidget);
        mainLayout->addLayout(buttonLayout);

        QWidget *centralWidget = new QWidget(this);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);

        connect(createButton, &QPushButton::clicked, this, &MainWindow::onCreateEntry);
        connect(displayButton, &QPushButton::clicked, this, &MainWindow::onDisplayEntries);
        connect(updateButton, &QPushButton::clicked, this, &MainWindow::onUpdateEntry);
        connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteEntry);
        connect(sortButton, &QPushButton::clicked, this, &MainWindow::onSortEntries);
        connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchEntries);
        connect(exportButton, &QPushButton::clicked, this, &MainWindow::onExportToFile);
        connect(importButton, &QPushButton::clicked, this, &MainWindow::onImportFromFile);
        connect(saveToDatabaseButton, &QPushButton::clicked, this, &MainWindow::onSaveToDatabase);
        connect(loadFromDatabaseButton, &QPushButton::clicked, this, &MainWindow::onLoadFromDatabase);}
void MainWindow::updateTable() {
    clearTable();

       const auto& entries = handler.GetEntries();
       for (const auto& entry : entries) {
           int row = tableWidget->rowCount();
           tableWidget->insertRow(row);
           tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(entry.GetFirstName())));
           tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(entry.GetMiddleName())));
           tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(entry.GetLastName())));
           tableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(entry.GetDateOfBirth())));
           tableWidget->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(entry.GetEmail())));
           tableWidget->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(entry.GetPhone())));
       }}
void MainWindow::clearTable() { tableWidget->setRowCount(0); }
void MainWindow::onCreateEntry() {
    bool ok;
      QString firstName = QInputDialog::getText(this, "First Name", "Enter first name:", QLineEdit::Normal, "", &ok);
      if (!ok || firstName.isEmpty()) return;

      QString middleName = QInputDialog::getText(this, "Middle Name", "Enter middle name:", QLineEdit::Normal, "", &ok);
      if (!ok || middleName.isEmpty()) return;

      QString lastName = QInputDialog::getText(this, "Last Name", "Enter last name:", QLineEdit::Normal, "", &ok);
      if (!ok || lastName.isEmpty()) return;

      QString dob = QInputDialog::getText(this, "Date of Birth", "Enter date of birth (dd-mm-yyyy):", QLineEdit::Normal, "", &ok);
      if (!ok || dob.isEmpty()) return;

      QString email = QInputDialog::getText(this, "Email", "Enter email:", QLineEdit::Normal, "", &ok);
      if (!ok || email.isEmpty()) return;

      QString phone = QInputDialog::getText(this, "Phone", "Enter phone:", QLineEdit::Normal, "", &ok);
      if (!ok || phone.isEmpty()) return;

      if (!InputValidator::ValidateName(firstName.toStdString()) ||
          !InputValidator::ValidateName(middleName.toStdString()) ||
          !InputValidator::ValidateName(lastName.toStdString()) ||
          !InputValidator::ValidateDate(dob.toStdString()) ||
          !InputValidator::ValidateEmail(email.toStdString()) ||
          !InputValidator::ValidatePhone(phone.toStdString())) {
          QMessageBox::warning(this, "Invalid Input", "One or more inputs are invalid.");
          return;
      }

      handler.AddEntry(ContactEntry(firstName.toStdString(), middleName.toStdString(),
                                     lastName.toStdString(), dob.toStdString(),
                                     email.toStdString(), phone.toStdString()));

      updateTable();}
void MainWindow::onDisplayEntries() { updateTable(); }
void MainWindow::onUpdateEntry() {
    bool ok;
        int row = QInputDialog::getInt(this, "Update Entry", "Enter row number to update:", 0, 0, tableWidget->rowCount() - 1, 1, &ok);
        if (!ok) return;

        QString firstName = QInputDialog::getText(this, "First Name", "Enter first name:", QLineEdit::Normal, "", &ok);
        if (!ok || firstName.isEmpty()) return;

        QString middleName = QInputDialog::getText(this, "Middle Name", "Enter middle name:", QLineEdit::Normal, "", &ok);
        if (!ok || middleName.isEmpty()) return;

        QString lastName = QInputDialog::getText(this, "Last Name", "Enter last name:", QLineEdit::Normal, "", &ok);
        if (!ok || lastName.isEmpty()) return;

        QString dob = QInputDialog::getText(this, "Date of Birth", "Enter date of birth (dd-mm-yyyy):", QLineEdit::Normal, "", &ok);
        if (!ok || dob.isEmpty()) return;

        QString email = QInputDialog::getText(this, "Email", "Enter email:", QLineEdit::Normal, "", &ok);
        if (!ok || email.isEmpty()) return;

        QString phone = QInputDialog::getText(this, "Phone", "Enter phone:", QLineEdit::Normal, "", &ok);
        if (!ok || phone.isEmpty()) return;

        handler.RemoveEntry(row);  // Удаляем старую запись
        handler.AddEntry(ContactEntry(firstName.toStdString(), middleName.toStdString(),
                                       lastName.toStdString(), dob.toStdString(),
                                       email.toStdString(), phone.toStdString()));  // Добавляем новую
        updateTable();}

void MainWindow::onDeleteEntry() {
    bool ok;
        int row = QInputDialog::getInt(this, "Delete Entry", "Enter row number to delete:", 0, 0, tableWidget->rowCount() - 1, 1, &ok);
        if (!ok) return;

        handler.RemoveEntry(row);
        updateTable();}

void MainWindow::onSortEntries() {
    bool ok;
        QString field = QInputDialog::getText(this, "Sort Entries", "Enter field to sort by (firstName, lastName, dob):", QLineEdit::Normal, "", &ok);
        if (!ok || field.isEmpty()) return;

        handler.Sort(field.toStdString());
        updateTable();}
void MainWindow::onSearchEntries() {
    QString keyword = searchLineEdit->text();
       auto results = handler.Search(keyword.toStdString());

       clearTable();
       for (const auto& entry : results) {
           int row = tableWidget->rowCount();
           tableWidget->insertRow(row);
           tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(entry.GetFirstName())));
           tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(entry.GetMiddleName())));
           tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(entry.GetLastName())));
           tableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(entry.GetDateOfBirth())));
           tableWidget->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(entry.GetEmail())));
           tableWidget->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(entry.GetPhone())));
       }
}
void MainWindow::onExportToFile() {
    QString filePath = QFileDialog::getSaveFileName(this, "Export to File", "", "Text Files (*.txt);;CSV Files (*.csv)");
        if (filePath.isEmpty()) return;

        handler.Save(filePath.toStdString());}

void MainWindow::onImportFromFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Import from File", "", "Text Files (*.txt);;CSV Files (*.csv)");
       if (filePath.isEmpty()) return;

       handler.Load(filePath.toStdString());
       updateTable();}
void MainWindow::onSaveToDatabase() {
    // Сохраняем записи в базу данных
        handler.SaveToDatabase();
        QMessageBox::information(this, "Success", "Data successfully saved to the database.");}
void MainWindow::onLoadFromDatabase() {
    // Загружаем записи из базы данных
       handler.LoadFromDatabase();
       updateTable();
       QMessageBox::information(this, "Success", "Data successfully loaded from the database.");}
