#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <ctime>
#include <windows.h>

using namespace std;

// Установка русской кодировки
void setRussian() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
}

// Структура записи
struct BookRecord {
    int id;
    char bookTitle[100];
    char readerName[100];
    char issueDate[11];
    char expectedReturnDate[11];
    char actualReturnDate[11];
    bool isReturned;
    
    BookRecord() : id(0), isReturned(false) {
        memset(bookTitle, 0, sizeof(bookTitle));
        memset(readerName, 0, sizeof(readerName));
        memset(issueDate, 0, sizeof(issueDate));
        memset(expectedReturnDate, 0, sizeof(expectedReturnDate));
        memset(actualReturnDate, 0, sizeof(actualReturnDate));
    }
};

class LibraryManager {
private:
    string filename;
    vector<BookRecord> records;
    int nextId;
    
    void clearBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char date[11];
        sprintf(date, "%02d.%02d.%04d", ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);
        return string(date);
    }
    
    int compareDates(const string& date1, const string& date2) {
        if (date1.empty() || date2.empty()) return 0;
        
        int day1, month1, year1, day2, month2, year2;
        sscanf(date1.c_str(), "%d.%d.%d", &day1, &month1, &year1);
        sscanf(date2.c_str(), "%d.%d.%d", &day2, &month2, &year2);
        
        if (year1 != year2) return year1 - year2;
        if (month1 != month2) return month1 - month2;
        return day1 - day2;
    }
    
    bool isOverdue(const BookRecord& record) {
        if (record.isReturned) return false;
        string currentDate = getCurrentDate();
        return compareDates(currentDate, record.expectedReturnDate) > 0;
    }
    
