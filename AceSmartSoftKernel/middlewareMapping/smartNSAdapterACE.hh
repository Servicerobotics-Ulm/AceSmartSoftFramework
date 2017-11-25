// --------------------------------------------------------------------------
//
//  Copyright (C) 2012 Alex Lotz
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

#ifndef SMARTNSHELPERACE_HH_
#define SMARTNSHELPERACE_HH_

#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Unbounded_Queue.h>
#include <ace/Name_Space.h>

#include <vector>
#include <map>
#include <iostream>

#include "smartNSValueType.hh"
#include "smartNSProxy.hh"

namespace SmartACE {

/** \class NSKeyType
 *
 *  This helper class encapsulates the "name" part of the name-value pair of a naming-service entry.
 *  Thereby the middleware specific representation (with corresponding conversions) is implemented once inside
 *  this class.
 */
class NSKeyType
{
public:
   // default constructor
   NSKeyType()
   {
      reset_names();
   }

   // the main data container (can be directly used without further access methods)
   std::vector<ACE_TString> names;

   // enumeration to easily access name-strings in the "names" member (e.g. as shown in the reset_names() method below)
   enum NAME_POS
   {
      COMP_NAME, // ns-level 1
      PATTERN_NAME, // ns-level 2
      SERVICE_NAME, // ns-level 3
      COMMOBJ1_NAME, // ns-level 4
      COMMOBJ2_NAME, // ns-level 5
      NUMBER_ELEMENTS
   };

   // simple helper to set the name-strings to initial values (asterisks represent arbitrary strings as in regular expressions)
   void reset_names()
   {
      names.resize(NUMBER_ELEMENTS);
      names[COMP_NAME] = "*";
      names[PATTERN_NAME] = "*";
      names[SERVICE_NAME] = "*";
      names[COMMOBJ1_NAME] = "*";
      names[COMMOBJ2_NAME] = "*";
   }

   // middleware specific conversion operator
   operator ACE_TString() const;
};

/** \class NSAdapterACE
 *
 *  This class defines the <em>client-side</em> interface of the <b>Naming-Service</b>.
 *  With that <em>binding/unbinding/rebinding</em> of <b><em>name-value</em></b> pairs in
 *  Naming-Service is possible. In addition some helper functions are available.
 */
class NSAdapterACE
{
protected:
   // Address which is set during initialization using either
   // (a) parameters passed as console parameters
   // (b) environment variable SMART_NS_ADDR
   // (c) or set to default values (see setConfiguration method)
   ACE_INET_Addr ns_address;
   // Address which is set during initialization using either
   // (a) parameters passed as console parameters
   // (b) environment variable SMART_IP
   // (c) or set to default values (see setConfiguration method)
   ACE_INET_Addr component_ip;
   // This variable specifies, whether a the component_ip was specified from
   // components command line arguments and thus should be used or otherwise
   // a so called "smartip" functionality from the naming service is used, which
   // works for many netwok configuration but has some limitations.
   bool use_component_ip;
   // Connector which connects to the remote NamingContext
   ACE_Connector<NSProxy, ACE_SOCK_CONNECTOR> connector;
   // Pointer to the remote NamingContext
   NSProxy *namingService;

   /* @internal helper function to extract naming-service parameters as follows:
    * 1) if console parameters ("--ns-ip" with <IP> and/or "--ns-port" with <nbr>
    *    and/or "--component-ip" with <IP>) are available in parameters then 
    *    they overwrite the following configs (highest priority)
    * 2) The environment variable $SMART_NS_ADDR containing a string
    *    "<hostname>:<port-nbr>" sets up teh "--ns-ip" and "--ns-port" params
    *    if these are not defined in 1). The environment variable 
    *    $SMART_IP defines the IP address for "--component-ip" if not already
    *    defined in 1).
    * 3) If none of the above parameters are used, then the naming-service is
    *    initialized with the following default parameters
    *    --ns-ip: "localhost"
    *    --ns-port: ACE_DEFAULT_SERVER_PORT
    *    --cmoponent-ip: "localhost"
    */
   int setConfiguration(const std::map<std::string,std::string> &parameters);

   /* @internal helper method that parses the argc/argv paramerers, selects only
    * those which have a value part and stores the key-value pairs in a map
    * which is the return value.
    */
   std::map<std::string,std::string> parseCommandLineParams(const int &argc, char ** argv, const int &skip_args=1) const;

   // this mutex ensures serialized method access among all threads in the process
   // this is necessary, because parallel access can lead to corrupted results...
   mutable ACE_Recursive_Thread_Mutex mutex;

   // this member variable is true if the context is successfully initialized (e.g. by calling the init method below)
   bool contextInitialized;

public:
   /// default constructor
   /**
    * Default constructor does NOT initialize internal naming-service connection. To fully initialize it, use either
    * the constructor with the naming-service string or call the init() method directly.
    * @sa init()
    */
   NSAdapterACE();

   /// constructor that initializes internal naming-service
   /**
    * This constructor fully initializes the naming-service by internally calling the init() method.
    * @param argc the number of console-parameter arguments
    * @param argv the list of console parameters
    */
   NSAdapterACE(const int &argc, char** argv);

   /// default destructor
   /**
    * This Destructor is responsible to close the naming-context (being the remote name-space-connection).
    */
   virtual ~NSAdapterACE();

