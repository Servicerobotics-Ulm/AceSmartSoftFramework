// --------------------------------------------------------------------------
//
//  Copyright (C) 2002/2004/2008 Christian Schlegel
//                2009/2013 Alex Lotz
//
//        lotz@hs-ulm.de
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//
//  This file is part of ACE/SmartSoft.
//
//  ACE/SmartSoft is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ACE/SmartSoft is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with ACE/SmartSoft.  If not, see <http://www.gnu.org/licenses/>.
//
// --------------------------------------------------------------------------

#ifndef _SMARTINIPARAMETER_HH
#define _SMARTINIPARAMETER_HH

#include <map>
#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#ifdef WIN32
   #pragma warning( disable : 4290 )   // disable __declspec(nothrow) warning (cased by throw(SmartACE::ParameterError) explicit declaration)
#endif

namespace SmartACE {
  /** Exception class to describe errors while processing parameters in SmartParameter.
   */
  class IniParameterError : public std::domain_error
  {
  public:
    IniParameterError(const std::string & reason)
      : domain_error(reason)
      { };
  };


  /** Ini-parameter management.
   *
   *  Supports storage of IniParameters in separate ini-parameter files.
   *  Ini-parameter files can be read by this class which then provides
   *  simple access to all kinds of ini-parameters. Ini-parameters can also
   *  be extracted from environment variables or be defined by command
   *  line arguments.
   *
   *  The prefix is used to group ini-parameters and can also be set to
   *  the empty string. An ini-parameter therefore is always referenced
   *  by its prefix and its name.
   *
   *  Prefix and ini-parameter name are case insensitive !
   *
   *  Demonstrated in seventh example
   */
  class SmartIniParameter {

  private:
    //
    // associative map to maintain parameters
    //
    std::map<std::pair<std::string,std::string>, std::list<std::string> > parameter;

    //
    // helper function to read until next unescaped newline
    //
    void getline(std::istream &is, std::string &line) const;

    //
    // Iterate the argv vector and try to get the value of a parameter with the name @argument_name.
    // The parameter must be formated as follows:
    // '-'('-')?ID('='|' ')STRING  (e.g. --filename=/some/path or -filename /some/path or a combination of both)
    //
    std::string getArgsParamValue(const int &argc, char * const * argv, const std::string &long_opt, const char &short_opt=-1);

  public:
    // Initialization

    /// Constructor
    SmartIniParameter();

    /// Destructor
    virtual ~SmartIniParameter();


    //
    // Adding Ini-parameters
    //

    /** Appends an entry to the ini-parameter list.
     *
     *  If same entry already exists overwrite it with the new value.
     */
    void addEntry(std::string prefix, std::string name, const std::string &value);

    /** Appends command line arguments to the ini-parameter list.
     *
     *  If same entry already exists overwrite it with the new value.
     *  Format of command line ini-parameter: -parameter=value
     */
    void addCommandLineArgs(const int &argc, char * const * argv, const std::string &default_prefix="component", const int &skip_args=1);

    /** Appends environment variable to the ini-parameter list.
     *
     *  @param prefix prefix of ini-parameter name (prefix.name)
     *  @param env is the name of the environment variable.
     *
     *  If same entry already exists overwrite it with the new value.
     *  Throw exception if environment variable unknown.
     */
    void addEnv(const std::string &prefix, const std::string &env);

