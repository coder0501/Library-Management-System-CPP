#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

class Book {
public:
    std::string title;
    std::string author;
    int bookId;
    bool isAvailable;

    Book() : bookId(0), isAvailable(true) {}  // Default constructor for loading from file

    Book(std::string title, std::string author, int bookId) :
        title(title), author(author), bookId(bookId), isAvailable(true) {}

    void serialize(std::ofstream& outFile) const {
        outFile << bookId << "," << title << "," << author << "," << isAvailable << "\n";
    }

    void deserialize(const std::string& data) {
        std::istringstream ss(data);
        std::string field;
        std::getline(ss, field, ',');
        bookId = std::stoi(field);
        std::getline(ss, title, ',');
        std::getline(ss, author, ',');
        std::getline(ss, field, ',');
        isAvailable = field == "1";
    }

    void display() const {
        std::cout << "Book ID: " << bookId
                  << ", Title: " << title
                  << ", Author: " << author
                  << ", Available: " << (isAvailable ? "Yes" : "No") << std::endl;
    }
};

class User {
public:
    std::string name;
    int userId;
    std::vector<int> borrowedBooks;

    User() : userId(0) {}  // Default constructor for loading from file

    User(std::string name, int userId) : name(name), userId(userId) {}

    void serialize(std::ofstream& outFile) const {
        outFile << userId << "," << name;
        for (int id : borrowedBooks) {
            outFile << "," << id;
        }
        outFile << "\n";
    }

    void deserialize(const std::string& data) {
        std::istringstream ss(data);
        std::string field;
        std::getline(ss, field, ',');
        userId = std::stoi(field);
        std::getline(ss, name, ',');
        while (std::getline(ss, field, ',')) {
            borrowedBooks.push_back(std::stoi(field));
        }
    }

    void display() const {
        std::cout << "User ID: " << userId
                  << ", Name: " << name << std::endl;
        if (!borrowedBooks.empty()) {
            std::cout << "Borrowed Book IDs: ";
            for (int id : borrowedBooks) {
                std::cout << id << " ";
            }
            std::cout << std::endl;
        }
    }
};

class LibraryManager {
private:
    std::vector<Book> books;
    std::vector<User> users;
    const std::string booksFile = "books.txt";
    const std::string usersFile = "users.txt";

public:
    LibraryManager() {
        loadBooks();
        loadUsers();
    }

    ~LibraryManager() {
        saveBooks();
        saveUsers();
    }

    void addBook(const Book& book) {
        books.push_back(book);
        saveBooks();
    }

    void registerUser(const User& user) {
        users.push_back(user);
        saveUsers();
    }

    void saveBooks() {
        std::ofstream outFile(booksFile);
        for (const auto& book : books) {
            book.serialize(outFile);
        }
    }

    void saveUsers() {
        std::ofstream outFile(usersFile);
        for (const auto& user : users) {
            user.serialize(outFile);
        }
    }

    bool borrowBook(int bookId, int userId) {
        auto it = std::find_if(books.begin(), books.end(), [bookId](const Book& b) {
            return b.bookId == bookId;
        });

        if (it != books.end() && it->isAvailable) {
            it->isAvailable = false;

            auto user_it = std::find_if(users.begin(), users.end(), [userId](const User& u) {
                return u.userId == userId;
            });
            
            if (user_it != users.end()) {
                user_it->borrowedBooks.push_back(bookId);
                return true;
            }
        }
        return false;
    }

    bool returnBook(int bookId) {
        auto it = std::find_if(books.begin(), books.end(), [bookId](const Book& b) {
            return b.bookId == bookId;
        });

        if (it != books.end() && !it->isAvailable) {
            it->isAvailable = true;
            return true;
        }
        return false;
    }
    
    void displayBooks() const {
        for (const auto& book : books) {
            book.display();
        }
    }

    void displayUsers() const {
        for (const auto& user : users) {
            user.display();
        }
    }
    
    void loadBooks() {
        std::ifstream inFile(booksFile);
        std::string line;
        while (std::getline(inFile, line)) {
            Book book;
            book.deserialize(line);
            books.push_back(book);
        }
    }

    void loadUsers() {
        std::ifstream inFile(usersFile);
        std::string line;
        while (std::getline(inFile, line)) {
            User user;
            user.deserialize(line);
            users.push_back(user);
        }
    }

    // Remaining methods unchanged
};

int main() {
    LibraryManager library;

    // Example operations
    // Test by commenting out add operations after first run to see load functionality
    library.addBook(Book("1984", "George Orwell", 1));
    library.addBook(Book("To Kill a Mockingbird", "Harper Lee", 2));
    library.registerUser(User("Alice Smith", 101));
    library.registerUser(User("Bob Johnson", 102));

    std::cout << "Initial Library Status:" << std::endl;
    library.displayBooks();
    library.displayUsers();

    // Borrow and return operations
    library.borrowBook(1, 101);
    library.borrowBook(2, 102);

    std::cout << "\nAfter Borrowing:" << std::endl;
    library.displayBooks();
    library.displayUsers();

    library.returnBook(1);
    library.returnBook(2);

    std::cout << "\nAfter Returning:" << std::endl;
    library.displayBooks();
    library.displayUsers();

    return 0;
}
