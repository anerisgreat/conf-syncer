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
        std::string field_name;
        int default_int;
        float default_float;
        std::string default_str;

        std::vector<int> default_int_arr;
        std::vector<float> default_flt_arr;
        std::vector<std::string> default_str_arr;

        field_type type;

        conf_field() :
            field_name(""),
            default_int(0),
            default_float(0),
            default_str(""),
            default_int_arr(),
            default_flt_arr(),
            default_str_arr()
        {}

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
