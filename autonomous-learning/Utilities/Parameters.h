#ifndef __BasicMarkovBrainTemplate__Parameters__
#define __BasicMarkovBrainTemplate__Parameters__

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

// This structure aggregates all information about a single configurable parameter
template<class T>
struct Entry {
  T variable;  // The current value of the parameter
  string documentation;  // A string descrbing the purpose of the parameter
  string classification;  // A string that broadly indicates what this Entry is involved in
  Entry() = default;
  Entry(T default_value, string doc, string cl = "none")
      : variable(default_value),
        documentation(doc),
        classification(cl) {
  }
};

// Create a "registry" for each type of configurable parameter
template<class T>
unordered_map<string, Entry<T>>& registry() {
  static unordered_map<string, Entry<T>> result;
  return result;
}

class Parameters {
 public:

  static unordered_map<string, string> readCommandLine(int argc, const char** argv) {
    unordered_map<string, string> comand_line_list;

    cout << "  - loading Command Line.\n";

    int argCount = argc;
    if ((argc & 1) == 0) {
      argCount = argc - 1;
      cout << "  - Command Line: too many arguments on command line... ignoring last argument.\n";
    }
    for (int i = 1; i < argCount; i = i + 2) {
      if (comand_line_list.find(string(argv[i])) == comand_line_list.end()) {
        comand_line_list[string(argv[i])] = string(argv[i + 1]);
      } else {
        cout << "  - \"" << string(argv[i]) << "\" is defined more then once on the command line. Using first definition!\n";
      }
    }
    cout << "\n";
    return comand_line_list;
  }

  static unordered_map<string, string> readConfigFile(string configFileName) {
    unordered_map<string, string> config_file_list;
    set<char> nameFirstLegalChars = {  // characters that can be used as the first "letter" of a name
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_' };
    set<char> nameLegalChars = {  // characters that can be used in the body of a name
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '_', ':' };
    set<char> whiteSpaceChars = { ' ', '\t' };

    string line;
    int line_number = 0;
    unsigned int index;
    string parameterName;
    string parameterValue;

    cout << "  - loading Config File \"" << configFileName << "\"\n";

    ifstream configFile(configFileName);  // open file named by configFileName
    if (configFile.is_open())  // if the file named by configFileName can be opened
    {
      while (getline(configFile, line))  // keep loading one line from the file at a time into "line" until we get to the end of the file
      {
        line_number++;
        index = 0;
        parameterName = "";
        parameterValue = "";
        // move past any leading white space
        while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
          index++;
        }
        // comment can start before name, if it does, we are done with this line... move to end of line
        if (index < line.length() && line[index] == '#') {
          index = line.length();
        }
        // get name (must start with letter or "_")
        if (index < line.length() && nameFirstLegalChars.find(line[index]) != nameFirstLegalChars.end()) {
          parameterName += line[index];
          index++;
        } else if (index < line.length()) {  // if the first character on the line is not white space, # or a name start character
          parameterName = "__";  // set the paramterName to something
          index = line.length();  // move to end of line (this will create a error since there is a name, but no value)
        }
        // get rest of name
        while (index < line.length() && nameLegalChars.find(line[index]) != nameLegalChars.end()) {
          parameterName += line[index];
          index++;
        }
        // move past white space between name and "="
        while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
          index++;
        }
        // the next character must be "="
        if (index < line.length() && line[index] == '=') {
          index++;
          // move past white space between "=" and value
          while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
            index++;
          }
          // get value : values can be made up of any characters
          while (index < line.length() && whiteSpaceChars.find(line[index]) == whiteSpaceChars.end() && line[index] != '#') {
            parameterValue += line[index];
            index++;
          }
          // move though whitespace till "#" or EoL
          while (index < line.length() && (whiteSpaceChars.find(line[index]) != whiteSpaceChars.end() || line[index] == '#')) {
            if (line[index] == '#') {  // if "#" move to end of line
              index = line.length();
            } else {  // move past whitespace
              index++;
            }
          }
        }
        if (index != line.length()) {  // if not at end of line, there was a problem
          cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << configFileName << "\". Ignoring line.\n";
        } else {
          // if there is a name and a value
          if (parameterName != "" && parameterValue != "") {
            if (config_file_list.find(parameterName) == config_file_list.end()) {
              config_file_list[parameterName] = string(parameterValue);
            } else {
              cout << "  - \"" << string(parameterName) << "\" is defined more then once in config file (line " << line_number << "). Using first definition!\n";
            }

          } else if (parameterName != "" && parameterValue == "") {  // if there is a name but no value, throw warning
            cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << configFileName << "\". Ignoring line.\n";
          }
        }
      }
      configFile.close();
    }

    else {
      cout << "  - Config File: WARNING unable to open file \"" << configFileName << "\". Proceeding without.\n";
    }
    cout << "\n";
    return config_file_list;

  }

