#ifndef GEN_CONF_CGEN
#define GEN_CONF_CGEN

#include "gen-conf-structs.h"
#include "gen-conf-utils.h"

std::string cgen_field_type_str(field_type ftype){
    std::string typestr;
    switch (ftype){ 
        case field_type::intf:
        case field_type::arrintf: typestr = "int"; break;
        case field_type::fltf:
        case field_type::arrfltf: typestr = "float"; break;
        case field_type::strf:
        case field_type::arrstrf: typestr = "char*"; break;
        default: break;
    }

    if(ftype == field_type::arrintf ||
        ftype == field_type::arrfltf ||
        ftype == field_type::arrstrf)
    {
        typestr += '*';
    }
    return typestr;
}

#define CGEN_STRUCT_FIELD_F \
"    ${TYPESTR} ${FIELDNAME};\n"
#define CGEN_STRUCT_FIELD_ARRF \
CGEN_STRUCT_FIELD_F \
"    size_t ${FIELDNAME}_nelements;\n"

std::string cgen_get_struct_field_str(conf_field* cfield){
    if(cfield->type == field_type::title)
        return "";
    std::map<std::string, std::string> replace_dict;
    replace_dict["FIELDNAME"] = cfield->field_name;
    replace_dict["TYPESTR"] = cgen_field_type_str(cfield->type);

    if(cfield->type==field_type::intf ||
            cfield->type==field_type::fltf ||
            cfield->type==field_type::strf){
        return replace_with_val_dict(CGEN_STRUCT_FIELD_F, replace_dict);
    }
    else{
        return replace_with_val_dict(CGEN_STRUCT_FIELD_ARRF, replace_dict);
    }
}

#define CGEN_STRUCT_STR \
"struct ${ALIAS}_struct{\n"\
"${STRUCT_FIELDS_STR}"\
"}${ALIAS};\n"

std::string cgen_get_struct_str(conf_file_out* cfile){
    std::string struct_fields_str = "";
    for(auto iter = cfile->file_fields.begin();
            iter != cfile->file_fields.end();
            ++iter)
    {
        struct_fields_str += cgen_get_struct_field_str(&*iter);
    }

    std::map<std::string, std::string> replace_dict;
    replace_dict["STRUCT_FIELDS_STR"] = struct_fields_str;
    replace_dict["ALIAS"] = cfile->alias;
    return replace_with_val_dict(CGEN_STRUCT_STR, replace_dict);
}


