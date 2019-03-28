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
"                        case('\\n'):\n"\
"                        case('\\0'):\n"\
"                            reader_state = post_val;\n"\
"                        break;\n"\
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
"                        case('\\0'):\n"\
"                            load_stupid_param(namebuff, valbuff, line, linen);\n"\
"                            reader_state = post_val;\n"\
"                            break;\n"\
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
"                            load_stupid_param(namebuff, valbuff, line, linen);\n"\
"                            break;\n"\
"                        case('\\n'):\n"\
"                        case('\\0'):\n"\
"                            msg_and_exit(\"Bad line!\", line, linen);\n"\
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
"                            load_stupid_param(namebuff,\n"\
"                                valbuff,\n"\
"                                line, \n"\
"                                linen);\n"\
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
"                        case('\\0'):\n"\
"                            break;\n"\
"                        default:\n"\
"                            msg_and_exit(\"Bad line!\", line, linen);\n"\
"                    }\n"\
"                    break;\n"\
"                default: break;\n"\
"            }\n"\
"        }\n"\
"        if(reader_state != pre_name && reader_state != post_val){\n"\
"            msg_and_exit(\"Bad line!\", line, linen);\n"\
"        }\n"\
"    }\n"\
"    fclose(fp);\n"\
"    if(line)\n"\
"        free(line);\n"\
"}\n"

#define INDENT_1 "    "
#define INDENT_2 INDENT_1 INDENT_1
#define INDENT_3 INDENT_2 INDENT_1
#define INDENT_4 INDENT_3 INDENT_1
#define INDENT_5 INDENT_4 INDENT_1
#define INDENT_6 INDENT_5 INDENT_1
#define INDENT_7 INDENT_6 INDENT_1

std::string cgen_field_type_str(field_type ftype){
    std::string typestr;
    switch (cfield->type){ 
        case field_type::intf: case field_type::arrintf: typestr = "int"; break;
        case field_type::fltf: case field_type::arrfltf: typestr = "flt"; break;
        case field_type::strf: case field_type::arrstrf: typestr = "char*"; break;
        default: break;
    }

    if(cfield->type == field_type::arrintf ||
        cfield->type == field_type::arrfltf ||
        cfield->type == field_type::arrstrf)
    {
        typestr += '*';
    }
    return typestr
}

#define CGEN_STRUCT_FIELD_F \
"    ${TYPESTR} ${FIELDNAME};\n"
#define CGEN_STRUCT_FIELD_ARRF \
CGEN_STRUCT_FIELD_F \
"    size_t ${FIELDNAME}_nelements;\n"

std::string cgen_get_header_struct_str(conf_field* cfield){
    if(cfield->type == field_type::title)
        return;
    std::map<std::string, std::string> replace_dict;
    replace_dict["FIELDNAME"] = cfield->field_name;
    replace_dict["TYPESTR"] = cgen_field_type_str(cfield->type);

    if(type==field_type::intf||type==field_type::fltf||type==field_type::strf){
        return replace_with_val_dict(CGEN_STRUCT_FIELD_F, replace_dict);
    }
    else{
        return replace_with_val_dict(CGEN_STRUCT_FIELD_ARRF, replace_dict);
    }
}

#define CGEN_STRUCT_STR
"struct ${ALIAS}_struct{\n"\
"${STRUCT_FIELDS_STR}"\
"}${ALIAS};\n"

std::string get_struct_str(conf_file_out* cfile){
    std::string struct_fields_str = "";
    for(auto iter = cfile->file_fields.begin();
            iter != cfile->file_fields.end();
            ++iter)
    {
        struct_fields_str += cgen_get_header_struct_str(&*iter);
    }

    std::map<std::string, std::string> replace_dict;
    replace_dict["STRUCT_FIELDS_STR"] = struct_fields_str;
    replace_dict["ALIAS"] = cfile->alias;
    return replace_with_val_dict(CGEN_STRUCT_STR, replace_dict);
}


std::string cgen_get_ctypename(field_type ftype){
    switch (type){
        case field_type::intf:
            return "intf"; break;
        case field_type::fltf:
            return "fltf"; break;
        case field_type::strf:
            return "strf"; break;
        case field_type::arrintf:
            return "arrintf"; break;
        case field_type::arrfltf:
            return "arrfltf"; break;
        case field_type::arrstrf:
            return "arrstrf"; break;
        default: break;
    }
}

#define CGEN_GET_FIELD_TYPE_FUNC_FIELD_STR \
"    if(strcmp(namebuff, \"${FIELDNAME}\") == 0)\n"\
"        return ${CTYPENAME};\n"

