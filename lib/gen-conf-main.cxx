#include <stdlib.h>
#include <string.h>
#include <regex>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>

//__________REGEX______________________________________________________________
#define FIELD_NAME_RE "[a-zA-Z_]+[a-zA-Z0-9\\-_]*"

#define INT_RE "\\-?[0-9]+"
#define FLT_RE "\\-?[0-9]*\\.[0-9]+"
#define STR_RE "\\\"(?:[^\"\\\\]|\\\\.)*\\\""

#define VAL_FIELD_START "^[ \t]*(" FIELD_NAME_RE ")[ \t]*=[ \t]*"
#define VAL_FIELD_END "[ \t]*$"

#define FIELD_TITLE_RE "^[ \t]*\\[([^\"\\[\\]]+)\\][ \t]*$"
#define FIELD_NOVAL_RE "^[ \t]*(" FIELD_NAME_RE ")[ \t]*" \
    "(int|float|string|arr_int|arr_float|arr_string)" VAL_FIELD_END

#define FIELD_INT_RE VAL_FIELD_START "(" INT_RE ")" VAL_FIELD_END
#define FIELD_FLT_RE VAL_FIELD_START "(" FLT_RE ")" VAL_FIELD_END
#define FIELD_STR_RE VAL_FIELD_START "(" STR_RE ")" VAL_FIELD_END

#define FIELD_INT_ARR_RE VAL_FIELD_START \
    "\\[([ \t]*" INT_RE "(?:[ \t]+" INT_RE ")*)[ \t]*\\]" VAL_FIELD_END
#define FIELD_FLT_ARR_RE VAL_FIELD_START \
    "\\[[ \t]*(" FLT_RE "(?:[ \t]+" FLT_RE ")*)[ \t]*\\]" VAL_FIELD_END
#define FIELD_STR_ARR_RE VAL_FIELD_START \
    "\\[[ \t]*(" STR_RE "(?:[ \t]+" STR_RE ")*)[ \t]*\\]" VAL_FIELD_END

#define EMPTY_LINE_RE "^[ \t]*$"

static std::regex comp_field_int_re(FIELD_INT_RE);
static std::regex comp_field_flt_re(FIELD_FLT_RE);
static std::regex comp_field_str_re(FIELD_STR_RE);
static std::regex comp_int_re(INT_RE);
static std::regex comp_flt_re(FLT_RE);
static std::regex comp_str_re(STR_RE);
static std::regex comp_field_int_arr_re(FIELD_INT_ARR_RE);
static std::regex comp_field_flt_arr_re(FIELD_FLT_ARR_RE);
static std::regex comp_field_str_arr_re(FIELD_STR_ARR_RE);
static std::regex comp_field_title_re(FIELD_TITLE_RE);
static std::regex comp_field_noval_re(FIELD_NOVAL_RE);
static std::regex comp_empty_line_re(EMPTY_LINE_RE);

#define INDENT_1 "    "
#define INDENT_2 INDENT_1 INDENT_1
#define INDENT_3 INDENT_2 INDENT_1
#define INDENT_4 INDENT_3 INDENT_1
#define INDENT_5 INDENT_4 INDENT_1
#define INDENT_6 INDENT_5 INDENT_1
#define INDENT_7 INDENT_6 INDENT_1


