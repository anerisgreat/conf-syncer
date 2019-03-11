#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <iostream>

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

struct conf_file_entry{
    public:
        std::string src_file;
        std::string dst_file;
        bool is_managed;
        std::string alias;

        conf_file_entry(std::string in_str){
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
                std::cout << "MANAGED ALIAS: " << alias << std::endl;
            }
            else{
                alias = in_str.substr(split_c+1, (in_str.size() - split_c - 1));
            }
        }

        bool operator<(const conf_file_entry& comp){
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

        std::string get_conf_line(){
            std::stringstream ret_str;

            switch(type){
                case field_type::intf :
                    ret_str << field_name << " = " << default_int; break;
                case field_type::fltf :
                    ret_str << field_name << " = " << default_float; break;
                case field_type::strf:
                    ret_str << field_name << " = \"" << default_str \
                        << "\""; break;
                case field_type::arrintf:
                    ret_str << field_name << " = [ ";
                    if(default_int_arr.size() > 0){
                        for(auto iter = default_int_arr.begin();
                                iter != default_int_arr.end();
                                ++iter)
                        {
                            ret_str << *iter << ' '; }
                    }
                    ret_str << ']';
                    break;
                case field_type::arrfltf:
                    ret_str << field_name << " = [ ";
                    if(default_flt_arr.size() > 0){
                        for(auto iter = default_flt_arr.begin();
                                iter != default_flt_arr.end();
                                ++iter)
                        {
                            ret_str << *iter << ' ';
                        }
                    }
                    ret_str << ']';
                    break;
                case field_type::arrstrf:
                    ret_str << field_name << " = [ ";
                    if(default_str_arr.size() > 0){
                        for(auto iter = default_str_arr.begin();
                                iter != default_str_arr.end();
                                ++iter)
                        {
                            ret_str << '"' << *iter << "\" ";
                        }
                    }
                    ret_str << ']';
                    break;
                case field_type::title:
                    ret_str << "[ " << field_name << " ]";
                    break;
            }
            return ret_str.str();
        }
};

struct file_entry{
    public:
        bool is_managed;
        std::string alias;
        std::vector<conf_field> file_fields;
        std::string conf_full_path;

        file_entry(std::string file, 
                std::string alias_str, 
                std::vector<conf_field> fields):
            is_managed(true),
            alias(alias_str),
            file_fields(fields),
            conf_full_path(file)
        {}

        file_entry(std::string file, 
                std::string alias_str):
            is_managed(false),
            alias(alias_str),
            conf_full_path(file),
            file_fields()
        {}
};

std::vector<file_entry> process_conf_file(char* conf_fname);

std::vector<conf_field> process_files_for_dest(
        std::vector<conf_file_entry>::iterator start_iter, 
        std::vector<conf_file_entry>::iterator end_iter);

void generate_outputs(std::vector<file_entry> conf_files,
        std::string out_header_fname,
        std::string out_source_fname);

int main(int argc, char** argv);