public:
    LibraryManager(const string& fname = "library.dat") : filename(fname), nextId(1) {
        loadFromFile();
    }
    
    void loadFromFile() {
        ifstream file(filename, ios::binary);
        if (!file) {
            cout << "Файл не найден. Будет создан новый файл.\n";
            return;
        }
        
        records.clear();
        BookRecord record;
        while (file.read(reinterpret_cast<char*>(&record), sizeof(BookRecord))) {
            records.push_back(record);
            if (record.id >= nextId) {
                nextId = record.id + 1;
            }
        }
        file.close();
        
        cout << "Загружено " << records.size() << " записей.\n";
    }
    
    void saveToFile() {
        ofstream file(filename, ios::binary | ios::trunc);
        if (!file) {
            cout << "Ошибка: не удалось открыть файл для записи!\n";
            return;
        }
        
        for (const auto& record : records) {
            file.write(reinterpret_cast<const char*>(&record), sizeof(BookRecord));
        }
        file.close();
        
        cout << "Данные сохранены.\n";
    }
    
    void addRecord() {
        BookRecord record;
        record.id = nextId++;
        
        cout << "\n=== Добавление новой записи ===\n";
        cout << "ID записи: " << record.id << "\n";
        
        cout << "Введите название книги: ";
        clearBuffer();
        cin.getline(record.bookTitle, sizeof(record.bookTitle));
        
        cout << "Введите ФИО читателя: ";
        cin.getline(record.readerName, sizeof(record.readerName));
        
        cout << "Введите дату выдачи (ДД.ММ.ГГГГ): ";
        cin.getline(record.issueDate, sizeof(record.issueDate));
        
        cout << "Введите предполагаемую дату возврата (ДД.ММ.ГГГГ): ";
        cin.getline(record.expectedReturnDate, sizeof(record.expectedReturnDate));
        
        record.isReturned = false;
        memset(record.actualReturnDate, 0, sizeof(record.actualReturnDate));
        
        records.push_back(record);
        saveToFile();
        
        cout << "Запись успешно добавлена!\n";
    }
    
    void viewAllRecords() {
        if (records.empty()) {
            cout << "\nНет записей для отображения.\n";
            return;
        }
        
        cout << "\n=== Все записи ===\n";
        cout << left << setw(5) << "ID" 
             << setw(30) << "Название книги"
             << setw(25) << "Читатель"
             << setw(12) << "Дата выдачи"
             << setw(15) << "Предп. возврат"
             << setw(15) << "Факт. возврат"
             << setw(10) << "Статус" << "\n";
        cout << string(112, '-') << "\n";
        
        for (const auto& record : records) {
            cout << left << setw(5) << record.id
                 << setw(30) << record.bookTitle
                 << setw(25) << record.readerName
                 << setw(12) << record.issueDate
                 << setw(15) << record.expectedReturnDate;
            
            if (record.isReturned) {
                cout << setw(15) << record.actualReturnDate
                     << setw(10) << "Возвращена";
            } else {
                cout << setw(15) << "Не возвращена"
                     << setw(10) << (isOverdue(record) ? "ПРОСРОЧКА!" : "На руках");
            }
            cout << "\n";
        }
    }
    
    void editRecord() {
        if (records.empty()) {
            cout << "\nНет записей для редактирования.\n";
            return;
        }
        
        int id;
        cout << "\n=== Редактирование записи ===\n";
        cout << "Введите ID записи: ";
        cin >> id;
        
        auto it = find_if(records.begin(), records.end(), [id](const BookRecord& r) {
            return r.id == id;
        });
        
        if (it == records.end()) {
            cout << "Запись с ID " << id << " не найдена!\n";
            return;
        }
        
        cout << "\nТекущие данные:\n";
        cout << "Книга: " << it->bookTitle << "\n";
        cout << "Читатель: " << it->readerName << "\n";
        cout << "Дата выдачи: " << it->issueDate << "\n";
        cout << "Предполагаемая дата возврата: " << it->expectedReturnDate << "\n";
        cout << "Статус: " << (it->isReturned ? "Возвращена" : "Не возвращена") << "\n";
        
        cout << "\nВведите новые данные (оставьте пустым, чтобы не изменять):\n";
        
        clearBuffer();
        char buffer[100];
        
        cout << "Название книги [" << it->bookTitle << "]: ";
        cin.getline(buffer, sizeof(buffer));
        if (strlen(buffer) > 0) strcpy(it->bookTitle, buffer);
        
        cout << "ФИО читателя [" << it->readerName << "]: ";
        cin.getline(buffer, sizeof(buffer));
        if (strlen(buffer) > 0) strcpy(it->readerName, buffer);
        
        cout << "Дата выдачи [" << it->issueDate << "]: ";
        cin.getline(buffer, sizeof(buffer));
        if (strlen(buffer) > 0) strcpy(it->issueDate, buffer);
        
        cout << "Предполагаемая дата возврата [" << it->expectedReturnDate << "]: ";
        cin.getline(buffer, sizeof(buffer));
        if (strlen(buffer) > 0) strcpy(it->expectedReturnDate, buffer);
        
        saveToFile();
        cout << "Запись успешно отредактирована!\n";
    }
    
    void deleteRecord() {
        if (records.empty()) {
            cout << "\nНет записей для удаления.\n";
            return;
        }
        
        int id;
        cout << "\n=== Удаление записи ===\n";
        cout << "Введите ID записи: ";
        cin >> id;
        
        auto it = find_if(records.begin(), records.end(), [id](const BookRecord& r) {
            return r.id == id;
        });
        
        if (it == records.end()) {
            cout << "Запись с ID " << id << " не найдена!\n";
            return;
        }
        
        cout << "\nВы уверены, что хотите удалить запись?\n";
        cout << "Книга: " << it->bookTitle << "\n";
        cout << "Читатель: " << it->readerName << "\n";
        cout << "1 - Да, 2 - Нет\n";
        
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            records.erase(it);
            saveToFile();
            cout << "Запись успешно удалена!\n";
        } else {
            cout << "Удаление отменено.\n";
        }
    }
    
    void returnBook() {
        if (records.empty()) {
            cout << "\nНет записей для возврата.\n";
            return;
        }
        
        int id;
        cout << "\n=== Возврат книги ===\n";
        cout << "Введите ID записи: ";
        cin >> id;
        
        auto it = find_if(records.begin(), records.end(), [id](const BookRecord& r) {
            return r.id == id;
        });
        
        if (it == records.end()) {
            cout << "Запись с ID " << id << " не найдена!\n";
            return;
        }
        
        if (it->isReturned) {
            cout << "Эта книга уже возвращена!\n";
            return;
        }
        
        cout << "\nИнформация о выдаче:\n";
        cout << "Книга: " << it->bookTitle << "\n";
        cout << "Читатель: " << it->readerName << "\n";
        cout << "Дата выдачи: " << it->issueDate << "\n";
        cout << "Предполагаемый возврат: " << it->expectedReturnDate << "\n";
        
        cout << "\nВведите фактическую дату возврата (ДД.ММ.ГГГГ): ";
        clearBuffer();
        cin.getline(it->actualReturnDate, sizeof(it->actualReturnDate));
        
        it->isReturned = true;
        
        if (compareDates(it->actualReturnDate, it->expectedReturnDate) > 0) {
            cout << "\nВНИМАНИЕ: Книга возвращена с просрочкой!\n";
        }
        
        saveToFile();
        cout << "Книга успешно возвращена!\n";
    }
    
    void searchByReader() {
        if (records.empty()) {
            cout << "\nНет записей для поиска.\n";
            return;
        }
        
        char searchName[100];
        cout << "\n=== Поиск по читателю ===\n";
        cout << "Введите ФИО читателя: ";
        clearBuffer();
        cin.getline(searchName, sizeof(searchName));
        
        vector<BookRecord> results;
        for (const auto& record : records) {
            if (strstr(record.readerName, searchName) != nullptr) {
                results.push_back(record);
            }
        }
        
        if (results.empty()) {
            cout << "Записи для читателя \"" << searchName << "\" не найдены.\n";
            return;
        }
        
        cout << "\nНайдено записей: " << results.size() << "\n";
        cout << left << setw(5) << "ID" 
             << setw(30) << "Название книги"
             << setw(25) << "Читатель"
             << setw(12) << "Дата выдачи"
             << setw(15) << "Предп. возврат"
             << setw(10) << "Статус" << "\n";
        cout << string(97, '-') << "\n";
        
        for (const auto& record : results) {
            cout << left << setw(5) << record.id
                 << setw(30) << record.bookTitle
                 << setw(25) << record.readerName
                 << setw(12) << record.issueDate
                 << setw(15) << record.expectedReturnDate;
            
            if (record.isReturned) {
                cout << setw(10) << "Возвращена";
            } else {
                cout << setw(10) << (isOverdue(record) ? "ПРОСРОЧКА" : "На руках");
            }
            cout << "\n";
        }
    }
    
    void showOverdue() {
        vector<BookRecord> overdue;
        for (const auto& record : records) {
            if (!record.isReturned && isOverdue(record)) {
                overdue.push_back(record);
            }
        }
        
        if (overdue.empty()) {
            cout << "\nНет просроченных книг.\n";
            return;
        }
        
        cout << "\n=== Просроченные книги ===\n";
        cout << left << setw(5) << "ID" 
             << setw(30) << "Название книги"
             << setw(25) << "Читатель"
             << setw(12) << "Дата выдачи"
             << setw(15) << "Предп. возврат"
             << setw(15) << "Дней просрочки" << "\n";
        cout << string(102, '-') << "\n";
        
        string currentDate = getCurrentDate();
        for (const auto& record : overdue) {
            int days = 0;
            int day1, month1, year1, day2, month2, year2;
            sscanf(currentDate.c_str(), "%d.%d.%d", &day1, &month1, &year1);
            sscanf(record.expectedReturnDate, "%d.%d.%d", &day2, &month2, &year2);
            days = (year1 - year2) * 365 + (month1 - month2) * 30 + (day1 - day2);
            
            cout << left << setw(5) << record.id
                 << setw(30) << record.bookTitle
                 << setw(25) << record.readerName
                 << setw(12) << record.issueDate
                 << setw(15) << record.expectedReturnDate
                 << setw(15) << days << "\n";
        }
    }
    
    void countBooks() {
        int count = 0;
        for (const auto& record : records) {
            if (!record.isReturned) {
                count++;
            }
        }
        
        cout << "\n=== Количество книг на руках ===\n";
        cout << "Всего книг выдано: " << records.size() << "\n";
        cout << "Книг на руках: " << count << "\n";
        cout << "Возвращено книг: " << (records.size() - count) << "\n";
    }
    
    void sortRecords() {
        if (records.empty()) {
            cout << "\nНет записей для сортировки.\n";
            return;
        }
        
        cout << "\n=== Сортировка записей ===\n";
        cout << "1 - По названию книги\n";
        cout << "2 - По ФИО читателя\n";
        cout << "3 - По дате выдачи\n";
        cout << "4 - По предполагаемой дате возврата\n";
        cout << "Выберите поле для сортировки: ";
        
        int choice;
        cin >> choice;
        
        switch(choice) {
            case 1:
                sort(records.begin(), records.end(), [](const BookRecord& a, const BookRecord& b) {
                    return strcmp(a.bookTitle, b.bookTitle) < 0;
                });
                cout << "Записи отсортированы по названию книги.\n";
                break;
            case 2:
                sort(records.begin(), records.end(), [](const BookRecord& a, const BookRecord& b) {
                    return strcmp(a.readerName, b.readerName) < 0;
                });
                cout << "Записи отсортированы по ФИО читателя.\n";
                break;
            case 3:
                sort(records.begin(), records.end(), [this](const BookRecord& a, const BookRecord& b) {
                    return compareDates(a.issueDate, b.issueDate) < 0;
                });
                cout << "Записи отсортированы по дате выдачи.\n";
                break;
            case 4:
                sort(records.begin(), records.end(), [this](const BookRecord& a, const BookRecord& b) {
                    return compareDates(a.expectedReturnDate, b.expectedReturnDate) < 0;
                });
                cout << "Записи отсортированы по предполагаемой дате возврата.\n";
                break;
            default:
                cout << "Неверный выбор!\n";
                return;
        }
        
        saveToFile();
        viewAllRecords();
    }
    
    void showMenu() {
        int choice;
        
        do {
            cout << "\n╔════════════════════════════════════════╗\n";
            cout << "║     БИБЛИОТЕЧНАЯ СИСТЕМА УЧЕТА      ║\n";
            cout << "╠════════════════════════════════════════╣\n";
            cout << "║ 1 - Добавить запись о выдаче         ║\n";
            cout << "║ 2 - Просмотр всех записей            ║\n";
            cout << "║ 3 - Редактировать запись             ║\n";
            cout << "║ 4 - Удалить запись                   ║\n";
            cout << "║ 5 - Возврат книги                    ║\n";
            cout << "║ 6 - Поиск по читателю                ║\n";
            cout << "║ 7 - Просроченные книги               ║\n";
            cout << "║ 8 - Количество книг на руках         ║\n";
            cout << "║ 9 - Сортировка записей               ║\n";
            cout << "║ 0 - Выход                            ║\n";
            cout << "╚════════════════════════════════════════╝\n";
            cout << "Выберите действие: ";
            
            cin >> choice;
            
            if (cin.fail()) {
                clearBuffer();
                cout << "Ошибка: введите число!\n";
                continue;
            }
            
            switch(choice) {
                case 1: addRecord(); break;
                case 2: viewAllRecords(); break;
                case 3: editRecord(); break;
                case 4: deleteRecord(); break;
                case 5: returnBook(); break;
                case 6: searchByReader(); break;
                case 7: showOverdue(); break;
                case 8: countBooks(); break;
                case 9: sortRecords(); break;
                case 0: cout << "\nДо свидания!\n"; break;
                default: cout << "Неверный выбор! Попробуйте снова.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    setRussian();
    LibraryManager library("library.dat");
    library.showMenu();
    return 0;
}
