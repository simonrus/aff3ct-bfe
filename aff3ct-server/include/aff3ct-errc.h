/* 
 * File:   aff3ct-errc.h
 * Author: sergei semenov (sergey.semionov@gmail.com)
 *
 * errc codes
 */

#ifndef AFF3CT_ERRC_H
#define AFF3CT_ERRC_H

# include <system_error>
//Include loguru headers
#include <loguru.hpp>
#include <stdio.h>
#include <sstream>

#define LOG_BUF_SIZE   256
extern char g_log_buffer[LOG_BUF_SIZE];
extern std::ostringstream g_err_stream;

inline std::ostringstream &getErrStream() {return g_err_stream;} 
inline std::ostringstream &clearErrStream() {g_err_stream.str(""); g_err_stream.clear(); } 

#define TRACELOG(LEVEL, ...) {g_log_buffer[0] = 0; \
                                        snprintf(g_log_buffer, LOG_BUF_SIZE, __VA_ARGS__); \
                                        clearErrStream(); \
                                        g_err_stream << g_log_buffer; \
                                        LOG_F(LEVEL, __VA_ARGS__); \
                                       }


char* getLastLogEntry();

enum class Aff3ctErrc
{
  // no 0
  NoError               = 0,
  ParsingError          = 1 //Parsing data error
};
 
namespace std
{
  template <>
    struct is_error_code_enum<Aff3ctErrc> : true_type {};
}
 
std::error_code make_error_code(Aff3ctErrc);
 

#endif /* AFF3CT_ERRC_H */