//DEFINES
#define GLOBAL_HEADER \
"#include <stdio.h>\n"\
"#include <stdlib.h>\n"\
"#include <string.h>\n"\
"typedef enum {intf, fltf, strf, arrintf, arrfltf, arrstrf} field_type;\n"\
"void msg_and_exit(char* msg, char* line, int linen){\n"\
"    printf(\"\%s\", msg);\n"\
"    printf(\"\\nLine #:%d\\n\%s\\n\", linen, line);\n"\
"    exit(EXIT_FAILURE);\n"\
"}\n"\
"\n"\
"void app_and_incr(char* buff, int* index, char to_append){\n"\
"    buff[*index] = to_append;\n"\
"    buff[*index + 1] = \'\\0\';\n"\
"    (*index)++;\n"\
"}\n"\
"void app_and_incr_escaped(char* buff, int* index, char to_append){\n"\
"    switch(to_append){\n"\
"        case('n'):\n"\
"            app_and_incr(buff, index, '\\n');\n"\
"            break;\n"\
"        case('\\\"'):\n"\
"            app_and_incr(buff, index, '\\\"');\n"\
"            break;\n"\
"        case('t'):\n"\
"            app_and_incr(buff, index, '\\t');\n"\
"            break;\n"\
"        case('\\\\'):\n"\
"            app_and_incr(buff, index, '\\\\');\n"\
"            break;\n"\
"        case('r'):\n"\
"            app_and_incr(buff, index, '\\r');\n"\
"            break;\n"\
"        case('f'):\n"\
"            app_and_incr(buff, index, '\\f');\n"\
"            break;\n"\
"        default:\n"\
"            printf(\"Bad escape sequence!\");\n"\
"            exit(EXIT_FAILURE);\n"\
"            break;\n"\
"    }\n"\
"}\n"

//Preceeded with "int {PROJ NAME}
#define _PARSE_VALUE_A
"_parse_value(char* namebuff, char* valbuff, char* line, int linen){\n"\
"\n"\
"}\n"

//Preceeded with "int {PROJ NAME}