void cgen_get_field_type_func_field_str(conf_field* cfield){
    if(type == field_type::intf ||
            type == field_type::fltf ||
            type == field_type::strf ||
            type == field_type::arrintf ||
            type == field_type::arrfltf ||
            type == field_type::arrstrf)
    {
        std::map<std::string, std::string> replace_dict;
        replace_dict["FIELDNAME"] = cfield->field_name;
        replace_dict["CTYPENAME"] = cgen_get_ctypename(cfield->type);
        return replace_with_val_dict(CGEN_GET_FIELD_TYPE_FUNC_FIELD_STR, 
                replace_dict);
    }
}

#define CGEN_GET_FIELD_TYPE_FUNC_STR \
"    field_type ${ALIAS}_get_field_type(char* namebuff){\n"\
"${GET_FIELD_TYPE_STR}"
"    printf(\"Invalid field name!\\n\%s\\n\", namebuff);\n"\
"    exit(EXIT_FAILURE);
"}\n"

std::string cgen_get_field_type_func_str(conf_file_out& cfile){
    fields_get_type_str = "";
    for(auot iter = cfile->file_fields.begin();
            iter != cfile->file_fields.end();
            ++iter)
    {
        fields_get_type_str += cgen_get_field_type_func_field_str(&*iter);
    }
    std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = cfile->alias;
    replace_dict["GET_FIELD_TYPE_STR"] = fields_get_type_str;
    return replace_with_val_dict(CGEN_GET_FIELD_TYPE_FUNC_STR, replace_dict);
}

#define CGEN_PATH_STR \
"static_const_char ${ALIAS}_path[] = \"${PATH}\";\n"

std::string cgen_get_path_str(conf_file_out* cfile){
     std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = cfile->alias;
    replace_dict["PATH"] = cfile->conf_full_path;

    return replace_with_val_dict(CGEN_PATH_STR, replace_dict);
}

