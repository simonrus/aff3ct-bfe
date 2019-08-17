/*
 * MIT License
 * 
 * Copyright (c) 2017 aff3ct
 * Copyright (c) 2019 Sergei Semenov
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

# include <aff3ct-errc.h>

std::ostringstream g_err_stream;
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

std::string Aff3ctErrCategory::message(int ev) const {
    switch (static_cast<Aff3ctErrc> (ev)) {
        case Aff3ctErrc::ParsingError:
            return "General parsing error";           
        default:
            return "(Uknown error)";
    }
}
  
const Aff3ctErrCategory theAff3ctErrCategory {};
  
} // anonymous namespace
 
std::error_code make_error_code(Aff3ctErrc e)
{
  return {static_cast<int>(e), theAff3ctErrCategory};
}