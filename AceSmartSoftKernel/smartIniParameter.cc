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

#include <sstream>

#include <locale>

#include "smartIniParameter.hh"

// use SMARTSOFT_PRINT from here
#include "smartOSMapping.hh"


SmartACE::SmartIniParameter::SmartIniParameter()
{
  parameter.clear();

  // this sets the floating point representation to allways use US default "."
  std::locale::global(std::locale::classic());
}

SmartACE::SmartIniParameter::~SmartIniParameter()
{
}

std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator
SmartACE::SmartIniParameter::begin()
{
  return parameter.begin();
}

std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator
SmartACE::SmartIniParameter::end()
{
  return parameter.end();
}


void SmartACE::SmartIniParameter::addEntry(std::string prefix, std::string param, const std::string &value)
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  parameter[p].push_back(value);
}

void SmartACE::SmartIniParameter::addCommandLineArgs(const int &argc, char * const * argv, const std::string &default_prefix, const int &skip_args)
{
	bool is_key = false;
	std::string param = "";
	std::string value = "";
	std::string prefix = "";

	for(int i=skip_args; i<argc; ++i) {
		std::string arg = argv[i];
		if(arg.size()>0 && arg.at(0) == '-') {
			// this seems to be a console parameter
			size_t key_pos = arg.find_first_not_of('-');
			if(key_pos == std::string::npos) continue; // this is an ill formed param, get the next one
			else is_key = true;

			// check whether the value is directly assigned to the parameter
			size_t assign_pos = arg.find('=');
			if(assign_pos != std::string::npos && key_pos < assign_pos) {
				if((assign_pos+1) < arg.size()) {
					// param also has a value directly assigned
					// e.g. --filename=/some/path
					param = arg.substr(key_pos, (assign_pos - key_pos) );
					value = arg.substr(assign_pos+1);

					size_t prefix_pos = param.find('.');
					if(prefix_pos != std::string::npos) {
						// A '.' was found in the parameter name. It is assumed that the
						// preceding part before the point is the prefix and the following part
						// after the '.' is the parameter name, e.g. "--component.name".
						prefix = param.substr(0, prefix_pos);
						param = param.substr(prefix_pos+1);
					} else {
						// parameter name is not specified using a point notation for groups
						// e.g. it is simply "--name" so use the default prefix
						prefix = default_prefix;
					}

					this->addEntry(prefix,param,value);
					is_key = false; // reset the key part to look for the next key
				} else {
					// the value seems to be empty, just save the key and ignore the value
					param = arg.substr(key_pos, (assign_pos - key_pos));
				}
			} else {
				// there is a key but no value, save the param name without the preceding '-'
				// e.g. --filename /some/path
				param = arg.substr(key_pos);

				size_t prefix_pos = param.find('.');
				if(prefix_pos != std::string::npos) {
					// A '.' was found in the parameter name. It is assumed that the
					// preceding part before the point is the prefix and the following part
					// after the '.' is the parameter name, e.g. "--component.name".
					prefix = param.substr(0, prefix_pos);
					param = param.substr(prefix_pos+1);
				} else {
					// parameter name is not specified using a point notation for groups
					// e.g. it is simply "--name" so use the default prefix
					prefix = default_prefix;
				}
			}
		} else if(is_key) {
			// the current arg does not start with a '-' and we had a key already found
			// thus the current arg must be the corresponding value
			value = arg;
			this->addEntry(prefix,param,value);
			is_key = false; // reset the key part to look for the next key
			// if a param have several values, just the first one is used and the others are ignored
			// e.g. --filename /some/path /some/other/path
		}
	}
}

void SmartACE::SmartIniParameter::addEnv(const std::string &prefix, const std::string &env) 
{
  const char *value;

  //value = getenv(env.c_str());
  value = ACE_OS::getenv(env.c_str());

  if (value) {
    std::string v = value;

    this->addEntry(prefix,env,v);
  } else {
    throw SmartACE::IniParameterError("Ini-parameter not found in environment");
  }
}