#define _LOAD_FROM_PATH_STR_A \
"    FILE * fp;\n"\
"    char * line = NULL;\n"\
"    int linen = 0;\n"\
"    size_t len = 0;\n"\
"    ssize_t read;\n\n"\
"    fp = fopen(path, \"r\");\n"\
"    if (fp == NULL)\n"\
"        exit(EXIT_FAILURE);\n"\
"    while((read = getline(&line, &len, fp))!= -1){\n"\
"        linen++;\n"\
"        char namebuff[128];\n"\
"        char valbuff[1024];\n"\
"        int namebuff_index = 0;\n"\
"        int valbuff_index = 0;\n"\
"        enum{pre_name, name, post_name, pre_val_num, pre_val_str, \n"\
"            pre_val_arr, val_num, val_str, val_arr,\n"\
"            val_str_escaped, val_arr_escaped,\n"\
"            post_val, title} reader_state = pre_name;\n"\
"        field_type ftype;\n"\
"        for(ssize_t i = 0; i < read; ++i){\n"\
"            int periodf;\n"\
"            char c = line[i];\n"\
"            switch(reader_state){;\n"\
"                case(pre_name):\n"\
"                    switch(c){\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                            break;\n"\
"                        case('['):\n"\
"                            reader_state = title;\n"\
"                            break;\n"\
"                        default:\n"\
"                            app_and_incr(namebuff, &namebuff_index, c);\n"\
"                            reader_state = name;\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(name):\n"\
"                    switch(c){\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                            reader_state = post_name;\n"\
"                            break;\n"\
"                        default:\n"\
"                            app_and_incr(namebuff, &namebuff_index, c);\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(title):\n"\
"                    switch(c){\n"\
"                        case(']'):\n"\
"                            reader_state = post_val;\n"\
"                            break;\n"\
"                        default:\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(post_name):\n"\
"                    switch(c){\n"\
"                        case('='):\n"\
"                            ftype = "
#define _LOAD_FROM_PATH_STR_B \
"_get_field_type(namebuff);\n"\
"\n"\
"                            switch(ftype){\n"\
"                               case(intf):\n"\
"                               case(fltf):\n"\
"                                   reader_state = pre_val_num; break;\n"\
"                               case(strf):\n"\
"                                   reader_state = pre_val_str; break;\n"\
"                               case(arrintf):\n"\
"                               case(arrfltf):\n"\
"                               case(arrstrf):\n"\
"                                   reader_state = pre_val_arr; break;\n"\
"                            }\n"\
"                            break;\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                            break;\n"\
"                        default:\n"\
"                            msg_and_exit(\"Bad line!\", line, linen);\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(pre_val_num):\n"\
"                    switch(c){\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                            break;\n"\
"                        default:\n"\
"                            if((c >= '0' && c <= '9') || (c == '-') || \n"\
"                                (c == '.' && ftype == fltf))\n"\
"                            {\n"\
"                                app_and_incr(valbuff, &valbuff_index, c);\n"\
"                                if(c == '.'){\n"\
"                                    periodf = 1;\n"\
"                                }\n"\
"                                else{\n"\
"                                    periodf = 0;\n"\
"                                }\n"\
"                                reader_state = val_num;\n"\
"                            }\n"\
"                            else{\n"\
"                                msg_and_exit(\"Bad line!\", line, linen);\n"\
"                            }\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(pre_val_str):\n"\
"                    switch(c){\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                            break;\n"\
"                        case(\'\\\"\'):\n"\
"                            reader_state = val_str;\n"\
"                            break;\n"\
"                        default:\n"\
"                            msg_and_exit(\"Bad line!\", line, linen);\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(val_num):\n"\
"                    switch(c){\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                        case('\\n'):\n"\
"                            break;\n"\
"                            //END;\n"\
"                        default:\n"\
"                            if((c >= '0' && c <= '9') ||\n"\
"                                (c == '.' && ftype == fltf && periodf == 0))\n"\
"                            {\n"\
"                                \n"\
"                                app_and_incr(valbuff, &valbuff_index, c);\n"\
"                                if(c == '.'){\n"\
"                                    periodf = 1;\n"\
"                                }\n"\
"                            }\n"\
"                            else{\n"\
"                                msg_and_exit(\"Bad line!\", line, linen);\n"\
"                            }\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(val_str):\n"\
"                    switch(c){\n"\
"                        case('\\\\'):\n"\
"                            reader_state = val_str_escaped;\n"\
"                            break;\n"\
"                        case(\'\\\"\'):\n"\
"                            reader_state = post_val;\n"\
"                            //END;\n"\
"                            break;\n"\
"                        case('\\n'):\n"\
"                            msg_and_exit(\"Bad line!\", line, linen);\n"\
"                            //END\n"\
"                            break;\n"\
"                        default:\n"\
"                            app_and_incr(valbuff, &valbuff_index, c);\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(val_str_escaped):\n"\
"                    app_and_incr_escaped(valbuff, &valbuff_index, c);\n"\
"                    reader_state = val_str;\n"\
"                    break;\n"\
"                case(pre_val_arr):\n"\
"                    switch(c){\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                            break;\n"\
"                        case('['):\n"\
"                            reader_state = val_arr;\n"\
"                            break;\n"\
"                        default:\n"\
"                            msg_and_exit(\"Bad line!\", line, linen);\n"\
"                    }\n"\
"                    break;\n"\
"                case(val_arr):\n"\
"                    switch(c){\n"\
"                        case('\\\\'):\n"\
"                            app_and_incr(valbuff, &valbuff_index, c);\n"\
"                            reader_state = val_arr_escaped;\n"\
"                            break;\n"\
"                        case(']'):\n"\
"                            reader_state = post_val;\n"\
"                            //END;\n"\
"                            break;\n"\
"                        default:\n"\
"                            app_and_incr(valbuff, &valbuff_index, c);\n"\
"                            break;\n"\
"                    }\n"\
"                    break;\n"\
"                case(val_arr_escaped):\n"\
"                    app_and_incr(valbuff, &valbuff_index, c);\n"\
"                    reader_state = val_arr;\n"\
"                    break;\n"\
"                case(post_val):\n"\
"                    switch(c){\n"\
"                        case(' '):\n"\
"                        case('\\t'):\n"\
"                        case('\\n'):\n"\
"                            break;\n"\
"                        default:\n"\
"                            msg_and_exit(\"Bad line!\", line, linen);\n"\
"                    }\n"\
"                    break;\n"\
"                default: break;\n"\
"            }\n"\
"        }\n"\
"    }\n"\
"    fclose(fp);\n"\
"    if(line)\n"\
"        free(line);\n"\
"}\n"

std::string get_file_name_no_extention(const std::string& s) {
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    char file_extend = '.'; 
    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        std::string fname = s.substr(i+1, s.length() - i);
        size_t j = fname.rfind(file_extend, fname.length());
        return(fname.substr(0, j));
    }

    return("");
}