// Use static calls to set up the parameter before main calls "set parameters"
  template<class T>
  static T& register_parameter(string name, const T& default_value, const string& documentation, const string& classification = "none") {
    if (registry<T>().find(name) != registry<T>().end()) {
      cout << "  - registering parameters :: \"" << name << "\" is being registered more then once... you should look into this!\n";
    }
    registry<T>()[name] = Entry<T>(default_value, documentation, classification);
    return registry<T>()[name].variable;
  }

// Put an arbitrary value to the target variable, return false on conversion failure
  template<class T>
  static bool load_value(const string& value, T& target) {
    std::stringstream ss(value);
    ss >> target;
    if (ss.fail()) {
      return false;
    } else {
      string remaining;
      ss >> remaining;
      // stream failure means nothing left in stream, which is what we want
      return ss.fail();
    }
  }

// Loads all of the parameters for a given type of variable from the key_value map
  template<class T>
  static void load_parameters_of_type(unordered_map<string, string> & key_value) {
    for (auto & pair : registry<T>()) {
      const auto & it = key_value.find(pair.first);
      // Only set the configuration if no argument given
      if (it != key_value.end()) {
        bool success = load_value(it->second, pair.second.variable);
        if (!success) {
          throw std::invalid_argument("Bad configuration option for '" + it->first + "' given: " + it->second);
        }
        key_value.erase(pair.first);
      }
    }
  }

  static void load_parameters(unordered_map<string, string> & key_value) {
    load_parameters_of_type<int>(key_value);
    load_parameters_of_type<double>(key_value);
    load_parameters_of_type<bool>(key_value);
    load_parameters_of_type<string>(key_value);
  }

  static void initialize_parameters(int argc, const char** argv) {

    cout << "-----\n" << "  You can use your own configFile by setting the comand line flag \"configFileName\" to the name of your config file (default: configFile settings.cfg).\n" << "  to write out a new config file set the comnad line flag \"makeConfigFile\" to 1.\n" << "-----\n";

    unordered_map<string, string> comand_line_list = readCommandLine(argc, argv);

    string configFileName = (comand_line_list.find("configFileName") == comand_line_list.end()) ? "settings.cfg" : comand_line_list["configFileName"];

    comand_line_list.erase("configFileName");

    unordered_map<string, string> config_file_list = readConfigFile(configFileName);

    load_parameters(config_file_list);
    load_parameters(comand_line_list);

    string makeConfigFile = (comand_line_list.find("makeConfigFile") == comand_line_list.end()) ? "0" : comand_line_list["makeConfigFile"];
    comand_line_list.erase("makeConfigFile");
    if (makeConfigFile == "1") {
      ofstream CONFIGFILE(configFileName);
      dump_parameters(CONFIGFILE);
      CONFIGFILE.close();
      cout << "  - \"" << configFileName << "\" has been created. Exiting.\n";
      exit(0);
    }

    if (!comand_line_list.empty()) {
      cout << "  - The following values were defined on the command line, but were not used:\n";
      for (auto name : comand_line_list) {
        cout << "  -  \"" << name.first << "\"\n";
      }
    }
    if (!config_file_list.empty()) {
      cout << "  - The following values were defined in the config file, but were not used:\n";
      for (auto name : config_file_list) {
        cout << "  -  \"" << name.first << "\"\n";
      }
    }

  }

  // Junk you already had
  static string get_type(int parameter) {
    return "int";
  }
  static string get_type(float parameter) {
    return "float";
  }
  static string get_type(double parameter) {
    return "double";
  }
  static string get_type(bool parameter) {
    return "bool";
  }
  static string get_type(string parameter) {
    return "string";
  }

// Output the type, name, current value, and doc string for each parameter of a given type
  template<class T>
  static void dump_parameters_of_type(map<string, map<string, string>> & sorted_parameters) {
    ostringstream build_string;

    for (const auto & parameter : registry<T>()) {

      build_string << parameter.first << " = " << parameter.second.variable;

      int size = build_string.str().size();
      while (size < 35) {
        build_string << " ";
        size++;
      }

      build_string << " # (" << get_type(parameter.second.variable) << ") " << parameter.second.documentation;

      sorted_parameters[parameter.second.classification][parameter.first] = build_string.str();

      build_string.str(string());
    }
  }

  static void dump_parameters(ostream & out) {

    map<string, map<string, string>> sorted_parameters;

    dump_parameters_of_type<int>(sorted_parameters);
    dump_parameters_of_type<double>(sorted_parameters);
    dump_parameters_of_type<bool>(sorted_parameters);
    dump_parameters_of_type<string>(sorted_parameters);

    if (sorted_parameters.find("GLOBAL") != sorted_parameters.end()) {
      out << "# GLOBAL:" << "\n";
      for (auto parameter : sorted_parameters["GLOBAL"]) {
        out << "  " << parameter.second << "\n";
      }

    }

    sorted_parameters.erase("GLOBAL");

    out << "\n";

    for (auto group : sorted_parameters) {
      out << "# " << group.first << ":\n";
      for (auto parameter : group.second) {
        out << "  " << parameter.second << "\n";
      }
      out << "\n";
    }

  }

}
;

#endif // __BasicMarkovBrainTemplate__Parameters__

