//
//  OSQLiteTest.cpp
//  OSQLite.Safety
//
//  Created by Steven Chang on 15/7/25.
//  Copyright (c) 2015 Steven Chang. All rights reserved.
//

#include "OSQLite.h"

// Pre-defines
#define TEST_SUCCESS(func_name) std::cout<<">> "<<#func_name<<"... SUCCESS"<<std::endl
#define TEST_FAIL(func_name) std::cout<<"!! "<<#func_name<<"... FAIL!!!"<<std::endl
#ifdef _MSC_VER
std::string databaseFilePath = "c://software/sqlite3/sqlitedb";
#else
std::string databaseFilePath = "/Users/Steven/Documents/Database/sqlitedb";
#endif

// Test: check the behavior of OSDatabase
void test_OSDatabase_ctors_dtors()
try{
    using namespace OSQLite;
    OSDatabase database(databaseFilePath);
    TEST_SUCCESS(ctor_dtors);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(ctor_dtors);
}

// Test: check OSStatement::execute interfaces
void test_OSStatement_execute()
try {
    using namespace OSQLite;
    // 1: no returns, no binding
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai'); insert into Person(id, name, address)  values(2, 'kevin', 'beijing')");
    _statement.execute("delete from Person where id=1");
    _statement.execute("drop table Person");
    // 2: no returns, binding
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id)); insert into Person(id, name, address) values(2, 'kevin', 'beijing')");
    int ivalue = 1;
    std::string nameValue = "steven";
    std::string addressValue = "shanghai";
    _statement.execute("insert into Person(id, name, address) values(?, ?, ?);", ivalue, nameValue, addressValue);
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(execute);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(execute);
}

// Test: OSStatement::executeRows interface
void test_OSStatement_executeRows()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai'); insert into Person(id, name, address)  values(2, 'kevin', 'beijing')");
    
    auto resultVec = _statement.executeRows<int, std::string, std::string>("select * from Person");
    if (resultVec.size()!=2) {
        throw OSQLite::OSException("Failed,1");
    }
    auto _tuple1 = resultVec[0];
    if (std::get<0>(_tuple1)!=1 || std::get<1>(_tuple1)!="steven" || std::get<2>(_tuple1)!="shanghai") {
        throw OSQLite::OSException("Failed,2");
    }
    
    auto _tuple2 = resultVec[1];
    if (std::get<0>(_tuple2)!=2 || std::get<1>(_tuple2)!="kevin" || std::get<2>(_tuple2)!="beijing") {
        throw OSQLite::OSException("Failed,3");
    }
    
    int _id = 2;
    resultVec = _statement.executeRows<int, std::string, std::string>("select * from Person where id=?", _id);
    if (resultVec.size()!=1) {
        throw OSQLite::OSException("Failed,4");
    }
    
    auto _tuple3 = resultVec[0];
    if (std::get<0>(_tuple3)!=2 || std::get<1>(_tuple3)!="kevin" || std::get<2>(_tuple3)!="beijing") {
        throw OSQLite::OSException("Failed,5");
    }
    
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(executeRows);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(executeRows);
}
    
// Test: OSStatement::executeScalar interface
void test_OSStatement_executeScalar()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai'); insert into Person(id, name, address)  values(2, 'kevin', 'beijing')");
    
    auto count = _statement.executeScalar<int>("select count(*) from Person");
    if (count != 2) {
        throw OSException("count is not 2");
    }
    
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(executeScalar);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(executeScalar);
}
    
// Test: check OSStatement transactions interface
void test_OSStatement_transactions()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai')");
        
    _statement.begin();
    try {
        _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai')");
        _statement.commit();
    } catch (const OSException&) {
        _statement.rollback();
    }
    
    _statement.execute("drop table Person");
        
    TEST_SUCCESS(transactions);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(transactions);
}


// Sample class displaying how to use OSQuery and OSTablePolicy.
// You just need to inherit OSTablePolicy class, and bind table name, primary
// key (there must be one) and other keys. Then you must call those binding
// functions at least once. After that, you can call the functions defined
// in OSQuery (save, saveOrUpdate, update, deleteObject) freely.
class Person : virtual public OSQLite::OSTablePolicy<Person> {
public:
    Person(int& id, const std::string& name, const std::string& address):_id(id), _name(name), _address(address), OSTablePolicy("Person", {"id", "name", "address"}, _id, _name, _address) {}
    virtual ~Person() {}
        
    // parameters should not have cv specifiers, and they should not be a reference
    // or a pointer. But they can be privite or public, as you like.
    int _id;
    std::string _name;
    std::string _address;
};

// Test: check OSQuery::save interface
void test_OSQuery_save()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    
    OSQuery _query(_database);
    int _id = 1;
    std::string _name = "xiaoyu";
    std::string _address = "CUC";
    Person _personObject(_id, _name, _address);
    _query.save(_personObject);
    
    auto resultVec = _statement.executeRows<int, std::string, std::string>("select * from Person");
    auto _tuple1 = resultVec[0];
    if (std::get<0>(_tuple1)!=_id || std::get<1>(_tuple1)!=_name || std::get<2>(_tuple1)!=_address) {
        throw OSQLite::OSException("Failed");
    }
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(save);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(save);
}
    