    /** Appends the contents of a ini-parameter file to the ini-parameter list.
     *  Entries which already exist are replaced by the new values.
     *  Throw exception if something went wrong (file does not exist
     *  or contains syntax errors).
     *
     *  Looks first in the current directory and then in $SMART_ROOT/etc
     *
     *  The file extension normally used for ini-parameter files is "ini".
     *  There is no maximum length per line, newlines may be escaped to
     *  concatenate successive lines. The syntax is as follows:
     *
     *  @code
     *  --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
     *     [smartsoft]
     *
     *     # -------------------------------------
     *     # laser server parameter
     *     # -------------------------------------
     *
     *     # laser device
     *     [laser]
     *
     *     type         SICK LMS200
     *     use          Yes
     *
     *     baudrate     57600          # 1200/9600/19200/38400/57600
     *     parity       NONE
     *     data         8
     *     device       /dev/cua2      # /dev/cur2  /dev/cua2
     *
     *     interval     0.167          # poll interval in seconds
     *
     *  --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
     *  @endcode
     *
     *  ALL ENTRIES ARE CASE INSENSITIVE !
     *
     *  The first line always contains the identification which is "[smartsoft]".
     *  For compatibility reasons we also accept "[bParamFile]" and "[smartParamFile]".
     *
     *  The first string in the line is the parameter name. Everything else following
     *  is the value excluding any comment. Comments are invoked by a "#" and end with
     *  the line.
     *
     *  Square brackets invoke the next group. If ini-parameters are defined before any
     *  group is named, then their group name is simply "" (empty group name).
     *
     *  Truth values can be defined in many different formats:
     *    - true  :   yes | y | true  | t | + | 1 | on
     *    - false :   no  | n | false | f | - | 0 | off
     */
    void addFile(const std::string &filename);

    /** This method searches for the file using different search strategies and if found returns the filestream.
     *
     *	@param filename The file name to search for
     *	@param filestream The filestream pointing to the content of the found file
     *	@returns true if the filename was found
     *	@returns false if the filename was not found (no exception is used here and filestream.open() is false in this case)
     */
    bool searchFile(const std::string &filename, std::ifstream &filestream);

    /** This is a wrapper for addFile(argc,argv,argname,false) without throwing an exception if no argument given.
     *
     *  This method checks whether the argv vector contains the long_opt and/or short_opt parameter.
     *  If not found a 'false' is returned without throwing an exception.
     *  If found the method addFile(argc,argv,argument_name,false) is called and if this succeeds 'true' is returned.
     */
    bool tryAddFileFromArgs(const int &argc, char * const * argv, const std::string &long_opt = "filename", const char &short_opt = -1);

    /** Append the contents of an ini-parameter file to the ini-parameter list.
     *
     *  The name of the ini-parameter file is not directly given, but extracted from 
     *  the argument list. So you can start multiple instances of the same
     *  program each with a different ini-parameter set.
     *
     *  The command line arguments are given by \a argc and \a argv and the
     *  string \a argname gives the name of the argument which contains
     *  the actual filename.
     *
     *  If \a allow_stdin is \c true (default) and the value of argument \a argname 
     *  is "stdin", the method will read the configuration data from stdin. 
     *
     *  @author Boris Kluge <kluge@faw.uni-ulm.de>
     */
    void addFile(int argc, char * const * argv, const std::string &argname, bool allow_stdin = true);

    /** Append the contents of an ini-parameter file to the ini-parameter list.
     *  The stream is assumed to be connected to a ini-parameter file.
     */
    void addFile(std::istream &is);

    //
    // helper methods
    //

    /// print all currently known parameters with their values
    void print(void);

    //
    // Retrieving parameters out of ini-parameter list
    //

    /** Search for an optional parameter in ini-file.
     *  If the parameter could be found, then "true", otherwise "false", is returned.
     *  If parameter is not found, no exception is thrown, which makes it easier
     *  to check for existence of an optional parameter in the ini-file.
     */
    bool checkIfParameterExists(const std::string &prefix, const std::string &parameter);

    /** Get all parameter pairs (key,value) for a given parameter group.
     *
     *  Iterates through all parameters and returns all (key,value) pairs that match the prefix (parameter group).
     */
    std::map<std::string,std::string> getAllParametersFromGroup(const std::string &prefix);

    std::map<std::string, std::list<std::string> > getAllParameterListsFromGroup(const std::string &prefix);

    /** Retrieve double casted value of ini-parameter name
     *
     * throw "IniParameterError" if ini-parameter not known or conversion fails
     */
    double getDouble(std::string prefix, std::string parameter) const;

    std::list<double> getDoubleList(std::string prefix, std::string parameter) const;

