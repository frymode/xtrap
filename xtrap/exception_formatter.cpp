#include "exception_formatter.h"
#include <sstream>
#include <map>

#include "exception.h"
#include "win32/symbol_server.h"
#include "win32/untyped_exception.h"
#include "win32/exception_trap.h"

namespace xtrap
{
    namespace 
    {
        std::string ExtractExceptionType(const type_info& type)
        {
            std::string type_name = type.name();
            int space_pos = type_name.find_last_of(' ');
            if (space_pos >= 0)
            {
                type_name = type_name.substr(space_pos + 1);
            }
            return type_name;
        }

        /*
        Regex exp_throw("(.*): Throw in function (.*)");
        Regex exp_function("([^ ]*)\\(");
        std::string ExtractFunctionName(const std::string& full_name)
        {
            MatchResults what;
            if (regex_search(full_name, what, exp_function))
            {
                return what[1];
            }

            return full_name;
        }

        Regex exp_exception_type("clone_impl<[^>]* ([^ >]*)>");
        std::string ExtractExceptionType(const std::string& info)
        {
            MatchResults what;
            if (regex_search(info, what, exp_exception_type))
            {
                return what[1];
            }

            return info;
        }

        Regex exp_exception_detail("^\\[struct .*_(.*)_ \\*\\] = (.*)");
        bool ExtractErrorDetails(const std::string& info, std::string* name, std::string* value)
        {
            MatchResults what;
            if (regex_search(info, what, exp_exception_detail))
            {
                *name = what[1];
                *value = what[2];

                replace_all(*name, "_", " ");
                return true;
            }

            return false;
        }*/

        void PrintStringVector(std::stringstream& stream, const std::string& key, const std::vector<std::string>& strings)
        {
            stream << "  " << key << ": ";
            switch (strings.size())
            {
            case 0:
                stream << std::endl;
                break;
            case 1:
                stream << strings[0] << std::endl;
                break;
            default:
                stream << std::endl;
                for (size_t i = 0; i < strings.size(); ++i)
                {
                    stream << "    " << strings[i] << std::endl;
                }
                break;
            }
        }
    }

    std::string format_exception(const std::exception& e, bool extract_stack_trace /* = true */)
    {
        std::string exception_type = ExtractExceptionType(typeid(e));
        std::stringstream res;
        res << "Exception of type " << exception_type << std::endl;
        res << "  Message: " << e.what() << std::endl;

        if (extract_stack_trace)
        {
            BacktraceStack stack = GetCurrentExceptionStackTrace(e);
            if (stack)
            {
                PrintStringVector(res, "Stack trace", stack);
            }
        }

        return res.str();
    }

    std::string format_exception(const boost::exception& e, bool extract_stack_trace /* = true */)
    {
        return format_boost_exception_details(diagnostic_information_what(e));
    }

    std::string format_exception(const Exception& e)
    {
        return e.what();
    }

    std::string format_boost_exception_details(const std::string& boost_details)
    {
        return boost_details;
        /*
        std::string file_name;
        std::string function_name;
        std::string exception_type;
        std::map<std::string, std::vector<std::string>> details;

        std::vector<std::string> lines = split(boost_details, "\r\n");
        MatchResults what;
        if (lines.size() > 1)
        {
            if (regex_search(lines[0], what, exp_throw))
            {
                std::string file_path = what[1];
                std::string function = what[2];

                file_name = ExtractFileName(file_path);
                function_name = ExtractFunctionName(function);
            }
            else
            {
                function_name = lines[0];
            }

            exception_type = ExtractExceptionType(lines[1]);
            int x = 1;
        }

        auto cur_detail = details.end();
        for (size_t i = 2; i < lines.size(); ++i)
        {
            const std::string& line = lines[i];
            std::string name;
            std::string value;
            if (ExtractErrorDetails(line, &name, &value))
            {
                cur_detail = details.insert(make_pair(name, std::vector<std::string>())).first;
                cur_detail->second.push_back(value);
            }
            else if (cur_detail != details.end() && !line.empty())
            {
                cur_detail->second.push_back(line);
            }
        }

        std::stringstream stream;
        stream << "Exception of type " << exception_type << std::endl;
        stream << "  Thrown at " << function_name;
        if (!file_name.empty())
        {
            stream << "   " << file_name;
        }
        stream << std::endl;

        auto i_det = details.find("Message");
        if (i_det != details.end())
        {
            PrintStringVector(stream, i_det->first, i_det->second);
        }

        for (i_det = begin(details); i_det != end(details); ++i_det)
        {
            if (i_det->first != "Message" && i_det->first != "Backtrace")
            {
                PrintStringVector(stream, i_det->first, i_det->second);
            }
        }

        i_det = details.find("Backtrace");
        if (i_det != details.end())
        {
            PrintStringVector(stream, "Stack trace", i_det->second);
        }

        return stream.str();
        */
    }
}