// Test: check OSQuery::exists interface
void test_OSQuery_exists()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai')");
    
    OSQuery _query(_database);
    int _id = 1;
    std::string _name = "xiaoyu";
    std::string _address = "CUC";
    Person _personObject(_id, _name, _address);
    if (!_query.exists(_personObject)) {
        throw OSException("Failed, 1");
    }
    
    _id = 2;
    Person _personObject2(_id, _name, _address);
    if (_query.exists(_personObject2)) {
        throw OSException("Failed, 2");
    }
    
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(exists);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(exists);
}
    
// Test: check OSQuery::fill interface
void test_OSQuery_fill()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai')");
    
    OSQuery _query(_database);
    int _id = 1;
    std::string _name = "xiaoyu";
    std::string _address = "CUC";
    Person _personObject(_id, _name, _address);
    _query.fill(_personObject);
    
    if (_personObject._id!=1 || _personObject._name!="steven" || _personObject._address!="shanghai") {
        throw OSException("Failed");
    }
    
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(fill);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(fill);
}
    
// Test: check OSQuery::update interface
void test_OSQuery_update()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai'); insert into Person(id, name, address)  values(2, 'kevin', 'beijing')");
    
    OSQuery _query(_database);
    int _id = 1;
    std::string _name = "xiaoyu";
    std::string _address = "CUC";
    Person _personObject(_id, _name, _address);
    _query.update(_personObject);
    
    auto resultVec = _statement.executeRows<int, std::string, std::string>("select * from Person");
    
    if (resultVec.size()!=2) {
        throw OSException("Failed,0");
    }
    
    auto _tuple1 = resultVec[0];
    if (std::get<0>(_tuple1)!=_id || std::get<1>(_tuple1)!=_name || std::get<2>(_tuple1)!=_address) {
        throw OSQLite::OSException("Failed, 1");
    }
    
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(update);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(update);
}
    
// Test: check OSQuery::saveOrUpdate interface
void test_OSQuery_saveOrUpdate()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'steven', 'shanghai')");
    
    OSQuery _query(_database);
    int _id = 1;
    std::string _name = "xiaoyu";
    std::string _address = "CUC";
    Person _personObject(_id, _name, _address);
    _query.saveOrUpdate(_personObject);
    
    auto resultVec = _statement.executeRows<int, std::string, std::string>("select * from Person");
    
    if (resultVec.size()!=1) {
        throw OSException("Failed,0");
    }
    
    auto _tuple1 = resultVec[0];
    if (std::get<0>(_tuple1)!=_id || std::get<1>(_tuple1)!=_name || std::get<2>(_tuple1)!=_address) {
        throw OSQLite::OSException("Failed, 1");
    }
    
    _id = 2;
    Person _personObject2(_id, _name, _address);
    _query.saveOrUpdate(_personObject2);
    auto resultVec2 = _statement.executeRows<int, std::string, std::string>("select * from Person where id='2'");
    
    if (resultVec2.size()!=1) {
        throw OSException("Failed,2");
    }
    
    auto _tuple2 = resultVec2[0];
    if (std::get<0>(_tuple2)!=_id || std::get<1>(_tuple2)!=_name || std::get<2>(_tuple2)!=_address) {
        throw OSQLite::OSException("Failed, 3");
    }
    
    _statement.execute("drop table Person");
    
    TEST_SUCCESS(saveOrUpdate);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(saveOrUpdate);
}

// Test: check OSQuery::deleteObject interface
void test_OSQuery_deleteObject()
try {
    using namespace OSQLite;
    OSDatabase _database(databaseFilePath);
    OSStatement _statement(_database);
    _statement.execute("create table if not exists Person(id integer not null, name varchar(56), address text, primary key(id))");
    _statement.execute("insert into Person(id, name, address) values(1, 'xiaoyu', 'CUC')");
    
    OSQuery _query(_database);
    int _id = 1;
    std::string _name = "xiaoyu";
    std::string _address = "CUC";
    Person _personObject(_id, _name, _address);
    _query.deleteObject(_personObject);
    
    _statement.execute("drop table Person");
        
    TEST_SUCCESS(deleteObject);
} catch (const OSQLite::OSException& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    TEST_FAIL(deleteObject);
}

int main(int argc, const char * argv[]) {

	// On my Macbook:
	// Performance: 1000 test suite loops, 11000 open&close ops, 50000 sql access->21.775s

	std::cout << "Test... OSDatabase" << std::endl;
	test_OSDatabase_ctors_dtors();

	std::cout << "Test... OSStatement" << std::endl;
	test_OSStatement_execute();
	test_OSStatement_executeRows();
	test_OSStatement_executeScalar();
	test_OSStatement_transactions();

	std::cout << "Test... OSQuery and OSTablePolicy tests" << std::endl;
	test_OSQuery_save();
	test_OSQuery_exists();
	test_OSQuery_fill();
	test_OSQuery_update();
	test_OSQuery_saveOrUpdate();
	test_OSQuery_deleteObject();

    return 0;
}
