#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <string>
#include <vector>
#include <algorithm>

class ContactEntry {
public:
    ContactEntry(const std::string& fName, const std::string& mName,
                 const std::string& lName, const std::string& dob,
                 const std::string& mail, const std::string& phoneNum);

    void Display() const;

    const std::string& GetFirstName() const;
    const std::string& GetLastName() const;
    const std::string& GetMiddleName() const;
    const std::string& GetDateOfBirth() const;
    const std::string& GetEmail() const;
    const std::string& GetPhone() const;

private:
    std::string firstName, middleName, lastName, dateOfBirth, email, phone;
};

class DataHandler {
public:
    void AddEntry(const ContactEntry& entry);
    void RemoveEntry(int index);
    void Sort(const std::string& field);
    std::vector<ContactEntry> Search(const std::string& keyword) const;
    void Save(const std::string& filePath) const;
    void Load(const std::string& filePath);
    void SaveToDatabase();
    void LoadFromDatabase();
    const std::vector<ContactEntry>& GetEntries() const;

private:
    std::vector<ContactEntry> entries;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void updateTable();
    void clearTable();

    QTableWidget* tableWidget;
    QLineEdit* searchLineEdit;
    DataHandler handler;

private slots:
    void onCreateEntry();
    void onDisplayEntries();
    void onUpdateEntry();
    void onDeleteEntry();
    void onSortEntries();
    void onSearchEntries();
    void onExportToFile();
    void onImportFromFile();
    void onSaveToDatabase();
    void onLoadFromDatabase();
};

#endif // MAINWINDOW_H