struct conf_file_in{
    public:
        std::string src_file;
        std::string dst_file;
        bool is_managed;
        std::string alias;

        conf_file_in(std::string in_str){
            size_t split_a = in_str.find(':');
            size_t split_b = in_str.find(':', split_a + 1);
            size_t split_c = in_str.find(':', split_b + 1);
            size_t split_d = in_str.find(':', split_c + 1);
            src_file = in_str.substr(0, split_a);
            dst_file = in_str.substr((split_a + 1), (split_b - split_a - 1));
            is_managed = (
                in_str.substr(
                    split_b+1, (split_c - split_b - 1)).compare("TRUE") == 0);
            if(is_managed){
                alias = get_file_name_no_extention(dst_file);
            }
            else{
                alias = in_str.substr(split_c+1, (in_str.size() - split_c - 1));
            }
        }

        bool operator<(const conf_file_in& comp){
            return dst_file.compare(comp.dst_file) > 0;
        }
};

enum field_type{
    intf,
    fltf,
    strf,
    arrintf,
    arrfltf,
    arrstrf,
    title
};

struct conf_field{
    public:
        //std::string src[MAX_FIELD_LEN]; //?
        //char dst[MAX_FIELD_LEN]; //?
        std::string field_name;
        int default_int;
        float default_float;
        std::string default_str;

        std::vector<int> default_int_arr;
        std::vector<float> default_flt_arr;
        std::vector<std::string> default_str_arr;

        field_type type; //IS THERE ENUM MUST MAKE

        conf_field() :
            field_name(""),
            default_int(0),
            default_float(0),
            default_str(""),
            default_int_arr(),
            default_flt_arr(),
            default_str_arr()
        {}

        void get_conf_line(std::stringstream& outs){
            switch(type){
                case field_type::intf :
                    outs << field_name << " = " << default_int; break;
                case field_type::fltf :
                    outs << field_name << " = " << default_float; break;
                case field_type::strf:
                    outs << field_name << " = \"" << default_str \
                        << "\""; break;
                case field_type::arrintf:
                    outs << field_name << " = [ ";
                    if(default_int_arr.size() > 0){
                        for(auto iter = default_int_arr.begin();
                                iter != default_int_arr.end();
                                ++iter)
                        {
                            outs << *iter << ' ';
                        }
                    }
                    outs << ']';
                    break;
                case field_type::arrfltf:
                    outs << field_name << " = [ ";
                    if(default_flt_arr.size() > 0){
                        for(auto iter = default_flt_arr.begin();
                                iter != default_flt_arr.end();
                                ++iter)
                        {
                            outs << *iter << ' ';
                        }
                    }
                    outs << ']';
                    break;
                case field_type::arrstrf:
                    outs << field_name << " = [ ";
                    if(default_str_arr.size() > 0){
                        for(auto iter = default_str_arr.begin();
                                iter != default_str_arr.end();
                                ++iter)
                        {
                            outs << '"' << *iter << "\" ";
                        }
                    }
                    outs << ']';
                    break;
                case field_type::title:
                    outs << "[ " << field_name << " ]";
                    break;
            }
        }

        void get_parse_lines(std::stringstream& outs){
            if(type != field_type::title){
                outs << INDENT_1 << \
                    "if(strcmp(namebuff, \"" << field_name << "\") == 0){\n";

                switch(type){
                    case field_type::intf:
                        outs << INDENT_2 << "char* end;\n";
                        outs << INDENT_2 << alias << '.' << field_name << \
                            ";\n";
                        outs << "\n";
                        break;
                }
                outs << INDENT_1 << "}\n";
            }
        }

        void get_field_type_str(std::stringstream& outs){
            if(type == field_type::intf ||
                    type == field_type::fltf ||
                    type == field_type::strf ||
                    type == field_type::arrintf ||
                    type == field_type::arrfltf ||
                    type == field_type::arrstrf)
            {
                outs << INDENT_1 << \
                    "if(strcmp(namebuff, \"" << field_name << "\") == 0){\n" \
                        << INDENT_2 << "return ";
                switch (type){
                    case field_type::intf:
                        outs << "intf"; break;
                    case field_type::fltf:
                        outs << "fltf"; break;
                    case field_type::strf:
                        outs << "strf"; break;
                    case field_type::arrintf:
                        outs << "arrintf"; break;
                    case field_type::arrfltf:
                        outs << "arrfltf"; break;
                    case field_type::arrstrf:
                        outs << "arrstrf"; break;
                    default: break;
                }
                outs << ";\n" << INDENT_1 << "}\n";
            }
        }

