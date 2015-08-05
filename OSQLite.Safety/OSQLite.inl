//
//  OSQLite.cpp
//  OSQLite.Safety
//
//  Created by Steven Chang on 15/7/25.
//  Copyright (c) 2015 Steven Chang. All rights reserved.
//

#include "OSQLite.h"


namespace OSQLite {
    
    // Define a templated struct named OSTypeOp (inherited from OSPlaceholder), used
    // to encapsulate type bindings from database to clients, or vice versa. (at compile-time)
    //
    // Funtions:
    //      ctors: used for OSTablePolicy class.
    //      statementReturnAssign: extract data from database to the tuple
    //      statementParamBinding: bind parameters to sql - for OSStatement operations
    //      queryReturnAssign: OSQuery operations extracting data
    //      queryParamBinding: OSQuery operations bind params
    //      queryPrimaryKey: OSQuery operations insert primary key to sql string
    //
    // All functions may throw OSException.
    // Providing types including: int, unsigned int, long, unsigned long, std::string
	struct OSPlaceHolder {
        OSPlaceHolder(){}
        virtual ~OSPlaceHolder(){}
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) = 0;
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) = 0;
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) = 0;
    };
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp : virtual public OSPlaceHolder {
        
        OSTypeOp(Args&... args_){ throw OSException("Unregisted type. Please check first."); }
		virtual ~OSTypeOp(){}

        // If this function called, throw an error
        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            throw OSException("OSTypeOp error: statementReturnAssign: invalid type.");
        }
        // If this function called, throw an error
        static inline void statementParamBinding(sqlite3_stmt* statement_, Args&...) {
            throw OSException("OSTypeOp error: statementParamBinding: invalid type.");
        }
        // If this function called, throw an error
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            throw OSException("OSTypeOp error: queryReturnAssign: invalid type.");
        }
        // If this function called, throw an error
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            throw OSException("OSTypeOp error: queryParamBinding: invalid type.");
        }
        // If this function called, throw an error
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            throw OSException("OSTypeOp error: queryPrimaryKey: invalid type.");
        }
    };
    // int
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp<NUM, int, Args...> : virtual public OSPlaceHolder {
        OSTypeOp<NUM+1, Args...> _next;
        int& _ref;
        
        OSTypeOp(int& iValue_, Args&... args_):_ref(iValue_), _next(args_...){}
		virtual ~OSTypeOp(){}

        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = sqlite3_column_int(statement_, NUM);
            OSTypeOp<NUM+1, Args...>::statementReturnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        static inline void statementParamBinding(sqlite3_stmt* statement_, int& iValue_, Args&... args_) {
            int _result = sqlite3_bind_int(statement_, NUM+1, iValue_);
            if (_result != SQLITE_OK) {
                throw OSException("statementParamBinding error. Bind int failed.", _result);
            }
            OSTypeOp<NUM+1, Args...>::statementParamBinding(statement_, args_...);
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            _ref = sqlite3_column_int(statement_, NUM);
            _next.queryReturnAssign(statement_);
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            int _result = sqlite3_bind_int(statement_, NUM+1, _ref);
            if (_result != SQLITE_OK) {
                throw OSException("queryParamBinding error. Bind int failed.", _result);
            }
            _next.queryParamBinding(statement_);
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            sqlStream_ << _ref;
        }
    };
    // unsigned int
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp<NUM, unsigned int, Args...> : virtual public OSPlaceHolder {
        OSTypeOp<NUM+1, Args...> _next;
        unsigned int& _ref;
        
        OSTypeOp(unsigned int& iValue_, Args&... args_):_ref(iValue_), _next(args_...){}
		virtual ~OSTypeOp(){}

        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (unsigned int)sqlite3_column_int(statement_, NUM);
            OSTypeOp<NUM+1, Args...>::statementReturnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        static inline void statementParamBinding(sqlite3_stmt* statement_, unsigned int& iValue_, Args&... args_) {
            int _result = sqlite3_bind_int(statement_, NUM+1, iValue_);
            if (_result != SQLITE_OK) {
                throw OSException("statementParamBinding error. Bind unsigned int failed.", _result);
            }
            OSTypeOp<NUM+1, Args...>::statementParamBinding(statement_, args_...);
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            _ref = (unsigned int)sqlite3_column_int(statement_, NUM);
            _next.queryReturnAssign(statement_);
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            int _result = sqlite3_bind_int(statement_, NUM+1, _ref);
            if (_result != SQLITE_OK) {
                throw OSException("queryParamBinding error. Bind unsigned int failed.", _result);
            }
            _next.queryParamBinding(statement_);
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            sqlStream_ << _ref;
        }
    };
    // long
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp<NUM, long, Args...> : virtual public OSPlaceHolder {
        OSTypeOp<NUM+1, Args...> _next;
        long& _ref;
        
        OSTypeOp(long& iValue_, Args&... args_):_ref(iValue_), _next(args_...){}
		virtual ~OSTypeOp(){}

        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = sqlite3_column_int64(statement_, NUM);
            OSTypeOp<NUM+1, Args...>::statementReturnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        static inline void statementParamBinding(sqlite3_stmt* statement_, long& lValue_, Args&... args_) {
            int _result = sqlite3_bind_int64(statement_, NUM+1, lValue_);
            if (_result != SQLITE_OK) {
                throw OSException("statementParamBinding error. Bind long failed.", _result);
            }
            OSTypeOp<NUM+1, Args...>::statementParamBinding(statement_, args_...);
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            _ref = (long)sqlite3_column_int64(statement_, NUM);
            _next.queryReturnAssign(statement_);
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            int _result = sqlite3_column_int64(statement_, NUM+1, _ref);
            if (_result != SQLITE_OK) {
                throw OSException("queryParamBinding error. Bind long failed.", _result);
            }
            _next.queryParamBinding(statement_);
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            sqlStream_ << _ref;
        }
    };
    // unsigned long
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp<NUM, unsigned long, Args...> : virtual public OSPlaceHolder {
        OSTypeOp<NUM+1, Args...> _next;
        unsigned long& _ref;
        
        OSTypeOp(unsigned long& iValue_, Args&... args_):_ref(iValue_), _next(args_...){}
		virtual ~OSTypeOp(){}

        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (unsigned long)sqlite3_column_int(statement_, NUM);
            OSTypeOp<NUM+1, Args...>::statementReturnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        static inline void statementParamBinding(sqlite3_stmt* statement_, unsigned long& lValue_, Args&... args_) {
            int _result = sqlite3_bind_int64(statement_, NUM+1, lValue_);
            if (_result != SQLITE_OK) {
                throw OSException("statementParamBinding error. Bind unsigned long failed.", _result);
            }
            OSTypeOp<NUM+1, Args...>::statementParamBinding(statement_, args_...);
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            _ref = (unsigned long)sqlite3_column_int64(statement_, NUM);
            _next.queryReturnAssign(statement_);
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            int _result = sqlite3_column_int64(statement_, NUM+1, _ref);
            if (_result != SQLITE_OK) {
                throw OSException("queryParamBinding error. Bind unsigned long failed.", _result);
            }
            _next.queryParamBinding(statement_);
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            sqlStream_ << _ref;
        }
    };
    // float. Warning: might lose data.
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp<NUM, float, Args...> : virtual public OSPlaceHolder {
        OSTypeOp<NUM+1, Args...> _next;
        float& _ref;
        
        OSTypeOp(float& iValue_, Args&... args_):_ref(iValue_), _next(args_...){}
		virtual ~OSTypeOp(){}

        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (float)(sqlite3_column_double(statement_, NUM));
            OSTypeOp<NUM+1, Args...>::statementReturnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        static inline void statementParamBinding(sqlite3_stmt* statement_, float& fValue_, Args&... args_) {
            int _result = sqlite3_bind_double(statement_, NUM+1, fValue_);
            if (_result != SQLITE_OK) {
                throw OSException("statementParamBinding error. Bind float failed.", _result);
            }
            OSTypeOp<NUM+1, Args...>::statementParamBinding(statement_, args_...);
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            _ref = (float)(sqlite3_column_double(statement_, NUM));
            _next.queryReturnAssign(statement_);
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            int _result = sqlite3_bind_double(statement_, NUM+1, _ref);
            if (_result != SQLITE_OK) {
                throw OSException("queryParamBinding error. Bind float failed.", _result);
            }
            _next.queryParamBinding(statement_);
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            sqlStream_ << _ref;
        }
    };
    // double
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp<NUM, double, Args...> : virtual public OSPlaceHolder {
        OSTypeOp<NUM+1, Args...> _next;
        double& _ref;
        
        OSTypeOp(double& iValue_, Args&... args_):_ref(iValue_), _next(args_...){}
		virtual ~OSTypeOp(){}

        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (double)sqlite3_column_double(statement_, NUM);
            OSTypeOp<NUM+1, Args...>::statementReturnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        static inline void statementParamBinding(sqlite3_stmt* statement_, double& dValue_, Args&... args_) {
            int _result = sqlite3_bind_double(statement_, NUM+1, dValue_);
            if (_result != SQLITE_OK) {
                throw OSException("statementParamBinding error. Bind double failed.", _result);
            }
            OSTypeOp<NUM+1, Args...>::statementParamBinding(statement_, args_...);
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            _ref = (double)(sqlite3_column_double(statement_, NUM));
            _next.queryReturnAssign(statement_);
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            int _result = sqlite3_bind_double(statement_, NUM+1, _ref);
            if (_result != SQLITE_OK) {
                throw OSException("queryParamBinding error. Bind double failed.", _result);
            }
            _next.queryParamBinding(statement_);
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            sqlStream_ << _ref;
        }
    };
    // std::string
    template <unsigned char NUM, typename... Args>
    struct OSTypeOp<NUM, std::string, Args...> : virtual public OSPlaceHolder {
        OSTypeOp<NUM+1, Args...> _next;
        std::string& _ref;
        
        OSTypeOp(std::string& iValue_, Args&... args_):_ref(iValue_), _next(args_...){}
		virtual ~OSTypeOp(){}

        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::string sValue((const char*)sqlite3_column_text(statement_, NUM));
            std::get<NUM>(tuple_) = sValue;
            OSTypeOp<NUM+1, Args...>::statementReturnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        static inline void statementParamBinding(sqlite3_stmt* statement_, std::string& sValue_, Args&... args_) {
            int _result = sqlite3_bind_text(statement_, NUM+1, sValue_.c_str(), (int)sValue_.length(), SQLITE_TRANSIENT);
            if (_result != SQLITE_OK) {
                throw OSException("statementParamBinding error. Bind std::string failed.", _result);
            }
            OSTypeOp<NUM+1, Args...>::statementParamBinding(statement_, args_...);
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            std::string sValue((const char*)sqlite3_column_text(statement_, NUM));
            _ref = sValue;
            _next.queryReturnAssign(statement_);
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            int _result = sqlite3_bind_text(statement_, NUM+1, _ref.c_str(), (int)_ref.length(), SQLITE_TRANSIENT);
            if (_result != SQLITE_OK) {
                throw OSException("queryParamBinding error. Bind std::string failed.", _result);
            }
            _next.queryParamBinding(statement_);
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            sqlStream_ << _ref;
        }
    };
    
    template <unsigned char NUM>
    struct OSTypeOp<NUM> : virtual public OSPlaceHolder {
        
        OSTypeOp() {}
		virtual ~OSTypeOp(){}
        
        template <typename... Returns>
        static inline void statementReturnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            // End of recursive type binding. Do nothing.
        }
        
        static inline void statementParamBinding(sqlite3_stmt* statement_) {
            // End of recursive type binding. Do nothing.
        }
        
        virtual inline void queryReturnAssign(sqlite3_stmt* statement_) override {
            // End of recursive type binding. Do nothing.
        }
        
        virtual inline void queryParamBinding(sqlite3_stmt* statement_) override {
            // End of recursive type binding. Do nothing.
        }
        
        virtual inline void queryPrimaryKey(std::stringstream& sqlStream_) override {
            // End of recursive type binding. Do nothing.
        }
    };
    
    
    
    
    
    // Functions for OSException
    OSException::OSException() noexcept : _content("OSLite exception"), _tag(0)
    {}
    
    OSException::OSException(const char* content_) noexcept : _content(content_), _tag(0)
    {}
    
    OSException::OSException(const char* content_, unsigned tag_) noexcept : _content(content_), _tag(tag_)
    {}
    
    OSException::~OSException() noexcept
    {}
    
    const char* OSException::what() const noexcept
    {return _content.c_str();}
    
    const unsigned OSException::tag() const noexcept
    {return _tag;}
    
    
    
    
    // Functions for OSTablePolicy
    template <class _DerivedCLS_>
    template <typename... Args>
    OSTablePolicy<_DerivedCLS_>::OSTablePolicy(const std::string& tableName_, std::initializer_list<std::string> keyNamesList_, Args&... args_) throw(OSException)
    {
        if (!_hasBindings) {
            _tableName = tableName_;
            _keyNameVec = keyNamesList_;
            _hasBindings = true;
        }
        _keyReference = new OSTypeOp<0, Args...>(args_...);
    }
    
    template <class _DerivedCLS_>
    OSTablePolicy<_DerivedCLS_>::~OSTablePolicy()
    {
        if (_keyReference != nullptr) {
            delete _keyReference;
            _keyReference = nullptr;
        }
    }
    
    template <class _DerivedCLS_>
    bool OSTablePolicy<_DerivedCLS_>::checkBindings()
    {
        return _hasBindings && _keyNameVec.size()>1;
    }
    
    
    
    // Functions for OSQuery
    OSQuery::OSQuery(const OSDatabase& database_) throw(OSException) : _connection(database_._connection)
    {
        if (_connection == nullptr) {
            throw OSException("OSStatement ctor error: SQLite connection is not opened.");
        }
    }
    
    OSQuery::~OSQuery()
    {
        if (_statement != nullptr) {
            sqlite3_finalize(_statement);
            _statement = nullptr;
        }
    }
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type OSQuery::save(Table& table_) throw(OSException)
    {
        try {
            // Check the acceptance of table binding
            if (!table_.checkBindings()) {
                throw OSException("save error: table binding is not acceptable.");
            }
            
            // Construct the SQL string and fill in data
            std::string _sqlString = "insert into " + table_._tableName + "(";
            std::string _endString = " values(";
            for (auto& _str : table_._keyNameVec) {
                _sqlString += _str;
                _sqlString +=",";
                _endString +="?,";
            }
            _sqlString[_sqlString.size()-1] = ')';
            _endString[_endString.size()-1] = ')';
            _sqlString += _endString;
            
            // Prepare statement first.
            int _result = sqlite3_prepare_v2(_connection, _sqlString.c_str(), (int)_sqlString.size(), &_statement, nullptr);
            if (_result != SQLITE_OK) {
                throw OSException("save error: Cannot prepare the sqlite3_stmt.", _result);
            }
            
            // Parameter binding.
            table_._keyReference->queryParamBinding(_statement);
            
            // Execute
            _result = sqlite3_step(_statement);
            if (_result != SQLITE_DONE) {
                throw OSException("save error. Execute SQLString failed.", _result);
            }
            
            sqlite3_finalize(_statement);
            _statement = nullptr;
        } catch (const OSException&) {
            sqlite3_finalize(_statement);
            _statement = nullptr;
            throw;
        }
    }
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, bool>::type OSQuery::exists(Table& table_) throw(OSException)
    {
        // Check the acceptance of table binding
        if (!table_.checkBindings()) {
            throw OSException("exists error: table binding is not acceptable.");
        }
        
        // Process select count operations to check if the data exists
        // Construct the SQL string and fill in data directly
        std::stringstream _sqlStream;
        _sqlStream << "select count(*) from " << table_._tableName << " where " << table_._keyNameVec[0] << "='";
        table_._keyReference->queryPrimaryKey(_sqlStream);
        _sqlStream << "'";
        std::string _sqlString = _sqlStream.str();
        
        // Execute
        int _result = sqlite3_exec(_connection, _sqlString.c_str(), [](void*,int,char** iptr2,char**){return ((**iptr2)=='0')-1;}, nullptr, nullptr);
        if (_result == SQLITE_ABORT) {
            // Exists.
            return true;
        } else if (_result == SQLITE_OK) {
            // Not exists.
            return false;
        } else {
            throw OSException("exists error. sqlite3_exec execution failed.", _result);
        }
    }
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, bool>::type OSQuery::fill(Table& table_) throw(OSException)
    {
        try {
            // Check the acceptance of table binding
            if (!table_.checkBindings()) {
                throw OSException("fill error: table binding is not acceptable.");
            }
            
            // Construct SQL string
            std::stringstream _sqlStream;
            _sqlStream << "select ";
            for (auto& _str : table_._keyNameVec) {
                _sqlStream << _str << ",";
            }
            std::string _sqlString = _sqlStream.str();
            _sqlString[_sqlString.size()-1] = ' ';
            _sqlStream.str("");
            _sqlStream << "from " << table_._tableName << " where " << table_._keyNameVec[0] << "='";
            table_._keyReference->queryPrimaryKey(_sqlStream);
            _sqlStream << "'";
            _sqlString += _sqlStream.str();
            
            // Prepare statement first.
            int _result = sqlite3_prepare_v2(_connection, _sqlString.c_str(), (int)_sqlString.size(), &_statement, nullptr);
            if (_result != SQLITE_OK) {
                throw OSException("fill error: Cannot prepare the sqlite3_stmt.", _result);
            }
            
            // Get value.
            int _colCount = sqlite3_column_count(_statement);
            assert(_colCount == table_._keyNameVec.size());
            _result = sqlite3_step(_statement);
            if (_result == SQLITE_DONE) {
                sqlite3_finalize(_statement);
                _statement = nullptr;
                return false;
            }
            if (_result != SQLITE_ROW) {
                throw OSException("fill error: step error", _result);
            }
            table_._keyReference->queryReturnAssign(_statement);
            sqlite3_finalize(_statement);
            _statement = nullptr;
            return true;
            
        } catch (const OSException&) {
            sqlite3_finalize(_statement);
            _statement = nullptr;
            throw;
        }
    }
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type OSQuery::update(Table& table_) throw(OSException)
    {
        try {
            // Check the acceptance of table binding
            if (!table_.checkBindings()) {
                throw OSException("update error: table binding is not acceptable.");
            }
            
            // Construct the SQL string and fill in data
            std::stringstream _sqlStream;
            _sqlStream << "update  " << table_._tableName << " set ";
            for (auto& _str : table_._keyNameVec) {
                _sqlStream << _str << "=?,";
            }
            std::string _sqlString = _sqlStream.str();
            _sqlString[_sqlString.size()-1] = ' ';
            _sqlStream.str("");
            _sqlStream << " where " << table_._keyNameVec[0] << "='";
            table_._keyReference->queryPrimaryKey(_sqlStream);
            _sqlStream << "'";
            _sqlString += _sqlStream.str();
            
            // Prepare statement first.
            int _result = sqlite3_prepare_v2(_connection, _sqlString.c_str(), (int)_sqlString.size(), &_statement, nullptr);
            if (_result != SQLITE_OK) {
                throw OSException("update error: Cannot prepare the sqlite3_stmt.", _result);
            }
            
            // Parameter binding.
            table_._keyReference->queryParamBinding(_statement);
            
            // Execute
            _result = sqlite3_step(_statement);
            if (_result != SQLITE_DONE) {
                throw OSException("update error. Execute SQLString failed.", _result);
            }
            
            sqlite3_finalize(_statement);
            _statement = nullptr;
        } catch(const OSException&) {
            sqlite3_finalize(_statement);
            _statement = nullptr;
            throw;
        }
    }
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type OSQuery::saveOrUpdate(Table& table_) throw(OSException)
    try {
        // check the existance.
        if (this->exists(table_)) {
            // Exists. Perform update.
            this->update(table_);
        } else {
            // Not exists. Perform save.
            this->save(table_);
        }
    } catch (const OSException& e) {
        std::string _oldExceptionStr(e.what());
        std::string _newExceptionStr = "saveOrUpdate error. " + _oldExceptionStr;
        throw OSException(_newExceptionStr.c_str(), e.tag());
    }
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type OSQuery::deleteObject(Table& table_) throw(OSException)
    {
        // Check the acceptance of table binding
        if (!table_.checkBindings()) {
            throw OSException("update error: table binding is not acceptable.");
        }
        
        // Construct the SQL string and fill in data directly
        std::string _sqlString = "delete from " + table_._tableName + " where ";
        _sqlString += table_._keyNameVec[0];
        _sqlString += "='";
        std::stringstream _sqlStream;
        table_._keyReference->queryPrimaryKey(_sqlStream);
        _sqlString += "'";
        
        // Execute
        int _result = sqlite3_exec(_connection, _sqlString.c_str(), nullptr, nullptr, nullptr);
        if (_result != SQLITE_OK) {
            throw OSException("update error. sqlite3_exec execution failed.", _result);
        }
    }
    
    
    
    
    // Functions for OSStatement
    OSStatement::OSStatement(const OSDatabase& database_) throw(OSException) : _connection(database_._connection)
    {
        if (_connection == nullptr) {
            throw OSException("OSStatement ctor error: SQLite connection is not opened.");
        }
    }
    
    OSStatement::~OSStatement()
    {
        if (_statement != nullptr) {
            sqlite3_finalize(_statement);
            _statement = nullptr;
        }
    }
    
    template <typename... Args>
    void OSStatement::execute(const std::string& sqlString_, Args&... args_) throw(OSException)
    try {
        // Prepare statement first.
        int _result = sqlite3_prepare_v2(_connection, sqlString_.c_str(), (int)sqlString_.length(), &_statement, nullptr);
        if (_result != SQLITE_OK) {
            throw OSException("execute error: Cannot prepare the sqlite3_stmt.", _result);
        }
        
        // Parameter binding.
        OSTypeOp<0, Args...>::statementParamBinding(_statement, args_...);
        
        // Execute
        _result = sqlite3_step(_statement);
        if (_result != SQLITE_DONE) {
            throw OSException("execute error. Execute SQLString failed.", _result);
        }
        
        sqlite3_finalize(_statement);
        _statement = nullptr;
        
    } catch (const OSException&) {
        sqlite3_finalize(_statement);
        _statement = nullptr;
        throw;
    }
    
    
    void OSStatement::execute(const std::string& sqlString_) throw(OSException)
    {
        int _result = sqlite3_exec(_connection, sqlString_.c_str(), nullptr, nullptr, nullptr);
        if (_result != SQLITE_OK) {
            throw OSException("execute error. sqlite3_exec execution failed.", _result);
        }
    }
    
    template <typename... Returns, typename... Args>
    std::vector<std::tuple<Returns...>> OSStatement::executeRows(const std::string& sqlString_, Args&... args_) throw (OSException)
    try {
        // Prepare statement first.
        int _result = sqlite3_prepare_v2(_connection, sqlString_.c_str(), (int)sqlString_.length(), &_statement, nullptr);
        if (_result != SQLITE_OK) {
            throw OSException("execute error: Cannot prepare the sqlite3_stmt.", _result);
        }
        
        // Parameter binding.
        OSTypeOp<0, Args...>::statementParamBinding(_statement, args_...);
        
        // Create a vector and a tuple instance. Get values, and assign them to
        // the tuple. Finally push the tuple back to the vector.
        std::vector<std::tuple<Returns...>> _returnVec;
        std::tuple<Returns...> _tuple;
        
        // Get value
        int _colCount = sqlite3_column_count(_statement);
        assert(_colCount == std::tuple_size<decltype(_tuple)>::value);
        while (true) {
            _result = sqlite3_step(_statement);
            if (_result == SQLITE_DONE) {
                break;
            }
            if (_result != SQLITE_ROW) {
                throw OSException("execute error: step error", _result);
            }
            
            OSTypeOp<0, Returns...>::statementReturnAssign(_tuple, _statement);
            _returnVec.push_back(_tuple);
        }
        
        sqlite3_finalize(_statement);
        _statement = nullptr;
        return _returnVec;
        
    } catch (const OSException&) {
        sqlite3_finalize(_statement);
        _statement = nullptr;
        throw;
    }
    
    
    template <typename R, typename... Args>
    R OSStatement::executeScalar(const std::string& sqlString_, Args&... args_) throw(OSException)
    try {
        // Prepare statement first.
        int _result = sqlite3_prepare_v2(_connection, sqlString_.c_str(), (int)sqlString_.length(), &_statement, nullptr);
        if (_result != SQLITE_OK) {
            throw OSException("executeScalar error: Cannot prepare the sqlite3_stmt.", _result);
        }
        
        // Parameter binding.
        OSTypeOp<0, Args...>::statementParamBinding(_statement, args_...);
        
        // Execute
        _result = sqlite3_step(_statement);
        if (_result != SQLITE_ROW) {
            throw OSException("executeScalar error. Execute SQLString failed.", _result);
        }
        std::tuple<R> _tuple;
        OSTypeOp<0, R>::statementReturnAssign(_tuple, _statement);
        
        sqlite3_finalize(_statement);
        _statement = nullptr;
        return std::get<0>(_tuple);
        
    } catch (const OSException&) {
        sqlite3_finalize(_statement);
        _statement = nullptr;
        throw;
    }
    
    inline void OSStatement::begin() throw(OSException)
    {
        this->begin("");
    }
    
    inline void OSStatement::begin(const std::string& beginArg_) throw(OSException)
    try {
        this->execute("begin" + beginArg_ );
    } catch(const OSException& e) {
        throw OSException("begin error: transaction execution failure.", e.tag());
    }
    
    inline void OSStatement::commit() throw(OSException)
    try {
        this->execute("commit");
    } catch(const OSException& e) {
        throw OSException("commit error: transaction execution failure.", e.tag());
    }
    
    inline void OSStatement::rollback() throw(OSException)
    try {
        this->execute("rollback");
    } catch(const OSException& e) {
        throw OSException("rollback error: transaction execution failure.", e.tag());
    }
    
    
    
    
    
    // Functions for OSDatabase
    OSDatabase::OSDatabase(const std::string& filePath_) throw(OSException)
    {
        _connection = nullptr;
        if (filePath_.length()==0)
            throw OSException("Invalid SQLite database file path");
        int _result = sqlite3_open(filePath_.c_str(), &_connection);
        if (_result != SQLITE_OK) {
            sqlite3_close(_connection);
            _connection = nullptr;
            throw OSException("Cannot open SQLite database file.", _result);
        }
    }
    
    OSDatabase::~OSDatabase()
    {
        if (_connection != nullptr) {
            int _result = sqlite3_close(_connection);
            if (_result != SQLITE_OK) {
                _connection = nullptr;
                throw OSException("Cannot close SQLite database", _result);
            }
        }
    }
    
}