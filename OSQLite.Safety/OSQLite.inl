//
//  OSQLite.cpp
//  OSQLite
//
//  Created by Steven Chang on 15/7/25.
//  Copyright (c) 2015 Steven Chang. All rights reserved.
//

#include "OSQLite.h"


namespace OSQLite {

    // Define a templated struct named OSTypeBinding, used to encapsulate type bindings
    // from database to clients, or vice versa. (at compile-time)
    //
    // returnAssign: extract data from database to the tuple
    // paramBinding: bind parameters to sql.
    //
    // All functions may throw OSException.
    // Providing types including: int, unsigned int, long, unsigned long, std::string
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding{
        // If this function called, throw an error
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            throw OSException("OSTypeBinding error: returnAssign: invalid type.");
        }
        // If this function called, throw an error
        inline void paramBinding(sqlite3_stmt* statement_, Args&...) {
            throw OSException("OSTypeBinding error: paramBinding: invalid type.");
        }
    };
    // int. Warning: might lose data.
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding<NUM, int, Args...>{
        OSTypeBinding<NUM+1, Args...> next;
    
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = sqlite3_column_int(statement_, NUM);
            next.returnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        inline void paramBinding(sqlite3_stmt* statement_, int& iValue_, Args&... args_) {
            int _result = sqlite3_bind_int(statement_, NUM+1, iValue_);
            if (_result != SQLITE_OK) {
                throw OSException("paramBinding error. Bind int failed.", _result);
            }
            next.paramBinding(statement_, args_...);
        }
    };
    // unsigned int. Warning: might lose data.
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding<NUM, unsigned int, Args...>{
        OSTypeBinding<NUM+1, Args...> next;
        
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (unsigned int)sqlite3_column_int(statement_, NUM);
            next.returnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        inline void paramBinding(sqlite3_stmt* statement_, unsigned int& iValue_, Args&... args_) {
            int _result = sqlite3_bind_int(statement_, NUM+1, iValue_);
            if (_result != SQLITE_OK) {
                throw OSException("paramBinding error. Bind unsigned int failed.", _result);
            }
            next.paramBinding(statement_, args_...);
        }
    };
    // long
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding<NUM, long, Args...>{
        OSTypeBinding<NUM+1, Args...> next;
        
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (long)sqlite3_column_int64(statement_, NUM);
            next.returnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        inline void paramBinding(sqlite3_stmt* statement_, long& lValue_, Args&... args_) {
            int _result = sqlite3_bind_int64(statement_, NUM+1, lValue_);
            if (_result != SQLITE_OK) {
                throw OSException("paramBinding error. Bind long failed.", _result);
            }
            next.paramBinding(statement_, args_...);
        }
    };
    // unsigned long
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding<NUM, unsigned long, Args...>{
        OSTypeBinding<NUM+1, Args...> next;
        
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (unsigned long)sqlite3_column_int(statement_, NUM);
            next.returnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        inline void paramBinding(sqlite3_stmt* statement_, unsigned long& lValue_, Args&... args_) {
            int _result = sqlite3_bind_int64(statement_, NUM+1, lValue_);
            if (_result != SQLITE_OK) {
                throw OSException("paramBinding error. Bind unsigned long failed.", _result);
            }
            next.paramBinding(statement_, args_...);
        }
    };
    // float. Warning: might lose data.
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding<NUM, float, Args...>{
        OSTypeBinding<NUM+1, Args...> next;
        
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (float)(sqlite3_column_double(statement_, NUM));
            next.returnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        inline void paramBinding(sqlite3_stmt* statement_, float& fValue_, Args&... args_) {
            int _result = sqlite3_bind_double(statement_, NUM+1, fValue_);
            if (_result != SQLITE_OK) {
                throw OSException("paramBinding error. Bind float failed.", _result);
            }
            next.paramBinding(statement_, args_...);
        }
    };
    // double
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding<NUM, double, Args...>{
        OSTypeBinding<NUM+1, Args...> next;
    
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::get<NUM>(tuple_) = (double)sqlite3_column_double(statement_, NUM);
            next.returnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        inline void paramBinding(sqlite3_stmt* statement_, double& dValue_, Args&... args_) {
            int _result = sqlite3_bind_double(statement_, NUM+1, dValue_);
            if (_result != SQLITE_OK) {
                throw OSException("paramBinding error. Bind double failed.", _result);
            }
            next.paramBinding(statement_, args_...);
        }
    };
    // std::string
    template <unsigned char NUM, typename... Args>
    struct OSTypeBinding<NUM, std::string, Args...>{
        OSTypeBinding<NUM+1, Args...> next;
    
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            std::string sValue((const char*)sqlite3_column_text(statement_, NUM));
            std::get<NUM>(tuple_) = sValue;
            next.returnAssign(tuple_, statement_);
        }
        
        template <typename... Params>
        inline void paramBinding(sqlite3_stmt* statement_, std::string& sValue_, Args&... args_) {
            int _result = sqlite3_bind_text(statement_, NUM+1, sValue_.c_str(), (int)sValue_.length(), SQLITE_TRANSIENT);
            if (_result != SQLITE_OK) {
                throw OSException("paramBinding error. Bind std::string failed.", _result);
            }
            next.paramBinding(statement_, args_...);
        }
    };

    template <unsigned char NUM>
    struct OSTypeBinding<NUM> {
        template <typename... Returns>
        inline void returnAssign(std::tuple<Returns...>& tuple_, sqlite3_stmt* statement_) {
            // End of recursive type binding. Do nothing.
        }
        
        inline void paramBinding(sqlite3_stmt* statement_) {
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
    OSTablePolicy<_DerivedCLS_>::OSTablePolicy()
    {}
    
    template <class _DerivedCLS_>
    OSTablePolicy<_DerivedCLS_>::~OSTablePolicy()
    {
        if (_primaryKeyHolder != nullptr) {
            delete _primaryKeyHolder;
            _primaryKeyHolder = nullptr;
        }
        
        auto _mapend = _keyBindingMap.end();
        for (auto _it = _keyBindingMap.begin(); _it != _mapend; ++_it) {
            auto _holderptr = _it->second;
            if (_holderptr != nullptr) {
                delete _holderptr;
                _it->second = nullptr;
            }
        }
        _keyBindingMap.clear();
    }
    
    template <class _DerivedCLS_>
    bool OSTablePolicy<_DerivedCLS_>::checkBinding()
    {
        return (_tableName!="") && (_primaryKeyName!="") && (_primaryKeyHolder!=nullptr) && (_keyBindingMap.size());
    }
    
    template <class _DerivedCLS_>
    inline void OSTablePolicy<_DerivedCLS_>::bind_table_name(const std::string& tableName_)
    {
        _tableName = tableName_;
    }
    
    template <class _DerivedCLS_>
    template <typename Tp>
    inline typename std::enable_if<!std::is_pointer<Tp>::value, void>::type OSTablePolicy<_DerivedCLS_>::bind_primary_key(const std::string& keyName_, Tp& memVar_)
    {
        _primaryKeyName = keyName_;
        if (_primaryKeyHolder != nullptr) {
            delete _primaryKeyHolder;
            _primaryKeyHolder = nullptr;
        }
        _primaryKeyHolder = new OSKeyHolder<Tp>(memVar_);
    }
    
    template <class _DerivedCLS_>
    template <typename Tp>
    inline typename std::enable_if<!std::is_pointer<Tp>::value, void>::type OSTablePolicy<_DerivedCLS_>::bind_key(const std::string& keyName_, Tp& memVar_)
    {
        auto _resultPair = _keyBindingMap.emplace(keyName_, new OSKeyHolder<Tp>(memVar_));
        // Replace the old to new
        if (!_resultPair.second) {
            auto _resultptr = _keyBindingMap[keyName_];
            if (_resultptr != nullptr) {
                delete _resultptr;
            }
            _keyBindingMap[keyName_] = new OSKeyHolder<Tp>(memVar_);
        }
    }
    
    template <class _DerivedCLS_>
    inline void OSTablePolicy<_DerivedCLS_>::insertDataToSQL(std::stringstream& stream_, OSPlaceHolder* holder_) throw(OSException)
    {
        // Err, RTTI is needed.
        if (holder_->type() == typeid(int)) {
            stream_ << ((OSKeyHolder<int>*)(holder_))->_content;
        } else if (holder_->type() == typeid(unsigned int)) {
            stream_ << ((OSKeyHolder<unsigned int>*)(holder_))->_content;
        } else if (holder_->type() == typeid(long)) {
            stream_ << ((OSKeyHolder<long>*)(holder_))->_content;
        } else if (holder_->type() == typeid(unsigned long)) {
            stream_ << ((OSKeyHolder<unsigned long>*)(holder_))->_content;
        } else if (holder_->type() == typeid(float)) {
            stream_ << ((OSKeyHolder<float>*)(holder_))->_content;
        } else if (holder_->type() == typeid(double)) {
            stream_ << ((OSKeyHolder<double>*)(holder_))->_content;
        } else if (holder_->type() == typeid(std::string)) {
            stream_ << ((OSKeyHolder<std::string>*)(holder_))->_content;
        } else {
            // No matches. throw OSException.
            throw OSException("insertDataToSQL error. Invalid type.");
        }
    }
         
    template <class _DerivedCLS_>
    inline void OSTablePolicy<_DerivedCLS_>::extractDataFromDB(sqlite3_stmt* statement_, OSPlaceHolder* holder_, int index_) throw(OSException)
    {
        // Err, RTTI is still needed.
        if (holder_->type() == typeid(int)) {
            ((OSKeyHolder<int>*)(holder_))->_content = (unsigned int)sqlite3_column_int(statement_, index_);
        } else if (holder_->type() == typeid(unsigned int)) {
            ((OSKeyHolder<unsigned int>*)(holder_))->_content = (int)sqlite3_column_int(statement_, index_);
        } else if (holder_->type() == typeid(long)) {
            ((OSKeyHolder<long>*)(holder_))->_content = (long)sqlite3_column_int64(statement_, index_);
        } else if (holder_->type() == typeid(unsigned long)) {
            ((OSKeyHolder<unsigned long>*)(holder_))->_content = (unsigned long)sqlite3_column_int64(statement_, index_);
        } else if (holder_->type() == typeid(float)) {
            ((OSKeyHolder<float>*)(holder_))->_content = (float)sqlite3_column_double(statement_, index_);
        } else if (holder_->type() == typeid(double)) {
            ((OSKeyHolder<double>*)(holder_))->_content = (double)sqlite3_column_double(statement_, index_);
        } else if (holder_->type() == typeid(std::string)) {
            std::string sValue((const char*)sqlite3_column_text(statement_, index_));
            ((OSKeyHolder<std::string>*)(holder_))->_content= sValue;
        } else {
            // No matches. throw OSException.
            throw OSException("extractDataFromDB error. Invalid type.");
        }
    }



    // Functions for OSQuery
    OSQuery::OSQuery(const OSDatabase& database_) throw(OSException) : _connection(database_._connection)
    {
        if (_connection == nullptr) {
            throw OSException("OSStatement ctor error: SQLite connection is not opened.");
        }
    }

    OSQuery::~OSQuery()
    {}
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type OSQuery::save(Table& table_) throw(OSException)
    {
        // Check the acceptance of table binding
        if (!table_.checkBinding()) {
            throw OSException("save error: table binding is not acceptable.");
        }
        
        // Construct the SQL string and fill in data directly
        std::stringstream _sqlStream;
        _sqlStream << "insert into " + table_._tableName + "(";
        std::stringstream _endStream;
        _endStream << ") values('";
        
        // Add keys and params
        _sqlStream << table_._primaryKeyName << ", ";
        table_.insertDataToSQL(_endStream, table_._primaryKeyHolder);
        _endStream << "', ";
        
        auto _bindMapEnd = table_._keyBindingMap.end();
        for (auto _it = table_._keyBindingMap.begin(); _it != _bindMapEnd; ++_it) {
            _sqlStream << _it->first << ", ";
            _endStream << "'";
            table_.insertDataToSQL(_endStream, _it->second);
            _endStream << "', ";
        }
        std::string _sqlStr = _sqlStream.str();
        _sqlStr[_sqlStr.size()-2] = ' ';
        std::string _sqlEnd = _endStream.str();
        _sqlEnd[_sqlEnd.size()-2] = ' ';
        _sqlEnd += ")";
        _sqlStr += _sqlEnd;
        
        // Execute
        int _result = sqlite3_exec(_connection, _sqlStr.c_str(), nullptr, nullptr, nullptr);
        if (_result != SQLITE_OK) {
            throw OSException("save error. sqlite3_exec execution failed.", _result);
        }
    }
            
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, bool>::type OSQuery::exists(Table& table_) throw(OSException)
    {
        // Check the acceptance of table binding
        if (!table_.checkBinding()) {
            throw OSException("exists error: table binding is not acceptable.");
        }
        
        // Process select count operations to check if the data exists
        // Construct the SQL string and fill in data directly
        std::stringstream _sqlStream;
        _sqlStream << "select count(*) from " + table_._tableName + " where ";
        
        // Add keys and params
        _sqlStream << table_._primaryKeyName << "='";
        table_.insertDataToSQL(_sqlStream, table_._primaryKeyHolder);
        _sqlStream << "'";
        std::string _sqlStr = _sqlStream.str();
        
        // Execute
        int _result = sqlite3_exec(_connection, _sqlStr.c_str(), [](void*,int,char** iptr2,char**){return ((**iptr2)=='0')-1;}, nullptr, nullptr);
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
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type OSQuery::fill(Table& table_) throw(OSException)
    {
        // statement
        sqlite3_stmt* _statement = nullptr;
        try {
            // Check the acceptance of table binding
            if (!table_.checkBinding()) {
                throw OSException("exists error: table binding is not acceptable.");
            }
            
            // Construct SQL string
            std::stringstream _sqlStream;
            _sqlStream << "select " ;
            std::stringstream _endStream;
            for (auto& _it : table_._keyBindingMap) {
                _sqlStream << _it.first << ", ";
            }
            _endStream << "from " << table_._tableName << " where " << table_._primaryKeyName << "='";
            table_.insertDataToSQL(_endStream, table_._primaryKeyHolder);
            _endStream << "'";
            std::string _sqlString = _sqlStream.str();
            _sqlString[_sqlString.size()-2] = ' ';
            _sqlString += _endStream.str();
            
            // Prepare statement
            int _result = sqlite3_prepare_v2(_connection, _sqlString.c_str(), (int)_sqlString.length(), &_statement, nullptr);
            if (_result != SQLITE_OK) {
                throw OSException("fill error: Cannot prepare the sqlite3_stmt.", _result);
            }
            
            // Get value.
            int _colCount = sqlite3_column_count(_statement);
            assert(_colCount == table_._keyBindingMap.size());
            int _loopCount = 0;
            _result = sqlite3_step(_statement);
            if (_result == SQLITE_DONE) {
                throw OSException("fill error: no records found", _result);
            }
            if (_result != SQLITE_ROW) {
                throw OSException("fill error: step error", _result);
            }
            for (auto& _it : table_._keyBindingMap) {
                table_.extractDataFromDB(_statement, _it.second, _loopCount);
                ++_loopCount;
            }
            
        } catch (const OSException&) {
            sqlite3_finalize(_statement);
            _statement = nullptr;
            throw;
        }
        
        sqlite3_finalize(_statement);
        _statement = nullptr;
    }
    
    template <typename Table>
    typename std::enable_if<std::is_base_of<OSTablePolicy<Table>, Table>::value, void>::type OSQuery::update(Table& table_) throw(OSException)
    {
        // Check the acceptance of table binding
        if (!table_.checkBinding()) {
            throw OSException("update error: table binding is not acceptable.");
        }
        
        // Construct the SQL string and fill in data directly
        std::stringstream _sqlStream;
        _sqlStream << "update " + table_._tableName + " set ";
        std::stringstream _endStream;
        _endStream << "where ";
        
        // Add keys and params
        _endStream << table_._primaryKeyName << "='";
        table_.insertDataToSQL(_endStream, table_._primaryKeyHolder);
        _endStream << "'";
        
        auto _bindMapEnd = table_._keyBindingMap.end();
        for (auto _it = table_._keyBindingMap.begin(); _it != _bindMapEnd; ++_it) {
            _sqlStream << _it->first << "='";
            table_.insertDataToSQL(_sqlStream, _it->second);
            _sqlStream << "', ";
        }
        std::string _sqlStr = _sqlStream.str();
        _sqlStr[_sqlStr.size()-2] = ' ';
        std::string _sqlEnd = _endStream.str();
        _sqlStr += _sqlEnd;
        
        // Execute
        int _result = sqlite3_exec(_connection, _sqlStr.c_str(), nullptr, nullptr, nullptr);
        if (_result != SQLITE_OK) {
            throw OSException("update error. sqlite3_exec execution failed.", _result);
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
        if (!table_.checkBinding()) {
            throw OSException("update error: table binding is not acceptable.");
        }
        
        // Construct the SQL string and fill in data directly
        std::stringstream _sqlStream;
        _sqlStream << "delete from " + table_._tableName + " where ";
        
        // Add keys and params
        _sqlStream << table_._primaryKeyName << "='";
        table_.insertDataToSQL(_sqlStream, table_._primaryKeyHolder);
        _sqlStream << "'";
        
        std::string _sqlStr = _sqlStream.str();
        
        // Execute
        int _result = sqlite3_exec(_connection, _sqlStr.c_str(), nullptr, nullptr, nullptr);
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
        OSTypeBinding<0, Args...> _typeBinding;
        _typeBinding.paramBinding(_statement, args_...);
        
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
        OSTypeBinding<0, Args...> _typeBinding;
        _typeBinding.paramBinding(_statement, args_...);
        
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
            
            OSTypeBinding<0, Returns...> _typeReturns;
            _typeReturns.returnAssign(_tuple, _statement);
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
        OSTypeBinding<0, Args...> _typeBinding;
        _typeBinding.paramBinding(_statement, args_...);
        
        // Execute
        _result = sqlite3_step(_statement);
        if (_result != SQLITE_ROW) {
            throw OSException("executeScalar error. Execute SQLString failed.", _result);
        }
        std::tuple<R> _tuple;
        OSTypeBinding<0, R> _typeReturns;
        _typeReturns.returnAssign(_tuple, _statement);
        
        sqlite3_finalize(_statement);
        _statement = nullptr;
        return std::get<0>(_tuple);
        
    } catch (const OSException&) {
        sqlite3_finalize(_statement);
        _statement = nullptr;
        throw;
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