        void get_header_struct_str(std::stringstream& outs){
            switch (type){
                case field_type::intf:
                    outs << INDENT_1 << "int " << field_name << ";\n"; break;
                case field_type::fltf:
                    outs << INDENT_1 << "float " << field_name << ";\n"; break;
                case field_type::strf:
                    outs << INDENT_1 << "char* " << field_name << ";\n"; break;
                case field_type::arrintf:
                    outs << INDENT_1 << "int* " << field_name << ";\n"; break;
                case field_type::arrfltf:
                    outs << INDENT_1 << "float* " << field_name << ";\n"; break;
                case field_type::arrstrf:
                    outs << INDENT_1 << "char** " << field_name << ";\n"; break;
                default: break;
            }

            if(type == field_type::arrintf || type == field_type::arrfltf ||
                    type == field_type :: arrstrf)
            {
                outs << INDENT_1 << "size_t " << field_name << "_nelements;\n";
            }
        }
};

struct conf_file_out{
    public:
        bool is_managed;
        std::string alias;
        std::vector<conf_field> file_fields;
        std::string conf_full_path;

        conf_file_out(std::string file, 
                std::string alias_str, 
                std::vector<conf_field> fields):
            is_managed(true),
            alias(alias_str),
            file_fields(fields),
            conf_full_path(file)
        {}

        conf_file_out(std::string file, 
                std::string alias_str):
            is_managed(false),
            alias(alias_str),
            conf_full_path(file),
            file_fields()
        {}

        void get_conf_str(std::stringstream& outs){
            for(auto iter = file_fields.begin();
                    iter != file_fields.end();
                    ++iter)
            {
                iter->get_conf_line(outs);
                outs << '\n';
            }
        }

    void get_header_str(std::stringstream& outs){
        outs << "static const char " << alias << "_path[] = \"" << \
            conf_full_path << "\";\n";
        if(is_managed){
            outs << "struct " << alias << "_struct{\n";
            for(auto field_iter = file_fields.begin();
                    field_iter != file_fields.end();
                    ++field_iter)
            {
                field_iter->get_header_struct_str(outs);
            }

            outs << "}" << alias << ";\n";

            outs << "field_type " << alias << \
                "_get_field_type(char* namebuff){\n";
            for(auto field_iter = file_fields.begin();
                    field_iter != file_fields.end();
                    ++field_iter)
            {
                field_iter->get_field_type_str(outs);
            }

            outs << INDENT_1 << "printf(\"Invalid field name!\\n" << \
                                "\%s\\n\", namebuff);\n" << \
                                INDENT_1 << "exit(EXIT_FAILURE);\n";

            outs << "}\n";


            outs << "int " << alias << "_load_from_path(const char* path){\n";
            outs << _LOAD_FROM_PATH_STR_A << alias << _LOAD_FROM_PATH_STR_B;
        }
    }
};

