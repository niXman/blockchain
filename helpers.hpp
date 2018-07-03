
#ifndef __blockchain__helpers_hpp
#define __blockchain__helpers_hpp

#include <cstdlib>
#include <ctime>
#include <cstring>

#include <chrono>
#include <string>
#include <limits>

/*************************************************************************************************/

inline
std::uint64_t timestamp() {
    return std::chrono::duration_cast<
            std::chrono::milliseconds
    >(std::chrono::system_clock::now().time_since_epoch()).count();
}


#define __IS_LEAP_YEAR(year) \
    (!(year % 4) && ((year % 100) || !(year % 400)))

inline
std::tm time_t_to_tm(const std::time_t t) {
    static const short spm[13] = {
         0
        ,(31)
        ,(31 + 28)
        ,(31 + 28 + 31)
        ,(31 + 28 + 31 + 30)
        ,(31 + 28 + 31 + 30 + 31)
        ,(31 + 28 + 31 + 30 + 31 + 30)
        ,(31 + 28 + 31 + 30 + 31 + 30 + 31)
        ,(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31)
        ,(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30)
        ,(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31)
        ,(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
        ,(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31)
    };

    static const int SPD = 24 * 60 * 60;

    std::tm r{};
    time_t i{};

    time_t work = t % SPD;
    r.tm_sec = work % 60;
    work /= 60;
    r.tm_min = work % 60;
    r.tm_hour = work / 60;
    work = t / SPD;
    r.tm_wday = (4 + work) % 7;

    for ( i = 1970; ; ++i ) {
        const time_t k = __IS_LEAP_YEAR(i) ? 366 : 365;
        if ( work >= k )
            work -= k;
        else
            break;
    }

    r.tm_year = i - 1900;
    r.tm_yday = work;
    r.tm_mday = 1;
    if (__IS_LEAP_YEAR(i) && (work > 58)) {
        if (work == 59)
            r.tm_mday = 2;
        work -= 1;
    }

    for (i = 11; i && (spm[i] > work); --i);
    r.tm_mon = i;
    r.tm_mday += work - spm[i];

    return r;
}

inline
std::string format_timestamp(std::uint64_t ts) {
    std::uint32_t s = ts / 1000;
    std::uint32_t ms = ts % 1000;
    const std::tm tm = time_t_to_tm(s);

    char buf[64] = "\0";
    std::snprintf(
         buf
        ,sizeof(buf)-1
        ,"%04d-%02d-%02d %02d:%02d:%02d"
        ,tm.tm_year+1900
        ,tm.tm_mon+1
        ,tm.tm_mday
        ,tm.tm_hour
        ,tm.tm_min
        ,tm.tm_sec
    );

    const std::string ms_str = std::to_string(ms);
    std::strcat(buf, ".");
    std::strcat(buf, ms_str.c_str());

    return buf;
}

/*************************************************************************************************/

#endif // __blockchain__helpers_hpp
