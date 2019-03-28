std::string replace_all_occurences(
    std::string& s,
    std::string& toReplace,
    std::string& replaceWith)
{
    std::string retval = s;
    size_t pos;
    pos = s.find(toReplace);
    while(pos != std::string::npos){
        retval = s.replace(pos, toReplace.length(), replaceWith);
        pos = s.find(toReplace, pos + toReplace.size());
    };

    return retval;
}

std::string replace_with_val_dict(const std::string& s,
        std::map<std::string, std::string>& val_dict)
{
    std::string retval = s;
    for(auto iter = val_dict.begin(); iter != val_dict.end(); ++iter)
    {
        std::string to_find = "${" + iter->first + "}";
        retval = replace_all_occurences(retval, to_find, iter->second);
    }

    return retval;
}

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
