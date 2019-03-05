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
        while(getline(readfconf, line)){
            bool full_field = false;
            bool partial_field = false;
            bool is_title = false;
            conf_field to_add;
            std::smatch matches;
            //Check all matches
            if(std::regex_search(line, matches, comp_field_int_re)){
                to_add.type = field_type::intf;
                full_field = true;
                int int_val;
                sscanf(matches[2].str().c_str(), "%d", &int_val);
                to_add.default_int = int_val;
            }
            else if(std::regex_search(line, matches, comp_field_flt_re)){
                to_add.type = field_type::fltf;
                full_field = true;
                float float_val;
                sscanf(matches[2].str().c_str(), "%f", &float_val);
                to_add.default_float = float_val;
            }
            else if(std::regex_search(line, matches, comp_field_str_re)){
                to_add.type = field_type::strf;
                full_field = true;
                to_add.default_str = matches[2].str();
            }
            else if(std::regex_search(line, matches, comp_field_int_arr_re)){
                to_add.type = field_type::arrintf;
                full_field = true;
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
            else if(std::regex_search(line, matches, comp_field_flt_arr_re)){
                to_add.type = field_type::arrfltf;
                full_field = true;
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
            else if(std::regex_search(line, matches, comp_field_str_arr_re)){
                to_add.type = field_type::arrstrf;
                full_field = true;
                std::string arrstr= matches[2].str();
                for(auto it = std::sregex_iterator(arrstr.begin(),
                                                    arrstr.end(),
                                                    comp_str_re);
                    it != std::sregex_iterator();
                    ++it)
                {
                    to_add.default_str_arr.push_back((*it).str());
                }
            }

            //Adding field
            if(full_field || partial_field || is_title){
                to_add.field_name = matches[1];
                to_add.proj = iter->proj_name;
            }

            to_add.is_title = is_title;

            if(full_field || is_title){
                fields.push_back(to_add);
            }
            else if(partial_field){
                to_check.push_back(to_add);
            }
        }
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
    //FOR TEST

    process_conf_file(argv[2]);

    exit(EXIT_SUCCESS);

    //printf("%zu\n", count_chars(rfbuff, '\n'));

}
