#include "database.h"
#include <iostream>
#include <string>
#include "../sqlite3.h"

Database::Database(const char *name){
    this->name = name;
}

Database::~Database(){
    sqlite3_close(db);
}

void Database::open_db(const char *name){
    int rc = sqlite3_open(name, &db);
    if(rc){
        std::cerr << "Error opening DB: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    } else {
        std::cout << "Opened database successfully!" << std::endl;
    }
}

void Database::create_table(){
    const char *sql = "CREATE TABLE USERS ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"  // Автоінкремент для ID
                      "EMAIL TEXT NOT NULL,"
                      "PASSWORD TEXT NOT NULL,"
                      "NAME TEXT NOT NULL,"
                      "SURNAME TEXT NOT NULL,"
                      "NOTE TEXT NOT NULL);";

    int rc = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error creating table: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Table created successfully" << std::endl;
    }
}

int Database::callback(void *data, int argc, char **argv, char **azColName){
    int *id = static_cast<int*>(data);  // Cast the data to an int pointer
    for(int i = 0; i < argc; i++){
        if (std::string(azColName[i]) == "ID") {
            *id = std::atoi(argv[i]);  // Set the ID when found
        }
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << " ";
    }
    std::cout << std::endl;
    return 0;  // Success
}

void Database::delete_table(){
    const char *sql = "DROP TABLE USERS;";
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error deleting table: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Table deleted successfully" << std::endl;
    }
}

void Database::insert_user(const char *email, const char *password, const char *name, const char *surname, const char *note){
    std::string sql = "INSERT INTO USERS (EMAIL, PASSWORD, NAME, SURNAME, NOTE) VALUES ('";
    sql += email + std::string("', '") + password + "', '" + name + "', '" + surname + "', '" + note + "');";
    
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error inserting user: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "User inserted successfully" << std::endl;
    }
}

void Database::delete_user(int id){
    std::string sql = "DELETE FROM USERS WHERE ID = " + std::to_string(id) + ";";
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error deleting user: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "User deleted successfully" << std::endl;
    }
}

int Database::find_user(const char *email){
    std::string sql = "SELECT ID FROM USERS WHERE EMAIL = '" + std::string(email) + "';";
    int user_id = -1;

    int rc = sqlite3_exec(db, sql.c_str(), callback, &user_id, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error finding user: " << sqlite3_errmsg(db) << std::endl;
    } else if (user_id != -1) {
        std::cout << "User found with ID: " << user_id << std::endl;
    } else {
        std::cout << "User not found." << std::endl;
    }

    return user_id;
}

char* Database::get_user_data(int id){
    std::string sql = "SELECT * FROM USERS WHERE ID = " + std::to_string(id) + ";";
    char *err_msg = nullptr;
    char *data = nullptr;

    int rc = sqlite3_exec(db, sql.c_str(), callback, &data, &err_msg);
    if(rc != SQLITE_OK){
        std::cerr << "Error getting user data: " << err_msg << std::endl;
    } else {
        std::cout << "User data retrieved successfully" << std::endl;
    }

    return data;
}

void Database::create_db(const char *name){
    sqlite3 *db;
    int rc = sqlite3_open(name, &db);
    if(rc){
        std::cerr << "Error creating DB: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
    } else {
        std::cout << "Database created successfully!" << std::endl;
        sqlite3_close(db);
    }
}

void Database::delete_db(const char *name){
    int rc = remove(name);
    if(rc){
        std::cerr << "Error deleting DB" << std::endl;
    } else {
        std::cout << "Database deleted successfully!" << std::endl;
    }
}
