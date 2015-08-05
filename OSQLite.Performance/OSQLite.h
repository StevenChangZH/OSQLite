//
//  OSQLite.h
//  OSQLite.Performance
//
//  The OSQLite encapsulation follows performance goal.
//  THE performance here means memory usage, compile speed and execution speed.
//
//  Created by Steven Chang on 15/8/5.
//  Copyright (c) 2015 Steven Chang. All rights reserved.
//
//  Interface-design stage. NOT accomplished.

#pragma once

/*
 rules... 
 no try/catch, no exceptions. -> Make use of SQLite result codes. - Or allow modification?
 limit use of templates.(->fast compiling)
 modified new/delete macros/overloading functions...
 string type... can be modified (reduce memory usage)
 blob type realizations
 DB open/close: RAII? Maybe not necessary.
 use of boost library?En...
 */