    /** Retrieve double casted value of ini-parameter name
     *
     * leave \a d alone and return false,
     * if ini-parameter not known or conversion fails
     */
    template<class Float>
    inline bool getDouble(const std::string &prefix, const std::string &parameter, Float &d) const;

    template<class Float>
    inline bool getDoubleList(const std::string &prefix, const std::string &parameter, std::list<Float> &d) const;

    /** Retrieve int casted value of ini-parameter name
     *
     *  throw "IniParameterError" if ini-parameter not known or conversion fails
     */
    int getInteger(std::string prefix, std::string parameter) const;

    std::list<int> getIntegerList(std::string prefix, std::string parameter) const;

    /** Retrieve int casted value of ini-parameter name
     *
     *  leave \a i alone and return false,
     *  if ini-parameter not known or conversion fails
     */
    template<class Integer>
    inline bool getInteger(const std::string &prefix, const std::string &parameter, Integer &i) const;

    template<class Integer>
    inline bool getIntegerList(const std::string &prefix, const std::string &parameter, std::list<Integer> &i) const;

    /** Retrieve truth value of ini-parameter name.
     *
     * throw "IniParameterError" if ini-parameter not known or conversion fails
     */
    bool getBoolean(std::string prefix, std::string parameter) const;

    std::list<bool> getBooleanList(std::string prefix, std::string parameter) const;

    /** Retrieve truth value of ini-parameter name.
     *
     * leave \a b alone and return false,
     * if ini-parameter not known or conversion fails
     */
    bool getBoolean(const std::string &prefix, const std::string &parameter, bool &b) const;

    bool getBooleanList(const std::string &prefix, const std::string &parameter, std::list<bool> &b) const;

    /** Retrieve string value of ini-parameter name
     *
     *  throw "IniParameterError" if parameter not known
     */
    std::string getString(std::string prefix, std::string parameter) const;
    
    std::list<std::string> getStringList(std::string prefix, std::string parameter) const;

    /** Retrieve string value of ini-parameter name
     *
     *  leave \a s alone and return false if ini-parameter not known
     */
    bool getString(const std::string &prefix, const std::string &parameter, std::string &s) const;
    
    bool getStringList(const std::string &prefix, const std::string &parameter, std::list<std::string> &s) const;

    /** @internal return the begin Iterator from our map.
     *
     *  subject to change without notice.
     */
    std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator
    begin();

    /** @internal return the end Iterator from our map.
     *
     *  subject to change without notice.
     */
    std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator
    end();

  };

  template<class Float>
  inline bool SmartACE::SmartIniParameter::getDouble(const std::string &prefix, const std::string &param, Float &d) const 
  {
    try {
      const Float tmp = getDouble(prefix, param);
      d = tmp;
      return true;
    } catch(SmartACE::IniParameterError &e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }

  template<class Float>
  inline bool SmartACE::SmartIniParameter::getDoubleList(const std::string &prefix, const std::string &param, std::list<Float> &d) const 
  {
    try {
      std::list<double> tmp = getDoubleList(prefix, param);
      std::list<double>::const_iterator it;
      d.clear();
      for(it=tmp.begin(); it!=tmp.end(); it++) {
        d.push_back(*it);
      }
      return true;
    } catch(SmartACE::IniParameterError &e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }

  template<class Integer>
  inline bool SmartACE::SmartIniParameter::getInteger(const std::string &prefix, const std::string &param, Integer &i) const 
  {
    try {
      const Integer tmp = getInteger(prefix, param);
      i = tmp;
      return true;
    } catch(SmartACE::IniParameterError &e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }

  template<class Integer>
  inline bool SmartACE::SmartIniParameter::getIntegerList(const std::string &prefix, const std::string &param, std::list<Integer> &i) const 
  {
    try {
      std::list<int> tmp = getIntegerList(prefix, param);
      std::list<int>::const_iterator it;
      i.clear();
      for(it=tmp.begin(); it!=tmp.end(); it++) {
        i.push_back(*it);
      }
      return true;
    } catch(SmartACE::IniParameterError &e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }
}


#endif // _SMARTINIPARAMETER_HH

