#include "CLog.h"
#include <chrono>
std::string& CL::CLog::Time() noexcept
{
    static std::string timeStr;
    timeStr.clear();
    auto time = std::chrono::system_clock::now();
    auto ti = std::chrono::system_clock::to_time_t(time);
    auto tm = std::localtime(&ti);
    std::ostringstream strStream;
    strStream << "[" << 1900 + tm->tm_year << "." << 1 + tm->tm_mon << "." << tm->tm_mday << " " << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec << "]";
    timeStr = strStream.str();
    return timeStr;
}
