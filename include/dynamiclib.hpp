/*
    Copyright 2016 Daniel Trugman

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef MP_DYNAMIC_LIB_HPP
#define MP_DYNAMIC_LIB_HPP

#include "config.hpp"

#include <dlfcn.h>

#include <string>

namespace mp
{

class dynamiclib
{
public: // C'tors & D'tors
    dynamiclib(const std::string & file, int mode = RTLD_LAZY | RTLD_LOCAL)
        : _handle(NULL)
    {
        open(file, mode);
    }

    virtual ~dynamiclib()
    {
        close();
    }

public: // Operators
    inline operator bool() const
    {
        return (NULL != _handle);
    }

public: // Methods
    inline bool close()
    {
        if (NULL == _handle)
        {
            return true;
        }

        if (dlclose(_handle) != 0)
        {
            RETURN_OR_THROW_EX(false, std::runtime_error, "Underlying dlclose failed");
        }
        
        _handle = NULL;
        return true;
    }

    inline void error(std::string & err)
    {
        char * errstr = error();
        err = (NULL != errstr) ? errstr : "Unknown underlying error or error was already read";
    }

    inline void * symbol(const std::string & name)
    {
        void * addr = NULL;

        if (NULL != _handle)
        {
            addr = dlsym(_handle, name.c_str());
        }

        return addr;
    }

private: // Methods
    inline char * error()
    {
        return dlerror();
    }

    inline bool open(const std::string & file, int mode)
    {
        _handle = dlopen(file.c_str(), mode);
        if (NULL == _handle)
        {
            RETURN_OR_THROW_EX(false, std::runtime_error, error());
        }

        return true;
    }

private: // Members
    void * _handle;
};

} // namespace mp

#endif // MP_DYNAMIC_LIB_HPP