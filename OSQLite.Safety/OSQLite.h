//
//  OSQLite.h
//  OSQLite
//
//  Created by Steven Chang on 15/7/25.
//  Copyright (c) 2015 Steven Chang. All rights reserved.
//

#pragma once
// C++ Standard Library
#include <iostream>
#include <sstream>
#include <exception>
#include <utility>
// STL Containers
#include <vector>
#include <unordered_map>
#include <tuple>

#include <assert.h>

#ifndef _MSC_VER
// XCODE
#include "sqlite3.h"

#	if !(__has_feature(cxx_noexcept))
#		define noexcept throw()
#	endif
#else
// VS
#include "..\SQLite\sqlite3.h"// SQLite
#pragma warning (disable:4290)// exception specification
#	if _MSC_VER <= 1800
//		noexcept
#		define noexcept throw()
#	endif
#endif
/*
 INTERFACES
 
 class OSException : public std::exception {
 public:
    OSException() noexcept;
    OSException(const char* content) noexcept;
    OSException(const char* content, unsigned tag) noexcept;
    virtual ~OSException() noexcept;
    virtual const char* what() const noexcept;
    virtual const unsigned tag() const noexcept;
 };
 
 class OSTablePolicy {
 public:
    bool checkBinding();
 
    inline void bind_table_name(const std::string& tableName);
    template <typename Tp> 
    inline void bind_primary_key(const std::string& key_name, Tp& mem_var);
    template <typename Tp> 
    inline void bind_key(const std::string& key_name, Tp& mem_var);
 };
 
 template <class _DerivedCLS_>
 class OSQuery {
 public:
    OSQuery(const OSDatabase& database) throw(OSException);
    virtual ~OSQuery();
 
    template <typename Table> void save(Table& table) throw(OSException);
    template <typename Table> void exists(Table& table) throw(OSException);
    template <typename Table> void fill(Table& table) throw(OSException);
    template <typename Table> void update(Table& table) throw(OSException);
    template <typename Table> void saveOrUpdate(Table& table) throw(OSException);
    template <typename Table> void deleteObject(Table& table) throw(OSException);
 };
 
 class OSStatement {
 public:
    OSStatement(const OSDatabase& database) throw(OSException);
    virtual ~OSStatement();
 
    template <typename... Args> 
    void execute(const std::string& sqlString, Args&...) throw(OSException);
    void execute(const std::string& sqlString) throw(OSException);
 
    template <typename... Returns, typename... Args>
    std::vector<std::tuple<Returns...>> executeRows(const std::string& sqlString, Args&...) throw (OSException);
 
    template <typename R, typename... Args>
    R executeScalar(const std::string& sqlString, Args&...) throw(OSException);
 };

 class OSDatabase {
 public:
    OSDatabase(const std::string& dbName) throw(OSException);
    virtual ~OSDatabase();
 };
 
 
 */

