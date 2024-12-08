#ifndef DB_HPP
#define DB_HPP
#ifndef linux
#include <iostream>
namespace db
{
    void USE_LINUX()
    {
        std::cout << "go find a linux system to run this, it wont run on windows 10. I tried already." << endl;
        __exit(-1);
    }
}
#else
#include <mariadb/conncpp.hpp>
#include <fstream>

// functions namespace
namespace data
{
    int connect_to_db(std::map<std::string, std::string> *database_config);
    void database_selection_check(std::string target_database);
    bool database_execute(std::string target_database, std::string query);
    sql::ResultSet *database_execute_query(std::string target_database, std::string query);
    std::list<std::map<std::string, void *>> database_query_result_resolve(sql::ResultSet *result, std::string **ids, std::string schema);
    void database_query_result_recycle(std::list<std::map<std::string, void *>> result_list);
};

using namespace sql;
// sql database associated things
static sql::SQLString *database_url = nullptr;
static sql::Driver *database_driver = nullptr;
static sql::Properties *database_properties = nullptr;
static std::unique_ptr<sql::Connection> *database_connection_pointer = nullptr;
static std::unique_ptr<sql::Statement> *database_query_pointer = nullptr;
static std::string current_database = "";

// read the config file
std::map<std::string, std::string> *read_database_config(std::string config_file_path)
{
    // open config and read
    std::ifstream fs(config_file_path);
    // save config and a string:string map and return
    std::map<std::string, std::string> *config = new std::map<std::string, std::string>();

    if (fs.is_open())
    {
        std::string buffer;
        int equal_index;
        std::string key, value;

        while (std::getline(fs, buffer))
        {
            equal_index = buffer.find_first_of("=");
            key = buffer.substr(0, equal_index);
            value = buffer.substr(equal_index + 1, buffer.length() - 1);
            (*config)[key] = value;
        }

        return config;
    }
    else
    {
        printf("config file cannot open or does not exist");
        exit(-1);
    }
}

// connect to database
int data::connect_to_db(std::map<std::string, std::string> *database_config)
{
    // database URL for connecting to database
    char db_url_buffer[128];
    sprintf(db_url_buffer, "jdbc:mariadb://%s:%s/%s", (*database_config)["HOST"].c_str(), (*database_config)["PORT"].c_str(), (*database_config)["DATABASE"].c_str());

    database_url = new SQLString(db_url_buffer);
    database_driver = sql::mariadb::get_driver_instance();
    database_properties = new Properties({
        {"user", (*database_config)["USER"].c_str()},
        {"password", (*database_config)["PASSWORD"].c_str()},
    });

    delete database_config;

    // connect to database
    database_connection_pointer = new std::unique_ptr<sql::Connection>(database_driver->connect(*database_url, *database_properties));

    if (!database_connection_pointer)
    {
        printf("failed connecting to database");
        return -1;
    }

    return 0;
}

// check if the operating database is the same as the current database, if not change the database
void data::database_selection_check(std::string target_database)
{
    if (database_query_pointer == nullptr)
    {
        database_query_pointer = new std::unique_ptr<sql::Statement>((*database_connection_pointer)->createStatement());
        (*database_query_pointer)->execute("SET character set utf8");
    }

    if (current_database != target_database)
    {
        (*database_query_pointer)->executeQuery("use " + target_database);
        current_database = target_database;
    }
}

// execute SQL query, return bool value of if success
bool data::database_execute(std::string target_database, std::string query)
{
    data::database_selection_check(target_database);
    return (*database_query_pointer)->execute(query);
}

// execute search query, return the results as a char array(as IDK what was being store, likely string or int)
sql::ResultSet *data::database_execute_query(std::string target_database, std::string query)
{
    data::database_selection_check(target_database);
    return (*database_query_pointer)->executeQuery(query);
}

// resolve the result, ids are key for result,schema are types of result,bytes_size are size of each binary result
std::list<std::map<std::string, void *>> data::database_query_result_resolve(sql::ResultSet *result, std::string **ids, std::string schema)
{
    std::list<std::map<std::string, void *>> result_list;
    std::map<std::string, void *> res;
    // number of results
    std::string id;
    // get the type of value based on schema
    char type;
    // get the size of the binary value
    int byte_index = 0;

    // for each individual in result
    while (result->next())
    {
        // for each key in result
        for (int i = 0; i < schema.length(); i++)
        {
            // get id
            id = *(ids[i]);
            type = schema[i];
            switch (type)
            {
            case 'i':
            {
                // new an int
                int *buffer = new int();
                // save result
                *buffer = result->getInt(id);
                // save pointer
                res[id] = buffer;
                break;
            };
            case 's':
            {
                std::string *buffer = new std::string();
                *buffer = result->getString(id);
                res[id] = buffer;
                break;
            };
            default:
                break;
            }
        }
        result_list.push_back(res);
    }
    return result_list;
}

// free the result after used
void data::database_query_result_recycle(std::list<std::map<std::string, void *>> result_list)
{
    for (std::map<std::string, void *> &res_map : result_list)
    {
        for (auto const &key : res_map)
        {
            // free those results saved in map
            free(key.second);
        }
    }
}

#endif
#endif