std::vector<conf_field> process_files_for_dest(
        std::vector<conf_file_in>::iterator start_iter,
        std::vector<conf_file_in>::iterator end_iter)
{
    std::vector<conf_field> fields;
    std::vector<conf_field> to_check;

    //Open line by line
    for(auto iter = start_iter; iter != end_iter; ++iter){
        std::ifstream readfconf(iter->src_file);
        std::string line;
        std::string linebuff;
        while(getline(readfconf, line)){
            linebuff = line;
            bool is_noval = false;
            bool is_empty = false;
            conf_field to_add;
            std::smatch matches;
            //Check all matches
            //Int field
            if(std::regex_search(linebuff, matches, comp_field_int_re)){
                to_add.type = field_type::intf;
                int int_val;
                sscanf(matches[2].str().c_str(), "%d", &int_val);
                to_add.default_int = int_val;
            }
            //Float field
            else if(std::regex_search(linebuff, matches, comp_field_flt_re)){
                to_add.type = field_type::fltf;
                float float_val;
                sscanf(matches[2].str().c_str(), "%f", &float_val);
                to_add.default_float = float_val;
            }
            //String field
            else if(std::regex_search(linebuff, matches, comp_field_str_re)){
                to_add.type = field_type::strf;
                std::string quoted_str = matches[2].str();
                to_add.default_str = 
                    quoted_str.substr(1, quoted_str.size() - 2);
            }
            //ARR fields
            //Int arr field
            else if(std::regex_search(linebuff,
                        matches,
                        comp_field_int_arr_re))
            {
                to_add.type = field_type::arrintf;
                std::string arrstr= matches[2].str();
                for(auto it = std::sregex_iterator(arrstr.begin(),
                                                    arrstr.end(),
                                                    comp_int_re);
                    it != std::sregex_iterator();
                    ++it)
                {
                    int int_val;
                    sscanf((*it).str().c_str(), "%d", &int_val);
                    to_add.default_int_arr.push_back(int_val);
                }
            }
            //Float arr field
            else if(std::regex_search(linebuff,
                        matches,
                        comp_field_flt_arr_re)){
                to_add.type = field_type::arrfltf;
                std::string arrstr= matches[2].str();
                for(auto it = std::sregex_iterator(arrstr.begin(),
                                                    arrstr.end(),
                                                    comp_flt_re);
                    it != std::sregex_iterator();
                    ++it)
                {
                    float flt_val;
                    sscanf((*it).str().c_str(), "%f", &flt_val);
                    to_add.default_flt_arr.push_back(flt_val);
                }
            }
            //String arr field
            else if(std::regex_search(linebuff,
                        matches,
                        comp_field_str_arr_re))
            {
                to_add.type = field_type::arrstrf;
                std::string arrstr= matches[2].str();
                for(auto it = std::sregex_iterator(arrstr.begin(),
                                                    arrstr.end(),
                                                    comp_str_re);
                    it != std::sregex_iterator();
                    ++it)
                {
                    std::string quoted_str = (*it).str();
                    to_add.default_str_arr.push_back(\
                                quoted_str.substr(1, quoted_str.size() - 2));
                }
            }
            else if(std::regex_search(linebuff, matches, comp_field_title_re)){
                to_add.type = field_type::title;
            }
            else if(std::regex_search(linebuff, matches, comp_empty_line_re)){
                is_empty = true;
            }
            else if(std::regex_search(linebuff, matches, comp_field_noval_re)){
                is_noval = true;
                std::string val_type_str = matches[2].str();
                if(val_type_str.compare("int") == 0){
                    to_add.type = field_type::intf;
                }
                else if(val_type_str.compare("float") == 0){
                    to_add.type = field_type::fltf;
                }
                else if(val_type_str.compare("string") == 0){
                    to_add.type = field_type::strf;
                }
                else if(val_type_str.compare("arr_int") == 0){
                    to_add.type = field_type::arrintf;
                }
                else if(val_type_str.compare("arr_float") == 0){
                    to_add.type = field_type::arrfltf;
                }
                else if(val_type_str.compare("arr_string") == 0){
                    to_add.type = field_type::arrstrf;
                }
            }
            else{
                std::cout << "Invalid line!" << std::endl;
                std::cout << linebuff << std::endl;
                //TODO: THROW SOMETHING BETTER
                throw "Invalid line!";
            }

            //Adding field
            if(!is_empty){
                to_add.field_name = matches[1];
                if(is_noval){
                    to_check.push_back(to_add);
                }
                else{
                    fields.push_back(to_add);
                }
            }
        }
    }//Finish reading conf files

    //Checking that all the 'to check' fields exist
    for(auto iter = to_check.begin(); iter != to_check.end(); ++iter){
        if(iter->type != field_type::title){
            bool found = false;
            for(auto fielditer = fields.begin();
                    fielditer != fields.end() && !found;
                    ++fielditer)
            {
                if(fielditer->type != field_type::title){
                    if(iter->field_name.compare(fielditer->field_name) == 0){
                        if(iter->type == fielditer->type){
                            found = true;
                        }
                        else{
                            std::cout << "The field " << iter->field_name << \
                                " was defined twice with different types."<< 
                                std::endl;
                            throw "Mismatch types!";
                        }
                    }
                }
            }
            if(!found){
                std::cout << "The field " << iter->field_name << \
                    " was defined without value and never redefined with one.";
                throw "No default value!";
            }
        }
    }

    //Checking for duplicate field types 
    for(auto iter = fields.begin(); iter != fields.end(); ++iter){
        if(iter->type != field_type::title){
            for(auto compiter = (iter + 1);
                    compiter != fields.end();
                    ++compiter)
            {
                if(compiter->type != field_type::title){
                    if(iter->field_name.compare(compiter->field_name) == 0){
                        std::cout << "Duplicate field " << iter->field_name << \
                            std::endl;
                        throw "Duplicate fields!";
                    }
                }
            }
        }
    }

    //Generate
    return fields;
}

