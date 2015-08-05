//
//  InterfacesDisplay.h
//  OSQLite
//
//  Created by Steven Chang on 15/8/4.
//  Copyright (c) 2015 Steven Chang. All rights reserved.
//
//  Display all interfaces you can call. Comments are added.
//  Do not include this header; just for displaying.

// some defines used as OSStatement begin transaction modifiers
#define BEGIN_DEFERRED
#define BEGIN_IMMEDIATE
#define BEGIN_EXCLUSIVE
#define BEGIN_NONE

namespace OSQLite {
    class OSException;
    template <class _Derived_>
    class OSTablePolicy;
    class OSQuery;
    class OSStatement;
    class OSDatabase;
    
    /*
     *  OSException class, inherited from std::exception.
     */
    class OSException : public std::exception {
    public:
        OSException() noexcept;
        OSException(const char* content) noexcept;
        OSException(const char* content, unsigned tag) noexcept;
        virtual ~OSException() noexcept;
        virtual const char* what() const noexcept;
        virtual const unsigned tag() const noexcept;
    };
    
    /*
     *  OSTablePolicy. Policy class at compile-time key bindings for tables.
     *  Inherit this class to perform object operations:
     *  e.g. clas DerivedObject : public OSTablePolicy<DerivedObject>
     *  The template is to assure type-only.
     *  Use inheritance ctor like CTOR():OSTablePolicy("Person", {"id", "name"}, _id, _name)
     *  Binding variable type supported: int, unsigned int, long, unsigned long, 
     *  float, double, std::string
     */
    template <_DerivedCLS_>
    class OSTablePolicy {
    protected:
        // Bind keys when constructing
        template <typename... Args>
        OSTablePolicy(const std::string& tableName, std::initializer_list<std::string> keyNamesList_, Args&... args) throw(OSException);
        virtual ~OSTablePolicy();
        
    public:
        // A function you can check if your bindings are acceptable.
        bool checkBindings();
    };
    
    /*
     *  OSQuery class, execute SQL with an object-oriented operations.
     *  Similar to Hibernate in Java.
     *  Provide generic functions to operate on objects.
     */
    class OSQuery {
    public:
        OSQuery(const OSDatabase& database) throw(OSException);
        virtual ~OSQuery();
        
        /* Generic functions:
         * save: insert the object to the table.
         * exists: check if the object (given primary key) exists in database. return bool.
         * fill: query and assign other key values given primary key.
         * update: update data given primary key.
         * savaOrUpdate: if the record exists (given primary key), then update; or save.
         * deleteObject: delete object given primary key.
         */
        template <typename Table> void save(Table& table) throw(OSException);
        template <typename Table> bool exists(Table& table) throw(OSException);
        template <typename Table> bool fill(Table& table) throw(OSException);
        template <typename Table> void update(Table& table) throw(OSException);
        template <typename Table> void saveOrUpdate(Table& table) throw(OSException);
        template <typename Table> void deleteObject(Table& table) throw(OSException);
    };
    
    /*
     *  OSStatement, SQL statement. It can execute SQL operations with/without
     *  parameter bindings, fitting for Create/Insert/Delete/Update/Query operations.
     */
    class OSStatement {
    public:
        OSStatement(const OSDatabase& database) throw(OSException);
        virtual ~OSStatement();
        
        // Unified execute function, and throw OSException if got a failure.
        // No returns, sql execution with bindings
        template <typename... Args>
        void execute(const std::string& sqlString, Args&...) throw(OSException);
        // No return, no bindings; just execute SQL.
        void execute(const std::string& sqlString) throw(OSException);
        
        // Returns a std::vector containing tuples, each tuple is a row of data
        template <typename... Returns, typename... Args>
        std::vector<std::tuple<Returns...>> executeRows(const std::string& sqlString, Args&...) throw (OSException);
        
        // Also we provide one interface executing SQL and return a scalar
        // value, e.g. select count(*). They will throw OSException if
        // got a failure.
        template <typename R, typename... Args>
        R executeScalar(const std::string& sqlString, Args&...) throw(OSException);
        
        // Interfaces for transactions
        inline void begin() throw(OSException);
        inline void begin(const std::string& beginArg) throw(OSException);
        inline void commit() throw(OSException);
        inline void rollback() throw(OSException);
    };
    
    /*
     *  OSDatabase class, control the access to SQLite database.
     *  Follow the RAII principle, the database is opened and closed when you
     *  instanciate and release the OSDatabase object.
     */
    class OSDatabase {
    public:
        OSDatabase(const std::string& dbName) throw(OSException);
        virtual ~OSDatabase();
    };
}