   /// initialize internal naming-service and check whether it started correctly
   /**
    * This method fully initializes the naming-service and checks if it starts successfully. After that
    * all methods (like bind, unbind, resolve etc.) can be used.
    * @param argc defined the number of console parameters
    * @param argv contains the list of console parameters
    * @return 0 on success
    * @return -1 on failure during initialization
    */
   int init(const int &argc, char** argv);

   /// an alternative initialization method using parameters passed in a map of name-value pairs
   /**
    * This method fully initializes the naming-service and checks if it starts successfully. After that
    * all methods (like bind, unbind, resolve etc.) can be used. Thereby the parameters are passed
    * in the std::map parameters which contains entries with the first part being the parameter name
    * and the second part being the corresponding parameter value.
    * @param parameters contains the parameter name-value pairs
    * @return 0 if initialization was successfully
    * @return -1 if initialization fails
    */
   int init(const std::map<std::string,std::string> &parameters);

   /// @internal close down naming-context
   void fini(void);

   /// check for help argument (e.g. -h or --help) and print out the help text
   /**
    *  If one of the arguments in the ARGV vector contains the help argument (e.g. -h or --help)
    *  then a text explaining all the valid naming service arguments is printed out using ostr.
    */
   void checkForHelpArg(const int &argc, char * const * argv, std::ostream &ostr = std::cout);

   /// function to <b>bind</b> a name-value pair in naming-context.
   /**
    * If the name exists in naming-context, it will be <b>not</b> overridden
    * and an error value is returned.
    * @param name Name (key) for the value to be stored in the naming-context (must be a fully qualified name without asterisks, except for commobj2)
    * @param value The value object to be bound with the name
    * @return 0 on success
    * @return -1 on failure or existing name
    * @sa rebind()
    * @sa unbind()
    * @sa resolve()
    */
   int bind(const NSKeyType &name, const NSValueType &value);

   /// function to <b>rebind</b> a name-value pair in naming-context.
   /**
    * If the name exists in naming-context, the value will be overridden <b>silently</b>.
    * @param name Name (key) for the value to be stored in the naming-context (must be a fully qualified name without asterisks, except for commobj2)
    * @param value The value object to be bound with the name
    * @return 0 on success
    * @return -1 on failure
    * @sa bind()
    * @sa resolve()
    * @sa unbind()
    */
   int rebind(const NSKeyType &name, const NSValueType &value);

   /// function to <b>resolve</b> (to find) a value, giving its name (key), in naming-context.
   /**
    * Search a name in naming-context, resolve its value and return it in the value parameter.
    * @param name Name to resolve (must be a fully qualified name without asterisks, except for commobj2)
    * @param value Resulting value to the corresponding name
    * @return 0 on success (found name)
    * @return -1 on not found or failure
    * @sa bind()
    * @sa rebind()
    * @sa unbind()
    */
   int resolve(const NSKeyType &name, NSValueType &value);

   /// function to <b>unbind</b> (to delete) a name-value pair from naming-context.
   /**
    * This function searches the name (key) in the naming-context and unbinds (deletes)
    * corresponding name-value pair (entry).
    * @param name Name (key) to unbind (delete) the corresponding name-value pair (must be a fully qualified name without asterisks, except for commobj2)
    * @return 0 on success
    * @return -1 on not found or failure
    * @sa bind()
    * @sa rebind()
    * @sa resolve()
    */
   int unbind(const NSKeyType &name);

   /// recursive unbind function.
   /**
    * This function is <b>similar</b> to unbind(). Here however, <b>all</b> name-value pairs,
    * matching the searchPattern will be unbound sequentially.
    * This function is typically used in SmartComponent in case of strg+c abortion to clean up all
    * potential dead-entries from the naming-service!
    * @param searchPattern Is used as pattern to find all name-value pairs (use asterisks to generalize variable parts of the search pattern).
    * @return 0 on success
    * @return -1 on failure
    * @sa getEntriesForMatchingPattern()
    */
   int unbindEntriesForMatchingPattern(const NSKeyType &searchPattern);

   /// get a list of naming-service entries from naming-context for a given search-pattern
   /**
    * This method queries the naming-service for a list of entries, that match the given search-pattern.
    * For each part of the search-pattern either a specific name can be set (e.g. a certain component name)
    * or an asterisk ("*") indicates this part to be an arbitrary string (like in regular expressions).
    * Thereby, the search-pattern is evaluated as follows: for each entry in the naming service it is checked, whether it matches
    * <comp-name> && <pattern-name> && <service-name> && <commobj1-name> && <commobj2-name>. For example, to query for a
    * list of all services of a component, the search pattern looks as follows: ("MyComponent" "*" "*" "*" "*").
    * @param searchPattern The search-pattern to be matched against each entry in the naming-service
    * @return A list of all entries in the naming-service that match the search-pattern. This list is empty if no matches are
    * found or an error occurs.
    */
   ACE_Unbounded_Queue<NSKeyType> getEntriesForMatchingPattern(
         const NSKeyType &searchPattern) const;
};

// NamingService defined as Singleton
typedef ACE_Singleton<NSAdapterACE, ACE_Thread_Mutex> NAMING;

} /* namespace Smart */

#endif /* SMARTNSHELPERACE_HH_ */
