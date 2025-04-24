
#include "../sqlite3.h"

class Database{
public:
    Database(const char *name);
    Database() = default;
    void open_db(const char *name);
    void create_table();
    void delete_table();
    void delete_user(int id);
    int find_user(const char *email);
    char* get_user_data(int id);
    static void create_db(const char *name);
    static void delete_db(const char *name);
    void insert_user(const char *email, const char *password, const char *name, const char *surname, const char *note);
    static int callback(void *NotUsed, int argc, char **argv, char **azColName);
    ~Database();
    sqlite3* get_db(){
        return db;
    }
private:
    sqlite3 *db;
    const char *name;
};