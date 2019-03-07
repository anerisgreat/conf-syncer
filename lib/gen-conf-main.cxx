#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen-conf-main.h"
#include "conf-regex.h"
#include <regex>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>

void process_files_for_dest(std::vector<conf_file_entry>::iterator start_iter, 
        std::vector<conf_file_entry>::iterator end_iter)
{
    std::vector<conf_field> fields;
    std::vector<conf_field> to_check;

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
                to_add.default_str = quoted_str.substr(1, quoted_str.size() - 2);
            }
            //ARR fields
            //Int arr field
            else if(std::regex_search(linebuff, matches, comp_field_int_arr_re)){
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
            else if(std::regex_search(linebuff, matches, comp_field_flt_arr_re)){
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
            else if(std::regex_search(linebuff, matches, comp_field_str_arr_re)){
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
            else{
                std::cout << "Invalid line!" << std::endl;
                throw "Invalid line!";
            }

            //Adding field
            if(!is_empty){
                to_add.field_name = matches[1];
                to_add.proj = iter->proj_name;
                if(is_noval){
                    to_check.push_back(to_add);
                }
                else{
                    fields.push_back(to_add);
                }
            }
        }
    }//Finish reading conf files

    for(auto iter = fields.begin(); iter != fields.end(); ++iter){
        std::cout << iter->get_conf_line() << std::endl;
    }

    //Check validity
    //Generate
}

void process_conf_file(char* conf_fname){
    std::ifstream readfconf(conf_fname);
    std::vector<conf_file_entry> conf_entries;
    std::string line;
    while(getline(readfconf, line)){
        conf_entries.push_back(conf_file_entry(line));
    }

    sort(conf_entries.begin(), conf_entries.end(),
        []( const conf_file_entry& a, const conf_file_entry&b ){
            return a.dst_file.compare(b.dst_file) > 0;
    });

    std::vector<conf_file_entry>::iterator start_iter = conf_entries.begin(); 
    while(start_iter != conf_entries.end()){
        std::vector<conf_file_entry>::iterator end_iter = start_iter + 1;
        while(end_iter != conf_entries.end() && 
            (*end_iter).dst_file == (*start_iter).dst_file)
            {++end_iter;}
        process_files_for_dest(start_iter, end_iter);
        start_iter = end_iter;
    }
}

int main(int argc, char** argv)
{
    /*if(!compile_regex_fields()){
        printf("ERROR COMPILING REGEX\n");
    }*/
    //FOR TEST
    printf("Generating configuration files.\n");

    FILE *fout = fopen(argv[1],"w");
    if (!fout)
    {
        exit(EXIT_FAILURE);
    }

    fprintf(fout,"#define THE_STRING \"CHEESE\"");
    fclose(fout);
    printf("Generating configuration files.\n");

    FILE *fout = fopen(argv[2],"w");
    if (!fout)
    {
        exit(EXIT_FAILURE);
    }

    fprintf(fout,"#define THE_STRING \"CHEESE\"");
    fclose(fout);
    //FOR TEST

    process_conf_file(argv[3]);

    exit(EXIT_SUCCESS);

    //printf("%zu\n", count_chars(rfbuff, '\n'));

}
