#define PYGEN_GET_PY_INT_FIELD_STR \
"        elif(isintf and fintm.group(1) == '${FIELDNAME}'):\n"\
"            retval.${FIELDNAME} = int(fintm.group(2))\n"\

#define PYGEN_GET_PY_FLT_FIELD_STR \
"        elif(isfltf and ffltm.group(1) == '${FIELDNAME}'):\n"\
"            retval.${FIELDNAME} = float(ffltm.group(2))\n"

#define PYGEN_GET_PY_STR_FIELD_STR \
"        elif(isstrf and fstrm.group(1) == '${FIELDNAME}'):\n"\
"            retval.${FIELDNAME} = fstrm.group(2)[1:len(fstrm.group(2)) - 1].decode('string-escape')\n"

#define PYGEN_GET_PY_ARR_INT_FIELD_STR \
"        elif(isarrintf and farrintm.group(1) == '${FIELDNAME}'):\n"\
"            retval.${FIELDNAME} = [int(match) for match in re.compile(r'\\-?[0-9]+').findall(farrintm.group(2))]\n"

#define PYGEN_GET_PY_ARR_FLT_FIELD_STR \
"        elif(isarrfltf and farrfltm.group(1) == '${FIELDNAME}'):\n"\
"            retval.${FIELDNAME} = [float(match) for match in re.compile(r'\\-?[0-9]*\\.[0-9]+').findall(farrfltm.group(2))]\n"

#define PYGEN_GET_PY_ARR_STR_FIELD_STR \
"        elif(isarrstrf and farrstrm.group(1) == '${FIELDNAME}'):\n"\
"            retval.${FIELDNAME} = [match[1:len(match)-1].decode('string-escape') for match in re.compile(r'\\\"(?:[^\\\"\\\\]|\\\\.)*\\\"').findall(farrstrm.group(2))]\n"

std::string pygen_get_py_field_str(conf_field* cfield){
    std::map<std::string, std::string> replace_dict;
    replace_dict["FIELDNAME"] = cfield->field_name;

    if(cfield->type != field_type::title){
        switch (cfield->type){
            case field_type::intf:
                return replace_with_val_dict(PYGEN_GET_PY_INT_FIELD_STR, 
                        replace_dict); break;
            case field_type::fltf:
                return replace_with_val_dict(PYGEN_GET_PY_FLT_FIELD_STR, 
                        replace_dict); break;
            case field_type::strf:
                return replace_with_val_dict(PYGEN_GET_PY_STR_FIELD_STR, 
                        replace_dict); break;
            case field_type::arrintf:
                return replace_with_val_dict(PYGEN_GET_PY_ARR_INT_FIELD_STR, 
                        replace_dict); break;
            case field_type::arrfltf:
                return replace_with_val_dict(PYGEN_GET_PY_ARR_FLT_FIELD_STR, 
                        replace_dict); break;
            case field_type::arrstrf:
                return replace_with_val_dict(PYGEN_GET_PY_ARR_STR_FIELD_STR, 
                        replace_dict); break;
            default: return "";
        }
    }
}

#define PYGEN_PATH_STR \
"${ALIAS}_path = \'${PATH}\'\n"

std::string pygen_get_py_path_str(conf_file_out* cfile){
    std::map<std::string, std::string> replace_dict;
    replace_dict["ALIAS"] = cfile->alias;
    replace_dict["PATH"] = cfile->conf_full_path;
    return replace_with_val_dict(PYGEN_PATH_STR, replace_dict);
}