bool SmartACE::SmartIniParameter::searchFile(const std::string &filename, std::ifstream &filestream)
{
	std::string name1, name2, name_env;
	const char *env;

	//
	// open file
	//
	name1 = "";
	name1.append("./");
	name1.append(filename);

	name2 = "";
	name2.append("./etc/");
	name2.append(filename);

	env = ACE_OS::getenv("SMART_ROOT_ACE");
	if (env) {
		name_env = "";
		name_env.append(env);
		name_env.append("/etc/");
		name_env.append(filename);
	} else {
		name_env = "";
	}

	filestream.open(name1.c_str());
	if (filestream.is_open()) {
	  return true;
	} else {
		filestream.open(name2.c_str());
		if(filestream.is_open()) {
			return true;
		} else {
			filestream.open(name_env.c_str());
			if(filestream.is_open()) {
				return true;
			}
		}
	}

	return false;
}

void SmartACE::SmartIniParameter::addFile(const std::string &filename) 
{
  std::ifstream t;

  //
  // open file
  //
  if(!searchFile(filename, t)) {
        std::ostringstream oss;
        oss << "SmartIniParameter: error: couldn't find parameter file <" << filename << ">" << std::endl;
        std::cerr << oss.str();
        throw IniParameterError(oss.str());
  }

  //
  // read and parse the file
  //
  addFile(t);
}

std::string SmartACE::SmartIniParameter::getArgsParamValue(const int &argc, char * const * argv, const std::string &long_opt, const char &short_opt)
{
	std::string param_value = "";
	bool key_found = false;

	for(int i=0; i<argc; ++i)
	{
		std::string arg = argv[i];
		if(arg.size() > 0 && arg[0] == '-')
		{
			// seems to be a key (due to starting with a '-' character)
			size_t not_minus_pos = arg.find_first_not_of('-');
			size_t assign_pos = arg.find('=');
			if(not_minus_pos == std::string::npos || not_minus_pos > assign_pos) continue; // ill-formed param, get the next one

			// get the key string
			std::string key = "";
			if(assign_pos != std::string::npos) {
				key = arg.substr(not_minus_pos, (assign_pos - not_minus_pos) );
			} else {
				key = arg.substr(not_minus_pos);
			}

			// check if the current key matches either the short-option or the long-option
			if(key.size() == 1) {
				// short option
				if(short_opt != -1 && key[0] == short_opt) {
					// key found!
					key_found = true;
				}
			} else {
				// long option
				if(key == long_opt) {
					// key found!
					key_found = true;
				}
			}

			// now check if the parameter is directly assigned
			if(key_found && assign_pos != std::string::npos && (assign_pos+1) < arg.size()) {
				// we have found both, the key and the directly assigned value
				param_value = arg.substr(assign_pos+1);
				break;
			}
		} else if(key_found) {
			// key was already found in one of the previous loops
			if(arg.size() == 0) {
				// empty value -> continue
				continue;
			} else if(arg.at(0) != '-') {
				// we assume that arg now contains the value for the previously found key
				param_value = arg;
				break;
			} else {
				// this seems to be the next console parameter
				// reset the key_found and search for the next key which might be the right one
				key_found = false;
				continue;
			}
		}
	} // end for each arg in argv

	return param_value;
}

bool SmartACE::SmartIniParameter::tryAddFileFromArgs(const int &argc, char * const * argv, const std::string &long_opt, const char &short_opt)
{
	std::string ini_file = getArgsParamValue(argc,argv,long_opt,short_opt);
	bool found_ini_file = false;

	if(ini_file != "") {
		try {
			std::ifstream ifs;
			ifs.open(ini_file.c_str());
			if(ifs.good())
			{
				addFile(ifs);
				found_ini_file = true;
			}
		} catch (SmartACE::IniParameterError &err) {
			std::cerr << err.what() << std::endl;
		} catch (std::exception &ex) {
			std::cerr << ex.what() << std::endl;
		}
	}

	return found_ini_file;
}

