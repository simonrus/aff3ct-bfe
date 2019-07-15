/* 
 * File:   aff3ct-errc.h
 * Author: sergei semenov (sergey.semionov@gmail.com)
 *
 * errc codes
 */

#ifndef AFF3CT_ERRC_H
#define AFF3CT_ERRC_H

# include <system_error>

enum class Aff3ctErrc
{
  // no 0
  NoError      = 0,
  ParsingError = 10, // requested airport doesn't exist
 
};
 
namespace std
{
  template <>
    struct is_error_code_enum<Aff3ctErrc> : true_type {};
}
 
std::error_code make_error_code(Aff3ctErrc);
 

#endif /* AFF3CT_ERRC_H */

