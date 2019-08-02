/*
 * Functions are take from aff3ct-main
 */

/* 
 * File:   aff3ct-utils.h
 * Author: simon
 *
 * Created on August 2, 2019, 8:57 PM
 */

#ifndef AFF3CT_UTILS_H
#define AFF3CT_UTILS_H

#include <aff3ct.hpp>
#include <list>

#include "Tools/types.h"
#include "Tools/version.h"
#include "Tools/Arguments/Argument_handler.hpp"
#include "Tools/Display/rang_format/rang_format.h"
#include "Tools/system_functions.h"

#include "Launcher/Launcher.hpp"
#include "Factory/Launcher/Launcher.hpp"


namespace aff3ct {
    namespace utils {
        void print_version();
        
        int read_arguments(const int argc, 
                const char** argv, 
                factory::Launcher::parameters &params);
    } //namespace utils 
} //namespace aff3ct
#endif /* AFF3CT_UTILS_H */

