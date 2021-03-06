//
//  OSQLite.h
//  OSQLite.Safety
//
//  The OSQLite encapsulation follows safety goal.
//  THE safety here means the safety of usage, the safety of memory and the safety of maintance.
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
#include "sqlite3.h"// SQLite
//  noexcept
#	if !(__has_feature(cxx_noexcept))
#		define noexcept throw()
#	endif
#else
// VS
#include "..\SQLite\sqlite3.h"// SQLite
#pragma warning (disable:4290)// Exception specification
#	if _MSC_VER <= 1800
//		noexcept
#		define noexcept throw()
#	endif
#endif


// some defines used as OSStatement transactions modifiers
#define BEGIN_DEFERRED std::string(" DEFERRED")
#define BEGIN_IMMEDIATE std::string(" IMMEDIATE")
#define BEGIN_EXCLUSIVE std::string(" EXCLUSIVE")
#define BEGIN_NONE std::string("")

// Namespace
namespace OSQLite {
    class OSException;
    struct OSPlaceHolder;
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
     *  Binding variable type supported: int, unsigned int, long, unsigned long,
     *  float, double, std::string
     */
    template <class _Derived_>
    class OSTablePolicy {
        friend class OSQuery;
        
        static bool _hasBindings;
        static std::string _tableName;
        static std::vector<std::string> _keyNameVec;
        OSPlaceHolder* _keyReference = nullptr;
        
    protected:
        // Bind keys when constructing
        template <typename... Args>
        OSTablePolicy(const std::string& tableName, std::initializer_list<std::string> keyNamesList_, Args&... args) throw(OSException);
        OSTablePolicy(const OSTablePolicy&) = delete;
        OSTablePolicy<_Derived_> operator=(const OSTablePolicy&) = delete;
        virtual ~OSTablePolicy();
        
    public:
        // A function you can check if your bindings are acceptable.
        bool checkBindings();
    };
    template <class _Derived_> bool OSTablePolicy<_Derived_>::_hasBindings = false;
    template <class _Derived_> std::string OSTablePolicy<_Derived_>::_tableName = "";
    template <class _Derived_> std::vector<std::string> OSTablePolicy<_Derived_>::_keyNameVec;
    
    /*
     *  OSQuery class, execute SQL with an object-oriented operations.
     *  Similar to Hibernate in Java.
     *  Provide generic functions to operate on objects.
     */
    class OSQuery {
        
        sqlite3* const& _connection;
        
        sqlite3_stmt* _statement = nullptr;
        
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
        template <typename Table> typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, bool>::type fill(Table& table) throw(OSException);
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
