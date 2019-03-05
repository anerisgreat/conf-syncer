#define FIELD_NAME_RE "[a-zA-Z_]+[a-zA-Z0-9\\-_]*"

#define INT_RE "\\-?[0-9]+"
#define FLT_RE "\\-?[0-9]*\\.[0-9]+"
#define STR_RE "\".*\""

#define FIELD_TITLE_RE "^\\h*\\[([^\"\\[\\]]+)\\]\\h*\\n"
#define FIELD_NOVAL_RE "^\\h*(" FIELD_NAME_RE ")\\h*\\n"

#define FIELD_INT_RE "^\\h*(" FIELD_NAME_RE ")\\h*=\\h*(" INT_RE ")\\h*\\n"
#define FIELD_FLT_RE "^\\h*(" FIELD_NAME_RE ")\\h*=\\h*(" FLT_RE ")\\h*\\n"
#define FIELD_STR_RE "^\\h*(" FIELD_NAME_RE ")\\h*=\\h*(" STR_RE ")\\h*\\n"

#define FIELD_INT_ARR_RE "^\\h*(" FIELD_NAME_RE ")\\h*="\
                "\\h*\[(" INT_RE "(?>\\h+" INT_RE ")*)\\]\\h*\\n"
#define FIELD_FLT_ARR_RE "^\\h*(" FIELD_NAME_RE ")\\h*="\
                "\\h*\[(" FLT_RE "(?>\\h+" FLT_RE ")*)\\]\\h*\\n"
#define FIELD_STR_ARR_RE "^\\h*(" FIELD_NAME_RE ")\\h*="\
                "\\h*\[(" STR_RE "(?>\\h+" STR_RE ")*)\\]\\h*\\n"

#define EMPTY_LINE_RE "^\\s*\n"

#include <regex.h>

static regex_t comp_field_int_re;
static regex_t comp_field_flt_re;
static regex_t comp_field_str_re;
static regex_t comp_field_int_arr_re;
static regex_t comp_field_flt_arr_re;
static regex_t comp_field_str_arr_re;
static regex_t comp_field_title_re;
static regex_t comp_field_noval_re;
static regex_t comp_empty_line_re;

int compile_regex_fields(){
    return (!(
            (regcomp(&comp_field_int_re, FIELD_INT_RE, 0))
            /*(regcomp(&comp_field_int_re, FIELD_INT_RE, 0)) ||
            (regcomp(&comp_field_flt_re, FIELD_FLT_RE, 0)) ||
            (regcomp(&comp_field_str_re, FIELD_STR_RE, 0)) ||
            (regcomp(&comp_field_int_arr_re, FIELD_INT_ARR_RE, 0)) ||
            (regcomp(&comp_field_flt_arr_re, FIELD_FLT_ARR_RE, 0)) ||
            (regcomp(&comp_field_str_arr_re, FIELD_STR_ARR_RE, 0)) ||
            (regcomp(&comp_field_title_re, FIELD_TITLE_RE, 0)) ||
            (regcomp(&comp_field_noval_re, FIELD_NOVAL_RE, 0)) ||
            (regcomp(&comp_empty_line_re, EMPTY_LINE_RE, 0))*/
        ));

}
