#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <regex>
#include <ctime>
#include <sstream>
#include <iomanip>

class InputValidator {
public:
    static bool ValidateName(const std::string& name) {
        std::string trimmedName = std::regex_replace(name, std::regex("^\\s+|\\s+$"), "");
        std::regex nameRegex(R"(^[A-Za-zÀ-ßà-ÿ¨¸][A-Za-zÀ-ßà-ÿ¨¸\s-]*[A-Za-zÀ-ßà-ÿ¨¸]$)");
        return std::regex_match(trimmedName, nameRegex);
    }

    static bool ValidateDate(const std::string& date) {
        std::regex dateRegex(R"(\d{2}-\d{2}-\d{4})");
        if (!std::regex_match(date, dateRegex)) {
            return false;
        }
        std::tm tm = {};
        std::istringstream ss(date);
        ss >> std::get_time(&tm, "%d-%m-%Y");
        if (ss.fail()) {
            return false;
        }
        std::time_t now = std::time(nullptr);
        std::tm currentTime = {};
        localtime_s(&currentTime, &now);
        if (std::mktime(&tm) >= std::mktime(&currentTime)) {
            return false;
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

class ContactEntry {
public:
    ContactEntry(const std::string& fName, const std::string& mName,
        const std::string& lName, const std::string& dob,
        const std::string& mail, const std::string& phoneNum)
        : firstName(fName), middleName(mName), lastName(lName),
        dateOfBirth(dob), email(mail), phone(phoneNum) {}

    void Display() const {
        std::cout << "Name: " << firstName << " " << middleName << " " << lastName
            << "\nDate of Birth: " << dateOfBirth
            << "\nEmail: " << email
            << "\nPhone: " << phone << "\n";
    }

    const std::string& GetFirstName() const { return firstName; }
    const std::string& GetLastName() const { return lastName; }
    const std::string& GetMiddleName() const { return middleName; }
    const std::string& GetDateOfBirth() const { return dateOfBirth; }
    const std::string& GetEmail() const { return email; }
    const std::string& GetPhone() const { return phone; }

private:
    std::string firstName;
    std::string middleName;
    std::string lastName;
    std::string dateOfBirth;
    std::string email;
    std::string phone;
};

class DataHandler {
public:
    void AddEntry(const ContactEntry& entry) {
        entries.push_back(entry);
    }

    void RemoveEntry(size_t index) {
        if (index < entries.size()) {
            entries.erase(entries.begin() + index);
        }
        else {
            std::cout << "Invalid index.\n";
        }
    }

    void ModifyEntry(size_t index, const ContactEntry& newEntry) {
        if (index < entries.size()) {
            entries[index] = newEntry;
        }
        else {
            std::cout << "Invalid index.\n";
        }
    }

    void ShowAll() const {
        for (size_t i = 0; i < entries.size(); ++i) {
            std::cout << "ID: " << i << "\n";
            entries[i].Display();
            std::cout << "-------------------\n";
        }
    }

    void Sort(const std::string& field) {
        if (field == "firstName") {
            std::sort(entries.begin(), entries.end(), [](const ContactEntry& a, const ContactEntry& b) {
                return a.GetFirstName() < b.GetFirstName();
                });
        }
        else if (field == "lastName") {
            std::sort(entries.begin(), entries.end(), [](const ContactEntry& a, const ContactEntry& b) {
                return a.GetLastName() < b.GetLastName();
                });
        }
        else if (field == "dob") {
            std::sort(entries.begin(), entries.end(), [](const ContactEntry& a, const ContactEntry& b) {
                return a.GetDateOfBirth() < b.GetDateOfBirth();
                });
        }
        else {
            std::cout << "Invalid sort field.\n";
        }
    }

    std::vector<ContactEntry> Search(const std::string& query) const {
        std::vector<ContactEntry> results;
        for (const auto& entry : entries) {
            if (entry.GetFirstName().find(query) != std::string::npos ||
                entry.GetLastName().find(query) != std::string::npos ||
                entry.GetDateOfBirth().find(query) != std::string::npos ||
                entry.GetEmail().find(query) != std::string::npos ||
                entry.GetPhone().find(query) != std::string::npos) {
                results.push_back(entry);
            }
        }
        return results;
    }

    void Save(const std::string& filepath) const {
        std::ofstream outFile(filepath);
        if (!outFile) {
            std::cout << "Failed to open file for writing.\n";
            return;
        }
        for (const auto& entry : entries) {
            outFile << entry.GetFirstName() << ","
                << entry.GetMiddleName() << ","
                << entry.GetLastName() << ","
                << entry.GetDateOfBirth() << ","
                << entry.GetEmail() << ","
                << entry.GetPhone() << "\n";
        }
        std::cout << "Data saved to " << filepath << "\n";
    }

    void Load(const std::string& filepath) {
        std::ifstream inFile(filepath);
        if (!inFile) {
            std::cout << "Failed to open file for reading.\n";
            return;
        }
        entries.clear();
        std::string line;
        while (std::getline(inFile, line)) {
            size_t pos = 0;
            std::string token;
            std::vector<std::string> fields;
            while ((pos = line.find(',')) != std::string::npos) {
                token = line.substr(0, pos);
                fields.push_back(token);
                line.erase(0, pos + 1);
            }
            fields.push_back(line);
            if (fields.size() == 6) {
                entries.emplace_back(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
            }
        }
        std::cout << "Data loaded from " << filepath << "\n";
    }

private:
    std::vector<ContactEntry> entries;
};

void CreateEntry(DataHandler& handler);
void DisplayEntries(const DataHandler& handler);
void UpdateEntry(DataHandler& handler);
void DeleteEntry(DataHandler& handler);
void ArrangeEntries(DataHandler& handler);
void FindEntries(const DataHandler& handler);
void ExportToFile(DataHandler& handler);
void ImportFromFile(DataHandler& handler);

int main() {
    DataHandler handler;
    int option;

    do {
        std::cout << "Options:\n"
            << "1. Create Entry\n"
            << "2. Display Entries\n"
            << "3. Update Entry\n"
            << "4. Delete Entry\n"
            << "5. Arrange Entries\n"
            << "6. Find Entries\n"
            << "7. Export to File\n"
            << "8. Import from File\n"
            << "0. Quit\nChoose: ";
        std::cin >> option;

        switch (option) {
        case 1: CreateEntry(handler); break;
        case 2: DisplayEntries(handler); break;
        case 3: UpdateEntry(handler); break;
        case 4: DeleteEntry(handler); break;
        case 5: ArrangeEntries(handler); break;
        case 6: FindEntries(handler); break;
        case 7: ExportToFile(handler); break;
        case 8: ImportFromFile(handler); break;
        case 0: std::cout << "Goodbye!\n"; break;
        default: std::cout << "Invalid option.\n";
        }
    } while (option != 0);

    return 0;
}

void CreateEntry(DataHandler& handler) {
    std::string firstName, middleName, lastName, dob, email, phone;

    do {
        std::cout << "First Name: ";
        std::cin >> firstName;
        if (!InputValidator::ValidateName(firstName)) {
            std::cout << "Invalid first name. Please try again.\n";
        }
    } while (!InputValidator::ValidateName(firstName));

    do {
        std::cout << "Middle Name: ";
        std::cin >> middleName;
        if (!InputValidator::ValidateName(middleName)) {
            std::cout << "Invalid middle name. Please try again.\n";
        }
    } while (!InputValidator::ValidateName(middleName));

    do {
        std::cout << "Last Name: ";
        std::cin >> lastName;
        if (!InputValidator::ValidateName(lastName)) {
            std::cout << "Invalid last name. Please try again.\n";
        }
    } while (!InputValidator::ValidateName(lastName));

    do {
        std::cout << "Date of Birth (dd-mm-yyyy): ";
        std::cin >> dob;
        if (!InputValidator::ValidateDate(dob)) {
            std::cout << "Invalid date of birth. Please try again.\n";
        }
    } while (!InputValidator::ValidateDate(dob));

    do {
        std::cout << "Email: ";
        std::cin >> email;
        if (!InputValidator::ValidateEmail(email)) {
            std::cout << "Invalid email. Please try again.\n";
        }
    } while (!InputValidator::ValidateEmail(email));

    do {
        std::cout << "Phone: ";
        std::cin >> phone;
        if (!InputValidator::ValidatePhone(phone)) {
            std::cout << "Invalid phone number. Please try again.\n";
        }
    } while (!InputValidator::ValidatePhone(phone));

    handler.AddEntry(ContactEntry(firstName, middleName, lastName, dob, email, phone));
}

void DisplayEntries(const DataHandler& handler) {
    handler.ShowAll();
}

void UpdateEntry(DataHandler& handler) {
    size_t id;
    std::cout << "ID of entry to update: ";
    std::cin >> id;

    std::string firstName, middleName, lastName, dob, email, phone;
    std::cout << "New First Name: ";
    std::cin >> firstName;
    std::cout << "New Middle Name: ";
    std::cin >> middleName;
    std::cout << "New Last Name: ";
    std::cin >> lastName;
    std::cout << "New Date of Birth (dd-mm-yyyy): ";
    std::cin >> dob;
    std::cout << "New Email: ";
    std::cin >> email;
    std::cout << "New Phone: ";
    std::cin >> phone;

    handler.ModifyEntry(id, ContactEntry(firstName, middleName, lastName, dob, email, phone));
}

void DeleteEntry(DataHandler& handler) {
    size_t id;
    std::cout << "ID of entry to delete: ";
    std::cin >> id;

    handler.RemoveEntry(id);
}

void ArrangeEntries(DataHandler& handler) {
    std::string criteria;
    std::cout << "Sort by (firstName/lastName/dob): ";
    std::cin >> criteria;

    handler.Sort(criteria);
}

void FindEntries(const DataHandler& handler) {
    std::string keyword;
    std::cout << "Search keyword: ";
    std::cin >> keyword;

    auto results = handler.Search(keyword);
    for (const auto& entry : results) {
        entry.Display();
    }
}

void ExportToFile(DataHandler& handler) {
    std::string filepath;
    std::cout << "File path to export: ";
    std::cin >> filepath;

    handler.Save(filepath);
}

void ImportFromFile(DataHandler& handler) {
    std::string filepath;
    std::cout << "File path to import: ";
    std::cin >> filepath;

    handler.Load(filepath);
}