#define PYGEN_LOAD_FUNC_STR \
"def ${ALIAS}_load_from_path(path):\n"\
"    fread = open(path, 'r')\n"\
"    retval = confobj()\n"\
"    linestr = fread.readline()\n"\
"    linen = 0\n"\
"    while(linestr != ''):\n"\
"        fintm = field_int_re.match(linestr)\n"\
"        ffltm = field_flt_re.match(linestr)\n"\
"        fstrm = field_str_re.match(linestr)\n"\
"        farrintm = field_int_arr_re.match(linestr)\n"\
"        farrfltm = field_flt_arr_re.match(linestr)\n"\
"        farrstrm = field_str_arr_re.match(linestr)\n"\
"        femptym = empty_line_re.match(linestr)\n"\
"        ftitlem = field_title_re.match(linestr)\n"\
"\n"\
"        isintf = fintm != None\n"\
"        isfltf = ffltm != None\n"\
"        isstrf = fstrm != None\n"\
"        isarrintf = farrintm != None\n"\
"        isarrfltf = farrfltm != None\n"\
"        isarrstrf = farrstrm != None\n"\
"        isemptyf = femptym != None\n"\
"        istitlef = ftitlem != None\n"\
"\n"\
"        if(isemptyf or istitlef):\n"\
"           pass\n"\
"${GET_PY_FIELD_STR}"\
"        else:\n"\
"            raise ValueError('Bad line one line #' + \n"\
"                str(linen) + ': ' + linestr)\n"\
"        linen = linen + 1\n"\
"        linestr = fread.readline()\n"\
"    return retval\n"\
"\n"

std::string pygen_get_py_file_str(conf_file_out* cfile){
    std::string retval = pygen_get_py_path_str(cfile);
    if(cfile->is_managed){
        std::map<std::string, std::string> replace_dict;
        replace_dict["ALIAS"] = cfile->alias;
        std::string field_str = "";
        for(auto iter = cfile->file_fields.begin();
                iter != cfile->file_fields.end();
                ++iter)
        {
            field_str += pygen_get_py_field_str(&*iter);
        }
        replace_dict["GET_PY_FIELD_STR"] = field_str;
        retval += replace_with_val_dict(PYGEN_LOAD_FUNC_STR, replace_dict);
    }

    return retval;
}


#define PYGEN_GLOBAL_HEADER \
"import re\n"\
"import sys\n"\
"\n"\
"field_int_re = re.compile(r'^[ \\t]*([a-zA-Z_]+[a-zA-Z0-9\\-_]*)[ \\t]*=[ \\t]*(\\-?[0-9]+)[ \\t]*$')\n"\
"field_flt_re = re.compile(r'^[ \\t]*([a-zA-Z_]+[a-zA-Z0-9\\-_]*)[ \\t]*=[ \\t]*(\\-?[0-9]*\\.[0-9]+)[ \\t]*$')\n"\
"field_str_re = re.compile(r'^[ \\t]*([a-zA-Z_]+[a-zA-Z0-9\\-_]*)[ \\t]*=[ \\t]*(\\\"(?:[^\\\"\\\\]|\\\\.)*\\\")[ \\t]*$')\n"\
"field_int_arr_re = re.compile(r'^[ \\t]*([a-zA-Z_]+[a-zA-Z0-9\\-_]*)[ \\t]*=[ \\t]*\\[([ \\t]*\\-?[0-9]+(?:[ \\t]+\\-?[0-9]+)*)[ \\t]*\\][ \\t]*$')\n"\
"field_flt_arr_re = re.compile(r'^[ \\t]*([a-zA-Z_]+[a-zA-Z0-9\\-_]*)[ \\t]*=[ \\t]*\\[[ \\t]*(\\-?[0-9]*\\.[0-9]+(?:[ \\t]+\\-?[0-9]*\\.[0-9]+)*)[ \\t]*\\][ \\t]*$')\n"\
"field_str_arr_re = re.compile(r'^[ \\t]*([a-zA-Z_]+[a-zA-Z0-9\\-_]*)[ \\t]*=[ \\t]*\\[[ \\t]*(\\\"(?:[^\\\"\\\\]|\\\\.)*\\\"(?:[ \\t]+\\\"(?:[^\\\"\\\\]|\\\\.)*\\\")*)[ \\t]*\\][ \\t]*$')\n"\
"empty_line_re = re.compile(r'^[ \\t]*$')\n"\
"field_title_re = re.compile(r'^[ \\t]*\\[([^\\[\\]]+)\\][ \\t]*$')\n"\
"\n"\
"class confobj(object):\n"\
"    pass\n"\
"\n"

void pygen(std::vector<conf_file_out> conf_files, std::string out_py_fname){
    std::ofstream pyoutstream;
    pyoutstream.open(out_py_fname);

    pyoutstream << PYGEN_GLOBAL_HEADER;

    for(auto file_iter = conf_files.begin();
            file_iter != conf_files.end();
            ++file_iter)
    {
        pyoutstream << pygen_get_py_file_str(&*file_iter);
    }

    pyoutstream.close();
}