// Namespace
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
    private:
        std::string _content;
        const unsigned _tag;
    };
    
    /*
     *  OSTablePolicy. Policy class for run-time key bindings for tables.
     *  Thus, RTTI is needed.
     *  Inherit this class to perform object operations:
     *  e.g. clas DerivedObject : public OSTablePolicy<DerivedObject>
     *  The template is to assure type-only.
     */
    template <class _DerivedCLS_>
    class OSTablePolicy {
        friend class OSQuery;
        
        // Binding storage. Similiar to boost::any.
        struct OSPlaceHolder {
            virtual ~OSPlaceHolder() {}
            virtual const std::type_info& type() const noexcept = 0;
        };
        template <class Any> struct OSKeyHolder : public OSPlaceHolder {
            OSKeyHolder(Any& content_):_content(content_) {}
            virtual ~OSKeyHolder() {}
            const std::type_info& type() const noexcept {return typeid(Any);}
            Any& _content;// Store a const reference
        };
        
        std::string _tableName = "";
        std::string _primaryKeyName = "";
        OSPlaceHolder* _primaryKeyHolder = nullptr;
        std::unordered_map<std::string, OSPlaceHolder*> _keyBindingMap;
        
    protected:
        OSTablePolicy();
        OSTablePolicy(const OSTablePolicy&) = delete;
        OSTablePolicy<_DerivedCLS_> operator=(const OSTablePolicy&) = delete;
        virtual ~OSTablePolicy();
        
    public:
        // Check if bindings are acceptable (table name, primary key, at least one key)
        bool checkBinding();
        
        // Key binding functions
        inline void bind_table_name(const std::string& tableName);
        
        template <typename Tp>
        inline typename std::enable_if<!std::is_pointer<Tp>::value, void>::type bind_primary_key(const std::string& keyName, Tp& memVar);
        
        template <typename Tp>
        inline typename std::enable_if<!std::is_pointer<Tp>::value, void>::type bind_key(const std::string& keyName, Tp& memVar);
        
    private:
        // Extract data from OSKeyHolder into SQL (at run-time)
        inline void insertDataToSQL(std::stringstream& stringStream, OSPlaceHolder* holder) throw(OSException);
        // Extract data form database to OSKeyHolder value reference (at run-time)
        inline void extractDataFromDB(sqlite3_stmt* statement, OSPlaceHolder* holder, int index) throw(OSException);
    };
    
    /*
     *  OSQuery class, execute SQL with an object-oriented operations.
     *  Similar to Hibernate in Java.
     *  Provide generic functions to operate on objects.
     */
    class OSQuery {
        
        sqlite3* const& _connection;
        
    public:
        OSQuery(const OSDatabase& database) throw(OSException);
        OSQuery(const OSQuery&) = delete;
        OSQuery operator=(const OSQuery&) = delete;
        virtual ~OSQuery();
        
        /* Generic functions: 
         * save: insert the object to the table.
         * exists: check if the object (given primary key) exists in database. return bool.
         * fill: query and assign other key values given primary key.
         * update: update data given primary key.
         * savaOrUpdate: if the record exists (given primary key), then update; or save.
         * deleteObject: delete object given primary key.
         */
        template <typename Table> typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type save(Table& table) throw(OSException);
        template <typename Table> typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, bool>::type exists(Table& table) throw(OSException);
        template <typename Table> typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type fill(Table& table) throw(OSException);
        template <typename Table> typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type update(Table& table) throw(OSException);
        template <typename Table> typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type saveOrUpdate(Table& table) throw(OSException);
        template <typename Table> typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type deleteObject(Table& table) throw(OSException);
    };
    
    
    
    /*
     *  OSStatement, SQL statement. It can execute SQL operations with/without 
     *  parameter bindings, fitting for Create/Insert/Delete/Update/Query operations.
     */
    class OSStatement {
        
        sqlite3* const& _connection;
        
        sqlite3_stmt* _statement = nullptr;
        
    public:
        OSStatement(const OSDatabase& database) throw(OSException);
        OSStatement(const OSStatement&) = delete;
        OSStatement operator=(const OSStatement&) = delete;
        virtual ~OSStatement();
        
        // Unified execute function, and throw OSException if got a failure.
        // No returns, sql execution with bindings
        template <typename... Args>
        void execute(const std::string& sqlString, Args&...) throw(OSException);
        // No return, no bindings; just execute SQL.
        void execute(const std::string& sqlString) throw(OSException);
        
        // Returns a std::vector containing tuples, each tuple is a block of data(row)
        template <typename... Returns, typename... Args>
        std::vector<std::tuple<Returns...>> executeRows(const std::string& sqlString, Args&...) throw (OSException);
        
        // Also we provide one interface executing SQL and return a scalar
        // value, e.g. select count(*). They will throw OSException if
        // got a failure.
        template <typename R, typename... Args>
        R executeScalar(const std::string& sqlString, Args&...) throw(OSException);
    };
    
    /*
     *  OSDatabase class, control the access to SQLite database.
     *  Follow the RAII principle, the database is opened and closed when you
     *  instanciate and release the OSDatabase object.
     */
    class OSDatabase {
        friend class OSStatement;
        friend class OSQuery;
        
        // SQLite connection. NOTICE the exception safety.
        sqlite3* _connection = nullptr;
        
    public:
        OSDatabase(const std::string& dbName) throw(OSException);
        OSDatabase(const OSDatabase&) = delete;
        OSDatabase operator=(const OSDatabase&&) = delete;
        virtual ~OSDatabase();
    };
    
}
#include "OSQLite.inl"
