#pragma once
#include "config.h"

namespace BeepBoop
{
    namespace App
    {
        const Config &config();
        bool run(const Config &config);
    }
}
