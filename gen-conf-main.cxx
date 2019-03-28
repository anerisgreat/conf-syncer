#include <stdlib.h>
#include <string.h>
#include <regex>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>

#include "gen-conf-structs.h"
#include "gen-conf-c.h"
#include "gen-conf-py.h"

//__________REGEX______________________________________________________________
#define FIELD_NAME_RE "[a-zA-Z_]+[a-zA-Z0-9\\-_]*"

#define INT_RE "\\-?[0-9]+"
#define FLT_RE "\\-?[0-9]*\\.[0-9]+"
#define STR_RE "\\\"(?:[^\\\"\\\\]|\\\\.)*\\\""

#define VAL_FIELD_START "^[ \t]*(" FIELD_NAME_RE ")[ \t]*=[ \t]*"
#define VAL_FIELD_END "[ \t]*$"

#define FIELD_TITLE_RE "^[ \t]*\\[([^\\[\\]]+)\\][ \t]*$"
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

conf_file_in get_conf_file_in(std::string in_str){
    conf_file_in retval;
    size_t split_a = in_str.find(':');
    size_t split_b = in_str.find(':', split_a + 1);
    size_t split_c = in_str.find(':', split_b + 1);
    size_t split_d = in_str.find(':', split_c + 1);
    retval.src_file = in_str.substr(0, split_a);
    retval.dst_file = in_str.substr((split_a + 1), (split_b - split_a - 1));
    retval.is_managed = (
        in_str.substr(
            split_b+1, (split_c - split_b - 1)).compare("TRUE") == 0);
    if(retval.is_managed){
        retval.alias = get_file_name_no_extention(retval.dst_file);
    }
    else{
        retval.alias = in_str.substr(split_c+1, (in_str.size() - split_c - 1));
    }
    retval.is_python =(in_str.substr(
                split_d+1, (in_str.size() - split_d - 1)).compare("TRUE") == 0);
    return retval;
}

std::string get_conf_line(conf_field* cfield){
    std::string field_name = cfield->field_name;
    std::stringstream outs;
    switch(cfield->type){
        case field_type::intf :
            outs << field_name << " = " << cfield->default_int; break;
        case field_type::fltf :
            outs << field_name << " = " << cfield->default_float; break;
            double integral;
            if(std::modf(cfield->default_float, &integral) == 0){
                outs << ".0";
            }

        case field_type::strf:
            outs << field_name << " = \"" << cfield->default_str \
                << "\""; break;
        case field_type::arrintf:
            outs << field_name << " = [ ";
            if(cfield->default_int_arr.size() > 0){
                for(auto iter = cfield->default_int_arr.begin();
                        iter != cfield->default_int_arr.end();
                        ++iter)
                {
                    outs << *iter << ' ';
                }
            }
            outs << ']';
            break;
        case field_type::arrfltf:
            outs << field_name << " = [ ";
            if(cfield->default_flt_arr.size() > 0){
                for(auto iter = cfield->default_flt_arr.begin();
                        iter != cfield->default_flt_arr.end();
                        ++iter)
                {
                    outs << *iter;
                    double integral;
                    if(std::modf(*iter, &integral) == 0){
                        outs << ".0";
                    }
                    outs << ' ';
                }
            }
            outs << ']';
            break;
        case field_type::arrstrf:
            outs << field_name << " = [ ";
            if(cfield->default_str_arr.size() > 0){
                for(auto iter = cfield->default_str_arr.begin();
                        iter != cfield->default_str_arr.end();
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

    return outs.str();
}

void get_conf_str(conf_file_out* cfile, std::stringstream& outs){
    for(auto iter = cfile->file_fields.begin();
            iter != cfile->file_fields.end();
            ++iter)
    {
        outs << get_conf_line(&*iter) << '\n';
    }
}

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
                    " was defined without value and never redefined with on.";
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
        conf_entries.push_back(get_conf_file_in(line));
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
            bool is_python = start_iter->is_python;
            std::vector<conf_file_in>::iterator end_iter = start_iter + 1;
            while(end_iter != conf_entries.end() && 
                (*end_iter).dst_file == (*start_iter).dst_file)
            {
                ++end_iter;
                is_python = is_python | end_iter->is_python;
            }
            std::vector<conf_field> fields = 
                process_files_for_dest(start_iter, end_iter);

            conf_file_out to_add(start_iter->dst_file,
                    start_iter->alias,
                    fields,
                    is_python);
            files_to_include.push_back(to_add);

            start_iter = end_iter;
        }
        else{
            conf_file_out to_add(start_iter->dst_file,
                    start_iter->alias,
                    start_iter->is_python);
            files_to_include.push_back(to_add);
            //Assume no two unmanaged files with same dst one after another
                //(input sanitized by CMake)
            ++start_iter;
        }
    }

    return files_to_include;
}

void generate_outputs(std::vector<conf_file_out> conf_files,
        std::string out_cgen_fname,
        std::string out_py_fname)
{
    //Generating configuration files
    for(auto file_iter = conf_files.begin();
            file_iter != conf_files.end();
            ++file_iter)
    {
        if(file_iter->is_managed){
            std::ofstream outstream;
            outstream.open(file_iter->alias + ".conf");
            std::stringstream buff;
            get_conf_str(&*file_iter, buff);
            outstream << buff.rdbuf();
            outstream.close();
        }
    }

    cgen(conf_files, out_cgen_fname);
    pygen(conf_files, out_py_fname);
}

int main(int argc, char** argv)
{
    printf("Generating configuration files.\n");

    std::vector<conf_file_out> output_files = process_conf_file(argv[3]);
    generate_outputs(output_files, argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}

