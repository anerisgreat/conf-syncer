#define FIELD_NAME_RE "[a-zA-Z_]+[a-zA-Z0-9\\-_]*"

#define INT_RE "\\-?[0-9]+"
#define FLT_RE "\\-?[0-9]*\\.[0-9]+"
#define STR_RE "\"[^(?<!\\\\)(?:\\\\{2})*\\K\"]*\""

#define FIELD_TITLE_RE "^[ \t]*\\[([^\"\\[\\]]+)\\][ \t]*$"
#define FIELD_NOVAL_RE "^[ \t]*(" FIELD_NAME_RE ")[ \t]*$"

#define VAL_FIELD_START "^[ \t]*(" FIELD_NAME_RE ")[ \t]*=[ \t]*"
#define VAL_FIELD_END "[ \t]*$"
#define FIELD_INT_RE VAL_FIELD_START "(" INT_RE ")" VAL_FIELD_END
#define FIELD_FLT_RE VAL_FIELD_START "(" FLT_RE ")" VAL_FIELD_END
#define FIELD_STR_RE VAL_FIELD_START "(" STR_RE ")" VAL_FIELD_END

#define FIELD_INT_ARR_RE VAL_FIELD_START \
    "\\[(" INT_RE "(?:[ \t]+" INT_RE ")*)\\]" VAL_FIELD_END
#define FIELD_FLT_ARR_RE VAL_FIELD_START \
    "\\[(" FLT_RE "(?:[ \t]+" FLT_RE ")*)\\]" VAL_FIELD_END
#define FIELD_STR_ARR_RE VAL_FIELD_START \
    "\\[(" STR_RE "(?:[ \t]+" STR_RE ")*)\\]" VAL_FIELD_END

#define EMPTY_LINE_RE "^[ \t]*$"

#include <regex>

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
