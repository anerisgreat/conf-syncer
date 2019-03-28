std::string replace_all_occurences(
    std::string& s,
    const std::string& toReplace,
    const std::string& replaceWith)
{
    std::string retval = s;
    size_t pos;
    pos = s.find(toReplace);
    while(si != std::string::npos){
        retval = s.replace(pos, toReplace.length(), replaceWith);
        pos = s.find(toReplace, pos + toReplace.size());
    };

    return retval;
}

std::string replace_with_val_dict(std::string& s,
        std::map<std::string, std::string>& val_dict)
{
    std::string retval = s;
    for(auto iter = val_dict.begin(); iter != val_dict.end(); ++iter)
    {
        std::string to_find = "${" + iter->first + "}";
        s = replace_all_occurences(retval, to_find, iter->second);
    }

    return s;
}