std::string cgen_get_ctypename(field_type ftype){
    switch (ftype){
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

std::string cgen_get_field_type_func_field_str(conf_field* cfield){
    if(cfield->type == field_type::intf ||
            cfield->type == field_type::fltf ||
            cfield->type == field_type::strf ||
            cfield->type == field_type::arrintf ||
            cfield->type == field_type::arrfltf ||
            cfield->type == field_type::arrstrf)
    {
        std::map<std::string, std::string> replace_dict;
        replace_dict["FIELDNAME"] = cfield->field_name;
        replace_dict["CTYPENAME"] = cgen_get_ctypename(cfield->type);
        return replace_with_val_dict(CGEN_GET_FIELD_TYPE_FUNC_FIELD_STR, 
                replace_dict);
    }
    return "";
}

#define CGEN_GET_FIELD_TYPE_FUNC_STR \
"    field_type ${ALIAS}_get_field_type(char* namebuff){\n"\
"${GET_FIELD_TYPE_STR}"\
"    printf(\"Invalid field name!\\n\%s\\n\", namebuff);\n"\
"    exit(EXIT_FAILURE);\n"\
"}\n"

std::string cgen_get_field_type_func_str(conf_file_out* cfile){
    std::string fields_get_type_str = "";
    for(auto iter = cfile->file_fields.begin();
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
"static const char ${ALIAS}_path[] = \"${PATH}\";\n"

std::string cgen_get_path_str(conf_file_out* cfile){
    std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = cfile->alias;
    replace_dict["PATH"] = cfile->conf_full_path;

    return replace_with_val_dict(CGEN_PATH_STR, replace_dict);
}

#define CGEN_LOAD_PARAM_INT_STR \
"    if(strcmp(namebuff, \"${FIELDNAME}\") == 0){\n"\
"        ${ALIAS}.${FIELDNAME}= atoi(valbuff);\n"\
"    }\n"

#define CGEN_LOAD_PARAM_FLT_STR \
"    if(strcmp(namebuff, \"${FIELDNAME}\") == 0){\n"\
"        ${ALIAS}.${FIELDNAME}= atof(valbuff);\n"\
"    }\n"

#define CGEN_LOAD_PARAM_STR_STR \
"    if(strcmp(namebuff, \"${FIELDNAME}\") == 0){\n"\
"        int l = strlen(valbuff);\n"\
"        ${ALIAS}.${FIELDNAME} = malloc(l + 1);\n"\
"        strcpy(${ALIAS}.${FIELDNAME}, valbuff);\n"\
"        ${ALIAS}.${FIELDNAME}[l] = '\\0';\n"\
"    }\n"

#define CGEN_LOAD_PARAM_ARR_INT_STR \
"    if(strcmp(namebuff, \"${FIELDNAME}\") == 0){\n"\
"        int tmp_buff[128];\n"\
"        int tmp_index = 0;\n"\
"        char tmp_chr_buff[128];\n"\
"        int tmp_chr_index = 0;\n"\
"        int in_num = 0;\n"\
"        for(int i=0; i<strlen(valbuff)+1; ++i){\n"\
"            if(valbuff[i] != ' ' && \n"\
"                valbuff[i] != '\\t' && \n"\
"                valbuff[i] != '\\n' && \n"\
"                valbuff[i] != '\\0' && \n"\
"                valbuff[i] != '-' && \n"\
"                !(valbuff[i] >= '0' && valbuff[i]<='9'))\n"\
"            {\n"\
"                msg_and_exit(\"Bad line!\",line,linen);\n"\
"            }\n"\
"            if(in_num){\n"\
"                if(valbuff[i] == '-'){\n"\
"                    msg_and_exit(\"Bad line!\",line,linen);\n"\
"                }\n"\
"                if(valbuff[i]>='0'&&valbuff[i]<='9'){\n"\
"                    app_and_incr(tmp_chr_buff,&tmp_chr_index, valbuff[i]);\n"\
"                }\n"\
"                else{\n"\
"                    tmp_buff[tmp_index] = atoi(tmp_chr_buff);\n"\
"                    tmp_chr_index = 0;\n"\
"                    tmp_index++;\n"\
"                    in_num = 0;\n"\
"                }\n"\
"            }\n"\
"            else{\n"\
"                if(valbuff[i]>='0'&&valbuff[i]<='9'\n"\
"                    || valbuff[i] == '-'){\n"\
"                    app_and_incr(tmp_chr_buff,&tmp_chr_index, valbuff[i]);\n"\
"                    in_num = 1;\n"\
"                }\n"\
"            }\n"\
"        }\n"\
"        ${ALIAS}.${FIELDNAME} = malloc(sizeof(int) * tmp_index);\n"\
"        for(int i = 0; i < tmp_index; i++){\n"\
"            ${ALIAS}.${FIELDNAME}[i] = tmp_buff[i];\n"\
"        }\n"\
"        ${ALIAS}.${FIELDNAME}_nelements = tmp_index;\n"\
"    }\n"

#define CGEN_LOAD_PARAM_ARR_FLT_STR \
"    if(strcmp(namebuff, \"${FIELDNAME}\") == 0){\n"\
"        float tmp_buff[128];\n"\
"        int tmp_index = 0;\n"\
"        char tmp_chr_buff[128];\n"\
"        int tmp_chr_index = 0;\n"\
"        int in_num = 0;\n"\
"        int after_point = 0;\n"\
"        for(int i=0; i<strlen(valbuff)+1; ++i){\n"\
"            if(valbuff[i] != ' ' &&\n"\
"                valbuff[i] != '\\t' &&\n"\
"                valbuff[i] != '\\n' &&\n"\
"                valbuff[i] != '\\0' &&\n"\
"                valbuff[i] != '.' &&\n"\
"                valbuff[i] != '-' &&\n"\
"                !(valbuff[i] >= '0'&&valbuff[i]<='9'))\n"\
"            {\n"\
"                msg_and_exit(\"Bad line!\",line,linen);\n"\
"            }\n"\
"            if(in_num){\n"\
"                if(valbuff[i] == '-'){\n"\
"                    msg_and_exit(\"Bad line!\",line,linen);\n"\
"                }\n"\
"                if(valbuff[i]>='0'&&valbuff[i]<='9' ||valbuff[i] == '.'){\n"\
"                    app_and_incr(tmp_chr_buff,&tmp_chr_index, valbuff[i]);\n"\
"                    if(valbuff[i] == '.'){\n"\
"                        if(after_point){\n"\
"                            msg_and_exit(\"Bad line!\",line,linen);\n"\
"                        }\n"\
"                        else{\n"\
"                            after_point=1;\n"\
"                        }\n"\
"                    }\n"\
"                }\n"\
"                else{\n"\
"                    tmp_buff[tmp_index] = atof(tmp_chr_buff);\n"\
"                    tmp_chr_index = 0;\n"\
"                    tmp_index++;\n"\
"                    in_num = 0;\n"\
"        after_point = 0;\n"\
"                }\n"\
"            }\n"\
"            else{\n"\
"                if(valbuff[i]>='0'&&valbuff[i]<='9' |\n"\
"                    valbuff[i] == '.' || valbuff[i] == '-')\n"\
"                {\n"\
"                    app_and_incr(tmp_chr_buff,&tmp_chr_index, valbuff[i]);\n"\
"                    if(valbuff[i] == '.'){\n"\
"                        if(after_point){\n"\
"                            msg_and_exit(\"Bad line!\",line,linen);\n"\
"                        }\n"\
"                        else{\n"\
"                            after_point=1;\n"\
"                        }\n"\
"                    }\n"\
"                    in_num = 1;\n"\
"                }\n"\
"            }\n"\
"        }\n"\
"        ${ALIAS}.${FIELDNAME} = malloc(sizeof(int) * tmp_index);\n"\
"        for(int i = 0; i < tmp_index; i++){\n"\
"            ${ALIAS}.${FIELDNAME}[i] = tmp_buff[i];\n"\
"        }\n"\
"        ${ALIAS}.${FIELDNAME}_nelements = tmp_index;\n"\
"    }\n"\
 
#define CGEN_LOAD_PARAM_ARR_STR_STR \
"    if(strcmp(namebuff, \"${FIELDNAME}\") == 0){\n"\
"        char* tmp_buff[128];\n"\
"        int tmp_index = 0;\n"\
"        char tmp_chr_buff[2048];\n"\
"        int tmp_chr_index = 0;\n"\
"        int escaped = 0;\n"\
"        int in_str = 0;\n"\
"        for(int i=0; i<strlen(valbuff)+1; ++i){\n"\
"            if(in_str){\n"\
"                if(!escaped){\n"\
"                    if(valbuff[i] == '\\\"'){\n"\
"                        in_str = 0;\n"\
"                        tmp_buff[tmp_index] =\n"\
"                            malloc(sizeof(char)*(tmp_chr_index + 1));\n"\
"                        strcpy(tmp_buff[tmp_index], tmp_chr_buff);\n"\
"                        tmp_chr_index = 0;\n"\
"                        tmp_index++;\n"\
"                    }\n"\
"                    else if(valbuff[i] != '\\\\'){\n"\
"                        app_and_incr(tmp_chr_buff,&tmp_chr_index,\n"\
"                            valbuff[i]);\n"\
"                    }\n"\
"                    else{\n"\
"                        escaped = 1;\n"\
"                    }\n"\
"                }\n"\
"                else{\n"\
"                    app_and_incr_escaped(tmp_chr_buff,\n"\
"                        &tmp_chr_index, valbuff[i]);\n"\
"                    escaped = 0;\n"\
"                }\n"\
"            }\n"\
"            else{\n"\
"                if(valbuff[i] != ' ' &&\n"\
"                    valbuff[i] != '\\t' &&\n"\
"                    valbuff[i] != '\\n' &&\n"\
"                    valbuff[i] != '\\0' &&\n"\
"                    valbuff[i] != '\\\"'\n"\
"                    )\n"\
"                {\n"\
"                    msg_and_exit(\"Bad line!\",line,linen);\n"\
"                }\n"\
"                else if(valbuff[i] == '\\\"'){\n"\
"                    in_str = 1;\n"\
"                }\n"\
"            }\n"\
"        }\n"\
"        if(in_str){\n"\
"            msg_and_exit(\"Bad line!\",line,linen);\n"\
"        }\n"\
"        ${ALIAS}.${FIELDNAME} = malloc(sizeof(char*) * tmp_index);\n"\
"        for(int i = 0; i < tmp_index; i++){\n"\
"            ${ALIAS}.${FIELDNAME}[i] = malloc(strlen(tmp_buff[i] + 1));\n"\
"            strcpy(${ALIAS}.${FIELDNAME}[i], tmp_buff[i]);\n"\
"            free(tmp_buff[i]);\n"\
"        }\n"\
"        ${ALIAS}.${FIELDNAME}_nelements = tmp_index;\n"\
"    }\n"

std::string cgen_get_field_load_param_str(conf_field* cfield,
        std::string alias)
{
    if(cfield->type == field_type::title)
        return "";
    std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = alias;
    replace_dict["FIELDNAME"] = cfield->field_name;

    std::string func_str;
    switch(cfield->type){
        case field_type::intf: func_str = CGEN_LOAD_PARAM_INT_STR; break;
        case field_type::fltf: func_str = CGEN_LOAD_PARAM_FLT_STR; break;
        case field_type::strf: func_str = CGEN_LOAD_PARAM_STR_STR; break;
        case field_type::arrintf: func_str = CGEN_LOAD_PARAM_ARR_INT_STR; break;
        case field_type::arrfltf: func_str = CGEN_LOAD_PARAM_ARR_FLT_STR; break;
        case field_type::arrstrf: func_str = CGEN_LOAD_PARAM_ARR_STR_STR; break;
    }

    return replace_with_val_dict(func_str, replace_dict);
}

#define CGEN_LOAD_PARAM_FUNC_STR \
"void load_${ALIAS}_param(char* namebuff, char* valbuff, char* line,\n"\
"    int linen)\n"\
"{\n"\
"${GET_FIELD_LOAD_PARAM_STR}"\
"}\n"

std::string cgen_get_load_param_func_str(conf_file_out* cfile){
    std::string field_load_param_str = "";
    std::string alias = cfile->alias;
    for(auto field_iter = cfile->file_fields.begin();
            field_iter != cfile->file_fields.end();
            ++field_iter)
    {
        field_load_param_str += cgen_get_field_load_param_str(&*field_iter,
                alias);
    }

    std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = alias;
    replace_dict["GET_FIELD_LOAD_PARAM_STR"] = field_load_param_str;
    return replace_with_val_dict(CGEN_LOAD_PARAM_FUNC_STR, replace_dict);
}


#define CGEN_LOAD_FUNC_DECL_STR \
"int ${ALIAS}_load_from_path(const char* path);\n"

std::string cgen_get_load_func_decl_str(conf_file_out* cfile){
    std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = cfile->alias;
    return replace_with_val_dict(CGEN_LOAD_FUNC_DECL_STR, replace_dict);
}


#define CGEN_LOAD_FUNC_STR \
"int ${ALIAS}_load_from_path(const char* path){\n"\
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
"                            ftype = ${ALIAS}_get_field_type(namebuff);\n"\
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
"                            load_${ALIAS}_param(namebuff, valbuff,\n"\
"                                line, linen);\n"\
"                            reader_state = post_val;\n"\
"                            break;\n"\
"                        default:\n"\
"                            if((c >= '0' && c <= '9') ||\n"\
"                                (c == '.' &&\n"\
"                                    ftype == fltf && periodf == 0))\n"\
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
"                            load_${ALIAS}_param(namebuff, valbuff,\n"\
"                                line, linen);\n"\
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
"                            load_${ALIAS}_param(namebuff,\n"\
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

std::string get_load_func_str(conf_file_out* cfile){
    std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = cfile->alias;
    return replace_with_val_dict(CGEN_LOAD_FUNC_STR, replace_dict);
}

//DEFINES
#define CGEN_HEADER_GLOBAL_STR \
"//This file was created automagically by conf-gen,\n"\
"//    for the project ${PROJNAME}\n"\
"#include <string.h>\n"\
"#ifdef __cplusplus\n"\
"extern \"C\" {\n"\
"#endif\n\n"

#define CGEN_HEADER_GLOBAL_FOOTER_STR \
"\n#ifdef __cplusplus\n"\
"}\n"\
"#endif\n"

std::string cgen_get_header_global_str(std::string projname){
    std::map<std::string, std::string> replace_dict;
    replace_dict["PROJNAME"] = projname;
    return replace_with_val_dict(CGEN_HEADER_GLOBAL_STR, replace_dict);
}

#define CGEN_SOURCE_GLOBAL_STR \
"#include <stdio.h>\n"\
"#include <stdlib.h>\n"\
"#include <${PROJNAME}.h>\n"\
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

std::string cgen_get_source_global_str(std::string projname){
    std::map<std::string, std::string> replace_dict;
    replace_dict["PROJNAME"] = projname;
    return replace_with_val_dict(CGEN_SOURCE_GLOBAL_STR, replace_dict);
}

void cgen(std::vector<conf_file_out> conf_files, std::string out_fname)
{
    std::ofstream headeroutstream;
    headeroutstream.open(out_fname + ".h");

    std::string projname = get_file_name_no_extention(out_fname);

    headeroutstream << cgen_get_header_global_str(projname);
    std::cout << cgen_get_header_global_str << std::endl;

    //Paths
    for(auto file_iter = conf_files.begin();
            file_iter != conf_files.end();
            ++file_iter)
    {
        headeroutstream << cgen_get_path_str(&*file_iter);
    }
    headeroutstream << std::endl;

    //Structs
    for(auto file_iter = conf_files.begin();
            file_iter != conf_files.end();
            ++file_iter)
    {
        if(file_iter->is_managed){
            headeroutstream << cgen_get_struct_str(&*file_iter);
            headeroutstream << cgen_get_load_func_decl_str(&*file_iter);
            headeroutstream << std::endl;
        }
    }

    headeroutstream << CGEN_HEADER_GLOBAL_FOOTER_STR;

    headeroutstream.close();

    std::ofstream srcoutstream;
    srcoutstream.open(out_fname + ".c");

    srcoutstream << cgen_get_source_global_str(projname);

    //Structs
    for(auto file_iter = conf_files.begin();
            file_iter != conf_files.end();
            ++file_iter)
    {
        if(file_iter->is_managed){
            srcoutstream << cgen_get_field_type_func_str(&*file_iter);
            srcoutstream << cgen_get_load_param_func_str(&*file_iter);
            srcoutstream << get_load_func_str(&*file_iter);

            srcoutstream << std::endl;
        }
    }

    srcoutstream.close();
}

#endif
