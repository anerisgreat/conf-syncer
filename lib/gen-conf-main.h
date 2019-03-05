#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include <string>
#include <vector>

struct conf_file_entry{
    public:
        std::string src_file;
        std::string dst_file;
        std::string proj_name;

        conf_file_entry(std::string in_str){
            size_t split_a = in_str.find(':');
            size_t split_b = in_str.find(':', split_a + 1);
            src_file = in_str.substr(0, split_a);
            dst_file = in_str.substr(split_a + 1, split_b);
            proj_name = in_str.substr(split_b, in_str.size());
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
    notype
};

struct conf_field{
    public:
        //std::string src[MAX_FIELD_LEN]; //?
        //char dst[MAX_FIELD_LEN]; //?
        std::string proj;
        std::string field_name;
        bool is_title;
        int default_int;
        float default_float;
        std::string default_str;

        std::vector<int> default_int_arr;
        std::vector<float> default_flt_arr;
        std::vector<std::string> default_str_arr;

        field_type type; //IS THERE ENUM MUST MAKE

        conf_field() :
            proj(""),
            field_name(""),
            is_title(false),
            default_int(0),
            default_float(0),
            default_str(""),
            type(field_type::notype),
            default_int_arr(),
            default_flt_arr(),
            default_str_arr()
        {}
};

void process_files_for_dest(std::vector<conf_file_entry>::iterator start_iter, 
        std::vector<conf_file_entry>::iterator end_iter);

int main(int argc, char** argv);