void SmartACE::SmartIniParameter::addFile(int argc, char * const * argv, const std::string &argname, bool allow_stdin) 
{
  std::string filename = getArgsParamValue(argc, argv, argname);

  // parse the file if found
  if(filename != "")
  {
    if(allow_stdin && (filename=="stdin"))
    {
      addFile(std::cin);
    }
    else
    {
      std::ifstream ifs;
      ifs.open(filename.c_str());
      if(ifs.good())
      {
        addFile(ifs);
      }
      else
      {
        std::ostringstream oss;
        oss << "SmartIniParameter: error: couldn't open parameter file <" << filename << ">" << std::endl;
        throw IniParameterError(oss.str());
      }
    }
  }
  else
  {
    std::ostringstream oss;
    oss << "SmartIniParameter: error: couldn't find command line parameter \"" << argname << "\"." << std::endl;
    throw IniParameterError(oss.str());
  }
}

void SmartACE::SmartIniParameter::addFile(std::istream &is) 
{
//  char         content[512];
  std::string  line;
  unsigned int index;
  unsigned int start;

  std::string prefix;
  std::string param;
  std::string value;

  //
  // read first line
  //
  if (is.eof()) {
    std::cerr << "SmartIniParameter: error: empty file" << std::endl;
    throw IniParameterError("SmartIniParameter: error: empty file");
  }

//  is.getline(content,512);
//  line = content;
  this->getline(is,line);

  if ((line != "[smartsoft]") && (line != "[bParamFile]") && (line != "[smartParamFile]")) {
    std::cerr << "SmartIniParameter: error: seems not to be a parameter file" << std::endl;
    throw IniParameterError( "SmartIniParameter: error: seems not to be a parameter file");
  }

  while (!is.eof()) {

//    is.getline(content,512);
//    line = content;
    this->getline(is,line);

    //
    //
    // interpret a single line
    //
    //
    index = 0;

    // skip leading white space
    while (isspace(line[index])) index++;

    // comment or blank line => skip this line
    if ((index >= line.size()) || (line.at(index)==';') || (line.at(index)=='#')) continue;

    //
    // group prefix
    //
    if (line.at(index)=='[') {
      index++;
      start = index;
      while ((index < line.size())  && (line.at(index)!=';')  && (line.at(index)!='#')  &&
             (line.at(index)!=' ')  && (line.at(index)!='\t') && (line.at(index)!='\n') &&
             (line.at(index)!='\r') && (line.at(index)!=']')) index++;

      prefix = line.substr(start,index-start);

      continue;
    }

    //
    // no group prefix, therefore we expect the parameter name
    //
    start = index;

    while ((index < line.size())  && (line.at(index)!=';')  && (line.at(index)!='#')  &&
           (line.at(index)!=' ')  && (line.at(index)!='\t') && (line.at(index)!='\n') &&
           (line.at(index)!='\r')) index++;

    param = line.substr(start,index-start);

    if (!isspace(line[index])) {
      //
      // line terminates without giving a value => value is set to empty string
      //
      std::string empty;

      this->addEntry(prefix,param,empty);

      continue;
    }

    //
    // found parameter name which is followed by a value
    //

    // skip white space in front of value
    while (isspace(line[index])) index++;

    start = index;

    // do not accept blank as delimiter since then one could not have multiple word
    // values
    while ((index < line.size())  && (line.at(index)!=';')  && (line.at(index)!='#') &&
           (line.at(index)!='\n') && (line.at(index)!='\r')) index++;

    // now remove white space at the end since we read until we reached a hard delimiter
    index--;
    while (isspace(line[index])) index--;

    value = line.substr(start,index-start+1);

    this->addEntry(prefix,param,value);
  }
}

