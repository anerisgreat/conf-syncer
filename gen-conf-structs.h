#ifndef GEN_CONF_STRUCTS_H
#define GEN_CONF_STRUCTS_H

#define INDENT_1 "    "
#define INDENT_2 INDENT_1 INDENT_1
#define INDENT_3 INDENT_2 INDENT_1
#define INDENT_4 INDENT_3 INDENT_1
#define INDENT_5 INDENT_4 INDENT_1
#define INDENT_6 INDENT_5 INDENT_1
#define INDENT_7 INDENT_6 INDENT_1

enum field_type{
    intf,
    fltf,
    strf,
    arrintf,
    arrfltf,
    arrstrf,
    title
};

struct conf_file_in{
    public:
        std::string src_file;
        std::string dst_file;
        bool is_managed;
        bool is_python;
        std::string alias;

        conf_file_in(){}

        bool operator<(const conf_file_in& comp){
            return dst_file.compare(comp.dst_file) > 0;
        }
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

        void get_py_str(std::stringstream& outs){
            if(type != field_type::title){
                outs << INDENT_2 << "elif(";
                switch (type){
                    case field_type::intf:
                        outs << "isintf"; break;
                    case field_type::fltf:
                        outs << "isfltf"; break;
                    case field_type::strf:
                        outs << "isstrf"; break;
                    case field_type::arrintf:
                        outs << "isarrintf"; break;
                    case field_type::arrfltf:
                        outs << "isarrfltf"; break;
                    case field_type::arrstrf:
                        outs << "isarrstrf"; break;
                    default: break;
                }
                outs << " and ";
                switch (type){
                    case field_type::intf:
                        outs << "fintm"; break;
                    case field_type::fltf:
                        outs << "ffltm"; break;
                    case field_type::strf:
                        outs << "fstrm"; break;
                    case field_type::arrintf:
                        outs << "farrintm"; break;
                    case field_type::arrfltf:
                        outs << "farrfltm"; break;
                    case field_type::arrstrf:
                        outs << "farrstrm"; break;
                    default: break;
                }
                outs << ".group(1) == '" << field_name << "'):\n";
                outs << INDENT_3 << "retval." << field_name << " = ";
                switch (type){
                    case field_type::intf:
                        outs << "int(fintm.group(2))"; break;
                    case field_type::fltf:
                        outs << "float(ffltm.group(2))"; break;
                    case field_type::strf:
                        outs << "fstrm.group(2)[1:len(fstrm.group(2)) - 1].decode('string-escape')"; break;
                    case field_type::arrintf:
                        outs << "[int(match) for match in "\
                            << "re.compile(r'\\-?[0-9]+')"\
                            << ".findall(farrintm.group(2))]"; break;
                    case field_type::arrfltf:
                        outs << "[float(match) for match in "\
                            << "re.compile(r'\\-?[0-9]*\\.[0-9]+')"\
                            << ".findall(farrfltm.group(2))]"; break;
                    case field_type::arrstrf:
                        outs << "[match[1:len(match)-1].decode('string-escape') for match in "\
                            << "re.compile(r'\\\"(?:[^\\\"\\\\]|\\\\.)*\\\"')"\
                            << ".findall(farrstrm.group(2))]"; break;
                }

                outs << "\n\n";
            }
        }
};

struct conf_file_out{
    public:
        bool is_managed;
        std::string alias;
        std::vector<conf_field> file_fields;
        std::string conf_full_path;
        std::string conf_fname;
        bool is_python;

        conf_file_out(std::string file, 
                std::string alias_str, 
                std::vector<conf_field> fields,
                bool is_make_python):
            is_managed(true),
            alias(alias_str),
            file_fields(fields),
            conf_full_path(file),
            is_python(is_make_python)
        {}

        conf_file_out(std::string file, 
                std::string alias_str,
                bool is_make_python):
            is_managed(false),
            alias(alias_str),
            conf_full_path(file),
            file_fields(),
            is_python(is_make_python)
        {}
};

#endif
