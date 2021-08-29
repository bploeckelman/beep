#pragma once
#include <cstddef>        // for integer types
#include <functional>     // for std::function

// so stupid... Beep() is a symbol in the global namespace in windows utilapiset.h
// guess we have to use something different as our primary namespace for the project
namespace BeepBoop
{

    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using ui8  = uint8_t;
    using ui16 = uint16_t;
    using ui32 = uint32_t;
    using ui64 = uint64_t;

    template<typename T>
    using Function = std::function<T>;

    using LifecycleFunction = Function<void()>;

}
