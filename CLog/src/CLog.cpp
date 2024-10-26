#include "CLog.h"
#include <chrono>
CL::CLog::CLog(const char* filePath) noexcept
{
    if (filePath != nullptr) {
        this->filePath = filePath;
        file.open(this->filePath, std::fstream::app | std::fstream::out);
    }      
}
CL::CLog::~CLog() noexcept
{
    file.close();
}
bool CL::CLog::SetSavePath(const char* path)
{
    if (file.is_open())
        file.close();
    this->filePath = path;
    file.open(this->filePath, std::fstream::app | std::fstream::out);
    return file.is_open();
}
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

void CL::CLog::Write(const char* str)
{
    if (!file.is_open()) {
        return;
    }
    file.write(str, strlen(str));

}
