// Contact Book - C++17 console application
// Features: add, search, delete, save, display all contacts.
// Data persisted to contacts.txt (pipe-delimited, with escaping).

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

struct Contact {
    std::string name;
    std::string phone;
    std::string email;
    std::string address;
};

static const char* DATA_FILE = "contacts.txt";

// ---------- Escaping helpers ----------
// Pipes and newlines inside fields are escaped as \p and \n so the
// format stays one-record-per-line: Name|Phone|Email|Address

std::string escapeField(const std::string& field) {
    std::string out;
    out.reserve(field.size());
    for (char c : field) {
        if (c == '|') {
            out += "\\p";
        } else if (c == '\n') {
            out += "\\n";
        } else if (c == '\\') {
            out += "\\\\";
        } else {
            out += c;
        }
    }
    return out;
}

std::string unescapeField(const std::string& field) {
    std::string out;
    out.reserve(field.size());
    for (size_t i = 0; i < field.size(); ++i) {
        if (field[i] == '\\' && i + 1 < field.size()) {
            char next = field[i + 1];
            if (next == 'p') { out += '|'; ++i; continue; }
            if (next == 'n') { out += '\n'; ++i; continue; }
            if (next == '\\') { out += '\\'; ++i; continue; }
        }
        out += field[i];
    }
    return out;
}

std::vector<std::string> splitLine(const std::string& line) {
    // Split on unescaped '|' characters.
    std::vector<std::string> parts;
    std::string current;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '\\' && i + 1 < line.size()) {
            current += line[i];
            current += line[i + 1];
            ++i;
        } else if (line[i] == '|') {
            parts.push_back(current);
            current.clear();
        } else {
            current += line[i];
        }
    }
    parts.push_back(current);
    return parts;
}

// ---------- Utility ----------

std::string trim(const std::string& s) {
    size_t start = 0, end = s.size();
    while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                    [](unsigned char c) { return std::tolower(c); });
    return out;
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void pause() {
    std::cout << "\nPress Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// ---------- Core operations ----------

bool loadContacts(std::vector<Contact>& contacts) {
    std::ifstream in(DATA_FILE);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto parts = splitLine(line);
        if (parts.size() < 4) continue; // skip malformed lines
        Contact c;
        c.name = unescapeField(parts[0]);
        c.phone = unescapeField(parts[1]);
        c.email = unescapeField(parts[2]);
        c.address = unescapeField(parts[3]);
        contacts.push_back(c);
    }
    return true;
}

bool saveContacts(const std::vector<Contact>& contacts) {
    std::ofstream out(DATA_FILE, std::ios::trunc);
    if (!out.is_open()) return false;

    for (const auto& c : contacts) {
        out << escapeField(c.name) << '|'
            << escapeField(c.phone) << '|'
            << escapeField(c.email) << '|'
            << escapeField(c.address) << '\n';
    }
    return true;
}

void addContact(std::vector<Contact>& contacts, bool& dirty) {
    std::cout << "\n--- Add Contact ---\n";
    std::string name;
    while (true) {
        name = trim(readLine("Name: "));
        if (name.empty()) {
            std::cout << "Name cannot be empty. Please try again.\n";
            continue;
        }
        break;
    }
    std::string phone = trim(readLine("Phone: "));
    std::string email = trim(readLine("Email: "));
    std::string address = trim(readLine("Address: "));

    contacts.push_back({name, phone, email, address});
    dirty = true;
    std::cout << "Contact added successfully.\n";
}

void printContact(size_t index, const Contact& c) {
    std::cout << index << ". " << c.name
               << "\n   Phone:   " << c.phone
               << "\n   Email:   " << c.email
               << "\n   Address: " << c.address << "\n";
}