std::string cgen_get_parse_lines(conf_field cfield, std::string alias){
    if(cfield.type != field_type::title){
        outs << INDENT_1 << \
            "if(strcmp(namebuff, \"" << cfield.field_name << \
                "\") == 0){\n";

        switch(cfield.type){
            case field_type::intf:
                outs << INDENT_2 << alias << '.' << cfield.field_name \
                    << "= atoi(valbuff);\n";
                break;
            case field_type::fltf:
                outs << INDENT_2 << alias << '.' << cfield.field_name \
                    << "= atof(valbuff);\n";
                break;
            case field_type::strf:
                outs << INDENT_2 << "int l = strlen(valbuff);\n"\
                    << INDENT_2 << alias << '.' << cfield.field_name \
                        << " = malloc(l + 1);\n" \
                    << INDENT_2 << "strcpy(" << alias << '.' \
                        << cfield.field_name << ", valbuff);\n"\
                        << INDENT_2 << alias << '.' << cfield.field_name \
                        << "[l] = '\\0';\n";
                break;
            case field_type::arrintf:
                outs << INDENT_2 << "int tmp_buff[128];\n"\
                    << INDENT_2 << "int tmp_index = 0;\n"\
                    << INDENT_2 << "char tmp_chr_buff[128];\n"\
                    << INDENT_2 << "int tmp_chr_index = 0;\n"\
                    << INDENT_2 << "int in_num = 0;\n"\
                    << INDENT_2 << "for(int i=0; i<strlen(valbuff)+1; ++i){\n"\
                    << INDENT_3 << "if(valbuff[i] != ' ' && \n"\
                    << INDENT_4 << "valbuff[i] != '\\t' && \n"\
                    << INDENT_4 << "valbuff[i] != '\\n' && \n"\
                    << INDENT_4 << "valbuff[i] != '\\0' && \n"\
                    << INDENT_4 << "valbuff[i] != '-' && \n"\
                    << INDENT_4 << "!(valbuff[i] >= '0' && valbuff[i]<='9'))\n"\
                    << INDENT_3 << "{\n"\
                    << INDENT_4 << "msg_and_exit(\"Bad line!\",line,linen);\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_3 << "if(in_num){\n"\
                    << INDENT_4 << "if(valbuff[i] == '-'){\n"\
                    << INDENT_5 << "msg_and_exit(\"bad line!\",line,linen);\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_4 << "if(valbuff[i]>='0'&&valbuff[i]<='9'){\n"\
                    << INDENT_5 << "app_and_incr(tmp_chr_buff,"
                        << "&tmp_chr_index, valbuff[i]);\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_4 << "else{\n"\
                    << INDENT_5 << "tmp_buff[tmp_index] = "\
                        << "atoi(tmp_chr_buff);\n"\
                    << INDENT_5 << "tmp_chr_index = 0;\n"\
                    << INDENT_5 << "tmp_index++;\n"\
                    << INDENT_5 << "in_num = 0;\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_3 << "else{\n"\
                    << INDENT_4 << "if(valbuff[i]>='0'&&valbuff[i]<='9'\n"\
                    << INDENT_5 << "|| valbuff[i] == '-'){\n"\
                    << INDENT_5 << "app_and_incr(tmp_chr_buff,"
                        << "&tmp_chr_index, valbuff[i]);\n"\
                    << INDENT_5 << "in_num = 1;\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_2 << "}\n"\
                    << INDENT_2 << alias << "." << cfield.field_name\
                        << " = malloc(sizeof(int) * tmp_index);\n"\
                    << INDENT_2 << "for(int i = 0; i < tmp_index; i++){\n"\
                    << INDENT_3 << alias << "." << cfield.field_name\
                        << "[i] = tmp_buff[i];\n"\
                    << INDENT_2 << "}\n"\
                    << INDENT_2 << alias << "." << cfield.field_name\
                        << "_nelements = tmp_index;\n";
                break;
            case field_type::arrfltf:
                outs << INDENT_2 << "float tmp_buff[128];\n"\
                    << INDENT_2 << "int tmp_index = 0;\n"\
                    << INDENT_2 << "char tmp_chr_buff[128];\n"\
                    << INDENT_2 << "int tmp_chr_index = 0;\n"\
                    << INDENT_2 << "int in_num = 0;\n"\
                    << INDENT_2 << "int after_point = 0;\n"\
                    << INDENT_2 << "for(int i=0; i<strlen(valbuff)+1; ++i){\n"\
                    << INDENT_3 << "if(valbuff[i] != ' ' && \n"\
                    << INDENT_4 << "valbuff[i] != '\\t' && \n"\
                    << INDENT_4 << "valbuff[i] != '\\n' && \n"\
                    << INDENT_4 << "valbuff[i] != '\\0' && \n"\
                    << INDENT_4 << "valbuff[i] != '.' && \n"\
                    << INDENT_4 << "valbuff[i] != '-' && \n"\
                    << INDENT_4 << "!(valbuff[i] >= '0'&&valbuff[i]<='9'))\n"\
                    << INDENT_3 << "{\n"\
                    << INDENT_4 << "msg_and_exit(\"Bad line!\",line,linen);\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_3 << "if(in_num){\n"\
                    << INDENT_4 << "if(valbuff[i] == '-'){\n"\
                    << INDENT_5 << "msg_and_exit(\"bad line!\",line,linen);\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_4 << "if(valbuff[i]>='0'&&valbuff[i]<='9' ||"\
                        <<  "valbuff[i] == '.'){\n"\
                    << INDENT_5 << "app_and_incr(tmp_chr_buff,"\
                        << "&tmp_chr_index, valbuff[i]);\n"\
                    << INDENT_5 << "if(valbuff[i] == '.'){\n"\
                    << INDENT_6 << "if(after_point){\n"\
                    << INDENT_7 << "msg_and_exit(\"Bad line!\",line,linen);\n"\
                    << INDENT_6 << "}\n"\
                    << INDENT_6 << "else{\n"\
                    << INDENT_7 << "after_point=1;\n"\
                    << INDENT_6 << "}\n"\
                    << INDENT_5 << "}\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_4 << "else{\n"\
                    << INDENT_5 << "tmp_buff[tmp_index] = "\
                        << "atof(tmp_chr_buff);\n"\
                    << INDENT_5 << "tmp_chr_index = 0;\n"\
                    << INDENT_5 << "tmp_index++;\n"\
                    << INDENT_5 << "in_num = 0;\n"\
                    << INDENT_2 << "after_point = 0;\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_3 << "else{\n"\
                    << INDENT_4 << "if(valbuff[i]>='0'&&valbuff[i]<='9' ||"\
                        <<  "valbuff[i] == '.' || valbuff[i] == '-'){\n"\
                    << INDENT_5 << "app_and_incr(tmp_chr_buff,"
                        << "&tmp_chr_index, valbuff[i]);\n"\
                    << INDENT_5 << "if(valbuff[i] == '.'){\n"\
                    << INDENT_6 << "if(after_point){\n"\
                    << INDENT_7 << "msg_and_exit(\"Bad line!\",line,linen);\n"\
                    << INDENT_6 << "}\n"\
                    << INDENT_6 << "else{\n"\
                    << INDENT_7 << "after_point=1;\n"\
                    << INDENT_6 << "}\n"\
                    << INDENT_5 << "}\n"\
                    << INDENT_5 << "in_num = 1;\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_2 << "}\n"\
                    << INDENT_2 << alias << "." << cfield.field_name\
                        << " = malloc(sizeof(int) * tmp_index);\n"\
                    << INDENT_2 << "for(int i = 0; i < tmp_index; i++){\n"\
                    << INDENT_3 << alias << "." << cfield.field_name\
                        << "[i] = tmp_buff[i];\n"\
                    << INDENT_2 << "}\n"\
                    << INDENT_2 << alias << "." << cfield.field_name\
                        << "_nelements = tmp_index;\n";
                break;
            case field_type::arrstrf:
                outs << INDENT_2 << "char* tmp_buff[128];\n"\
                    << INDENT_2 << "int tmp_index = 0;\n"\
                    << INDENT_2 << "char tmp_chr_buff[2048];\n"\
                    << INDENT_2 << "int tmp_chr_index = 0;\n"\
                    << INDENT_2 << "int escaped = 0;\n"\
                    << INDENT_2 << "int in_str = 0;\n"\
                    << INDENT_2 << "for(int i=0; i<strlen(valbuff)+1; ++i){\n"\
                    << INDENT_3 << "if(in_str){\n"\
                    << INDENT_4 << "if(!escaped){\n"\
                    << INDENT_5 << "if(valbuff[i] == '\\\"'){\n"\
                    << INDENT_6 << "in_str = 0;\n"\
                    << INDENT_6 << "tmp_buff[tmp_index] = "\
                        << "malloc(sizeof(char)*(tmp_chr_index + 1));\n"\
                    << INDENT_6 << "strcpy(tmp_buff[tmp_index], "\
                        << "tmp_chr_buff);\n"\
                    << INDENT_6 << "tmp_chr_index = 0;\n"\
                    << INDENT_6 << "tmp_index++;\n"\
                    << INDENT_5 << "}\n"\
                    << INDENT_5 << "else if(valbuff[i] != '\\\\'){\n"\
                    << INDENT_6 << "app_and_incr(tmp_chr_buff,"
                        << "&tmp_chr_index, valbuff[i]);\n"\
                    << INDENT_5 << "}\n"\
                    << INDENT_5 << "else{\n"\
                    << INDENT_6 << "escaped = 1;\n"\
                    << INDENT_5 << "}\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_4 << "else{\n"\
                    << INDENT_5 << "app_and_incr_escaped(tmp_chr_buff,"
                        << "&tmp_chr_index, valbuff[i]);\n"\
                    << INDENT_5 << "escaped = 0;\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_3 << "else{\n"\
                    << INDENT_4 << "if(valbuff[i] != ' ' &&\n"\
                    << INDENT_5 << "valbuff[i] != '\\t' &&\n"\
                    << INDENT_5 << "valbuff[i] != '\\n' &&\n"\
                    << INDENT_5 << "valbuff[i] != '\\0' &&\n"\
                    << INDENT_5 << "valbuff[i] != '\\\"'\n"\
                    << INDENT_5 << ")\n"\
                    << INDENT_4 << "{\n"\
                    << INDENT_5 << "msg_and_exit(\"Bad line!\",line,linen);\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_4 << "else if(valbuff[i] == '\\\"'){\n"\
                    << INDENT_5 << "in_str = 1;\n"\
                    << INDENT_4 << "}\n"\
                    << INDENT_3 << "}\n"\
                    << INDENT_2 << "}\n"\
                    << INDENT_2 << "if(in_str){\n"\
                    << INDENT_3 << "msg_and_exit(\"Bad line!\",line,linen);\n"\
                    << INDENT_2 << "}\n"\
                    << INDENT_2 << alias << "." << cfield.field_name\
                        << " = malloc(sizeof(char*) * tmp_index);\n"\
                    << INDENT_2 << "for(int i = 0; i < tmp_index; i++){\n"\
                    << INDENT_3 << alias << "." << cfield.field_name\
                        << "[i] = malloc(strlen(tmp_buff[i] + 1));\n"\
                    << INDENT_3 << "strcpy("\
                        << alias << "." << cfield.field_name << "[i], "\
                        << "tmp_buff[i]);\n"\
                    << INDENT_3 << "free(tmp_buff[i]);\n"\
                    << INDENT_2 << "}\n"\
                    << INDENT_2 << alias << "." << cfield.field_name\
                        << "_nelements = tmp_index;\n";
                break;
        }
        outs << INDENT_1 << "}\n";
    }
}

std::string cgen_header(conf_file_out* cfile){
    std::string retval = "";
       if(cfile->is_managed){
        outs << "void load_" << alias << "_param(char* namebuff,\n"\
            << INDENT_1 << "char* valbuff,\n"\
            << INDENT_1 << "char* line,\n"\
            << INDENT_1 << "int linen)\n"\
            << "{\n";
        for(auto field_iter = file_fields.begin();
                field_iter != file_fields.end();
                ++field_iter)
        {
            get_parse_lines(*field_iter, alias, outs);
        }
        outs << "}\n";

        outs << "int " << alias << "_load_from_path(const char* path){\n";
        outs << _LOAD_FROM_PATH_STR_A << alias << _LOAD_FROM_PATH_STR_B;
    }
}


void generate_outputs(std::vector<conf_file_out> conf_files,
        std::string out_fname)
{

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


}
