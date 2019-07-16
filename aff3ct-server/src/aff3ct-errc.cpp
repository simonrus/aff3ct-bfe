/* 
 * File:   aff3ct-errc.cpp
 * Author: sergei semenov (sergey.semionov@gmail.com)
 *
 * errc codes
 */

# include <aff3ct-errc.h>

char g_log_buffer[LOG_BUF_SIZE];

char* getLastLogEntry()
{
    return g_log_buffer;
}

namespace { // anonymous namespace
  
struct Aff3ctErrCategory : std::error_category
{
  const char* name() const noexcept override;
  std::string message(int ev) const override;
};
  
const char* Aff3ctErrCategory::name() const noexcept
{
  return "aff3ct-server errors";
}
  
std::string Aff3ctErrCategory::message(int ev) const
{
  switch (static_cast<Aff3ctErrc>(ev))
  {
  case Aff3ctErrc::ParsingError:
    return "General parsing error";
   
  default:
    return "(OK)";
  }
}
  
const Aff3ctErrCategory theAff3ctErrCategory {};
  
} // anonymous namespace
 
std::error_code make_error_code(Aff3ctErrc e)
{
  return {static_cast<int>(e), theAff3ctErrCategory};
}