void displayAll(const std::vector<Contact>& contacts) {
    std::cout << "\n--- All Contacts (" << contacts.size() << ") ---\n";
    if (contacts.empty()) {
        std::cout << "No contacts saved yet.\n";
        return;
    }
    for (size_t i = 0; i < contacts.size(); ++i) {
        printContact(i + 1, contacts[i]);
    }
}

void searchContact(const std::vector<Contact>& contacts) {
    std::cout << "\n--- Search Contact ---\n";
    std::string query = toLower(trim(readLine("Enter name (or part of name) to search: ")));
    if (query.empty()) {
        std::cout << "Search term cannot be empty.\n";
        return;
    }

    std::vector<size_t> matches;
    for (size_t i = 0; i < contacts.size(); ++i) {
        if (toLower(contacts[i].name).find(query) != std::string::npos) {
            matches.push_back(i);
        }
    }

    if (matches.empty()) {
        std::cout << "No contacts found matching \"" << query << "\".\n";
        return;
    }

    std::cout << "Found " << matches.size() << " match(es):\n";
    for (size_t i = 0; i < matches.size(); ++i) {
        printContact(i + 1, contacts[matches[i]]);
    }
}

void deleteContact(std::vector<Contact>& contacts, bool& dirty) {
    std::cout << "\n--- Delete Contact ---\n";
    if (contacts.empty()) {
        std::cout << "No contacts to delete.\n";
        return;
    }

    displayAll(contacts);
    std::string input = trim(readLine("\nEnter the number of the contact to delete (0 to cancel): "));
    if (input.empty()) {
        std::cout << "Cancelled.\n";
        return;
    }

    int choice;
    try {
        choice = std::stoi(input);
    } catch (...) {
        std::cout << "Invalid input.\n";
        return;
    }

    if (choice == 0) {
        std::cout << "Cancelled.\n";
        return;
    }
    if (choice < 1 || static_cast<size_t>(choice) > contacts.size()) {
        std::cout << "Invalid selection. Please choose a number between 1 and "
                  << contacts.size() << ".\n";
        return;
    }

    std::string removedName = contacts[choice - 1].name;
    contacts.erase(contacts.begin() + (choice - 1));
    dirty = true;
    std::cout << "Deleted contact: " << removedName << "\n";
}

void printMenu() {
    std::cout << "\n===== C++ Contact Book =====\n"
               << "1. Add contact\n"
               << "2. Search contact\n"
               << "3. Delete contact\n"
               << "4. Save contacts\n"
               << "5. Display all contacts\n"
               << "6. Exit\n"
               << "Choose an option: ";
}

int main() {
    std::vector<Contact> contacts;
    bool dirty = false;

    if (loadContacts(contacts)) {
        std::cout << "Loaded " << contacts.size() << " contact(s) from " << DATA_FILE << ".\n";
    } else {
        std::cout << "No existing " << DATA_FILE << " found. Starting fresh.\n";
    }

    bool running = true;
    while (running) {
        printMenu();
        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        choiceStr = trim(choiceStr);

        if (choiceStr == "1") {
            addContact(contacts, dirty);
        } else if (choiceStr == "2") {
            searchContact(contacts);
        } else if (choiceStr == "3") {
            deleteContact(contacts, dirty);
        } else if (choiceStr == "4") {
            if (saveContacts(contacts)) {
                dirty = false;
                std::cout << "Contacts saved to " << DATA_FILE << ".\n";
            } else {
                std::cout << "Error: could not write to " << DATA_FILE << ".\n";
            }
        } else if (choiceStr == "5") {
            displayAll(contacts);
        } else if (choiceStr == "6") {
            if (dirty) {
                if (saveContacts(contacts)) {
                    std::cout << "Unsaved changes detected. Auto-saved to " << DATA_FILE << ".\n";
                } else {
                    std::cout << "Warning: failed to auto-save changes.\n";
                }
            }
            std::cout << "Goodbye!\n";
            running = false;
        } else {
            std::cout << "Invalid option. Please choose 1-6.\n";
        }
    }

    return 0;
}