std::vector<conf_file_out> process_conf_file(char* conf_fname){
    std::ifstream readfconf(conf_fname);
    std::vector<conf_file_in> conf_entries;
    std::string line;
    //Read all files logged by CMake
    while(getline(readfconf, line)){
        conf_entries.push_back(conf_file_in(line));
    }

    //Sort by destination file
    sort(conf_entries.begin(), conf_entries.end(),
        []( const conf_file_in& a, const conf_file_in&b ){
            return a.dst_file.compare(b.dst_file) > 0;
    });

    //List of struct of files to take care of
    std::vector<conf_file_out> files_to_include;

    std::vector<conf_file_in>::iterator start_iter = conf_entries.begin(); 
    //Run with all entires
    while(start_iter != conf_entries.end()){
        //Group managed entries
        if(start_iter->is_managed){
            std::vector<conf_file_in>::iterator end_iter = start_iter + 1;
            while(end_iter != conf_entries.end() && 
                (*end_iter).dst_file == (*start_iter).dst_file)
                {++end_iter;}
            std::vector<conf_field> fields = 
                process_files_for_dest(start_iter, end_iter);

            conf_file_out to_add(start_iter->dst_file,
                    start_iter->alias,
                    fields);
            files_to_include.push_back(to_add);

            start_iter = end_iter;
        }
        else{
            conf_file_out to_add(start_iter->dst_file,
                    start_iter->alias);
            files_to_include.push_back(to_add);
            //Assume no two unmanaged files with same dst one after another
                //(input sanitized by CMake)
            ++start_iter;
        }
    }

    return files_to_include;
}

void generate_outputs(std::vector<conf_file_out> conf_files,
        std::string out_header_fname,
        std::string out_source_fname)
{
    //Generate configuration files
    //Write headers
        //Make structure
        //Fill with fields
        //

    //Generating configuration files
    for(auto file_iter = conf_files.begin();
            file_iter != conf_files.end();
            ++file_iter)
    {
        if(file_iter->is_managed){
            std::ofstream outstream;
            outstream.open(file_iter->alias + ".conf");
            std::stringstream buff;
            file_iter->get_conf_str(buff);
            outstream << buff.rdbuf();
            outstream.close();
        }
    }

    std::ofstream headeroutstream;
    std::stringstream headerbuff;

    //Global includes
    headerbuff << GLOBAL_HEADER;

    for(auto file_iter = conf_files.begin();
            file_iter != conf_files.end();
            ++file_iter)
    {
        file_iter->get_header_str(headerbuff);
    }

    headeroutstream.open(out_header_fname);
    headeroutstream << headerbuff.rdbuf();
    headeroutstream.close();

    FILE* fout_source = fopen(out_source_fname.c_str(), "w");
    if (!fout_source )
        exit(EXIT_FAILURE);

    fclose(fout_source);
}

int main(int argc, char** argv)
{
    printf("Generating configuration files.\n");

    std::vector<conf_file_out> output_files = process_conf_file(argv[3]);
    generate_outputs(output_files, argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}


