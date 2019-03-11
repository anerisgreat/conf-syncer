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

std::vector<conf_field> process_files_for_dest(
        std::vector<conf_file_entry>::iterator start_iter,
        std::vector<conf_file_entry>::iterator end_iter)
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

std::vector<file_entry> process_conf_file(char* conf_fname){
    std::ifstream readfconf(conf_fname);
    std::vector<conf_file_entry> conf_entries;
    std::string line;
    //Read all files logged by CMake
    while(getline(readfconf, line)){
        conf_entries.push_back(conf_file_entry(line));
    }

    //Sort by destination file
    sort(conf_entries.begin(), conf_entries.end(),
        []( const conf_file_entry& a, const conf_file_entry&b ){
            return a.dst_file.compare(b.dst_file) > 0;
    });

    //List of struct of files to take care of
    std::vector<file_entry> files_to_include;

    std::vector<conf_file_entry>::iterator start_iter = conf_entries.begin(); 
    //Run with all entires
    while(start_iter != conf_entries.end()){
        //Group managed entries
        if(start_iter->is_managed){
            std::vector<conf_file_entry>::iterator end_iter = start_iter + 1;
            while(end_iter != conf_entries.end() && 
                (*end_iter).dst_file == (*start_iter).dst_file)
                {++end_iter;}
            std::vector<conf_field> fields = 
                process_files_for_dest(start_iter, end_iter);

            file_entry to_add(start_iter->dst_file,
                    start_iter->alias,
                    fields);
            files_to_include.push_back(to_add);

            start_iter = end_iter;
        }
        else{
            file_entry to_add(start_iter->dst_file,
                    start_iter->alias);
            files_to_include.push_back(to_add);
            //Assume no two unmanaged files with same dst one after another
                //(input sanitized by CMake)
            ++start_iter;
        }
    }

    return files_to_include;
}

void generate_outputs(std::vector<file_entry> conf_files,
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
            FILE* fout_conf = fopen((file_iter->alias + ".conf").c_str(), "w");
            if (!fout_conf)
                exit(EXIT_FAILURE);

            for(auto field_iter = file_iter->file_fields.begin();
                    field_iter != file_iter->file_fields.end();
                    ++field_iter )
            {
                fprintf(fout_conf, 
                        (field_iter->get_conf_line() + '\n').c_str());
            }

            fclose(fout_conf);
        }
    }
    FILE* fout_header = fopen(out_header_fname.c_str(), "w");
    if (!fout_header)
        exit(EXIT_FAILURE);

    FILE* fout_source = fopen(out_source_fname.c_str(), "w");
    if (!fout_source )
        exit(EXIT_FAILURE);


    fclose(fout_header);
    fclose(fout_source);
}

int main(int argc, char** argv)
{
    printf("Generating configuration files.\n");

    std::vector<file_entry> output_files = process_conf_file(argv[3]);
    generate_outputs(output_files, argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