void SmartACE::SmartIniParameter::print(void)
{
	std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator i;

  for (i=parameter.begin();i!=parameter.end();++i) {
	std::list<std::string>::const_iterator val;
	for(val=i->second.begin(); val!=i->second.end(); ++val) {
		//std::cout << "<" << (*i).first.first << "><" << (*i).first.second << "><" << *val << ">" << std::endl;
    		SMARTSOFT_PRINT(ACE_TEXT("<%s><%s><%s>\n"), i->first.first.c_str(), i->first.second.c_str(), val->c_str());
	}
  }
}

bool SmartACE::SmartIniParameter::checkIfParameterExists(const std::string &prefix, const std::string &param)
{
  std::string lower_param(param);
  std::string lower_prefix(prefix);
  transform (lower_param.begin(),  lower_param.end(),  lower_param.begin(),  ::tolower);
  transform (lower_prefix.begin(), lower_prefix.end(), lower_prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(lower_prefix,lower_param);

  if (parameter.find(p) != parameter.end()) {
    return true;
  }

  return false;
}

std::map<std::string,std::string> SmartACE::SmartIniParameter::getAllParametersFromGroup(const std::string &prefix)
{
	std::map<std::string,std::string> result;

	std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator it;
	for(it=parameter.begin(); it!=parameter.end(); it++) {
		if(it->first.first == prefix) {
			result[it->first.second] = it->second.back();
		}
	}

	return result;
}

std::map<std::string, std::list<std::string> > SmartACE::SmartIniParameter::getAllParameterListsFromGroup(const std::string &prefix)
{
	std::map<std::string, std::list<std::string> > result;

	std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator it;
	for(it=parameter.begin(); it!=parameter.end(); it++) {
		if(it->first.first == prefix) {
			result[it->first.second] = it->second;
		}
	}

	return result;
}

double SmartACE::SmartIniParameter::getDouble(std::string prefix, std::string param) const 
{
  // return the last element from a potential value list
  return getDoubleList(prefix, param).back();
}

std::list<double> SmartACE::SmartIniParameter::getDoubleList(std::string prefix, std::string param) const 
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Ini-parameter not found: double [" << prefix  << "] " << param;
    throw IniParameterError(oss.str());
  }

  std::list<double> result;
  std::list<std::string>::const_iterator it;
  for(it=i->second.begin(); it!=i->second.end(); ++it) {
	  const char *value = it->c_str();
	  char *end;
	  const double d = strtod(value, &end);
	  if (end == value) {
	    std::ostringstream oss;
	    oss << "Conversion failed: double [" << prefix  << "] " << param;
	    throw IniParameterError(oss.str());
	  }

	  result.push_back(d);
  }

  return result;
}

int SmartACE::SmartIniParameter::getInteger(std::string prefix, std::string param) const 
{
	return getIntegerList(prefix, param).back();
}

std::list<int> SmartACE::SmartIniParameter::getIntegerList(std::string prefix, std::string param) const 
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Ini-parameter not found: integer [" << prefix  << "] " << param;
    throw IniParameterError(oss.str());
  }

  std::list<int> result;
  std::list<std::string>::const_iterator it;
  for(it=i->second.begin(); it!=i->second.end(); ++it) {
	  const char *value = (*i).second.back().c_str();
	  char *end;
	  const long int l = strtol(value, &end, 0); // accept hex ("0x...") and oct ("0...") representation

	  if (end == value) {
		std::ostringstream oss;
		oss << "Conversion failed: integer [" << prefix  << "] " << param;
		throw IniParameterError(oss.str());
	  }

	  result.push_back(l);
  }

  return result;
}

bool SmartACE::SmartIniParameter::getBoolean(std::string prefix, std::string param) const 
{
	return getBooleanList(prefix, param).back();
}

std::list<bool> SmartACE::SmartIniParameter::getBooleanList(std::string prefix, std::string param) const 
{

  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Ini-parameter not found: boolean value [" << prefix  << "] " << param;
    throw IniParameterError(oss.str());
  }

  std::list<bool> result_list;
  std::list<std::string>::const_iterator it;
  for(it=i->second.begin(); it!=i->second.end(); ++it) {
	std::string result;

	result = (*i).second.back();
	transform (result.begin(), result.end(), result.begin(), ::tolower);

	if (result=="yes" || result=="true" || result=="y" || result=="t" || result=="on" || result=="+") {
		result_list.push_back(true);
	} else if (result=="no" || result=="false" || result=="n" || result=="f" || result=="off" || result=="-") {
		result_list.push_back(false);
	} else if (atoi(result.c_str())) {
		result_list.push_back(true);
	} else {
		result_list.push_back(false);
	}
  }
  return result_list;
}

bool SmartACE::SmartIniParameter::getBoolean(const std::string &prefix, const std::string &param, bool &b) const 
{
  try {
    const bool tmp = getBoolean(prefix, param);
    b = tmp;
    return true;
  } catch(SmartACE::IniParameterError &e) {
    std::cerr << e.what() << std::endl;
  }
   return false;
}

bool SmartACE::SmartIniParameter::getBooleanList(const std::string &prefix, const std::string &param, std::list<bool> &b) const 
{
  try {
    b = getBooleanList(prefix, param);
    return true;
  } catch(SmartACE::IniParameterError &e) {
    std::cerr << e.what() << std::endl;
  }
   return false;
}

std::string SmartACE::SmartIniParameter::getString(std::string prefix, std::string param) const 
{
	return getStringList(prefix, param).back();
}

std::list<std::string> SmartACE::SmartIniParameter::getStringList(std::string prefix, std::string param) const 
{
  transform (param.begin(),  param.end(),  param.begin(),  ::tolower);
  transform (prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  std::pair<std::string,std::string> p(prefix,param);

  std::map<std::pair<std::string,std::string>, std::list<std::string> >::const_iterator i;
  i = parameter.find(p);

  if (i == parameter.end()) {
    std::ostringstream oss;
    oss << "Ini-parameter not found: string [" << prefix  << "] " << param;
    throw IniParameterError(oss.str());
  }

  std::list<std::string> result;
  std::list<std::string>::const_iterator it;
  for(it=i->second.begin(); it!=i->second.end(); ++it) {
	  result.push_back(*it);
  }
  return result;
}

bool SmartACE::SmartIniParameter::getString(const std::string &prefix, const std::string &param, std::string &s) const 
{
  try {
    const std::string tmp = getString(prefix, param);
    s = tmp;
    return true;
  } catch(SmartACE::IniParameterError &e) {
    std::cerr << e.what() << std::endl;
  }
    return false;
}

bool SmartACE::SmartIniParameter::getStringList(const std::string &prefix, const std::string &param, std::list<std::string> &s) const 
{
  try {
    s = getStringList(prefix, param);
    return true;
  } catch(SmartACE::IniParameterError &e) {
    std::cerr << e.what() << std::endl;
  }
    return false;
}

void SmartACE::SmartIniParameter::getline(std::istream &is, std::string &line) const
{
  std::ostringstream oss;
  bool escaped = false; // "true" means last character was a backslash 
                        // which has not yet been appended to the buffer
  char ch;
  while(!is.get(ch).eof())
  {
    if(ch=='\\')
    {
      if(escaped)
      {
        oss.put(ch); // write the previous backslash, not the current.
      }
      escaped = true; // do not append backslash to buffer until next character is read
    }
    else
    {
      if(!escaped)
      {
        if(ch=='\n') break; // done.
        oss.put(ch);
      }
      else
      {
        if(ch!='\n') 
        {
          oss.put('\\');
          oss.put(ch);
        }
        // else ignore newline
      }
      escaped = false; // next character isn't escaped
    }
  }
  line = oss.str();
}
