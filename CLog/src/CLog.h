#pragma once
#include <string>
#include <map>
#include <iostream>
#include "Global.h"
#ifdef CLOG_PLATFORM_WINDOWS
	#include <Windows.h>
	#define SETCONSOLECOLOR(COLOR) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR)
#else 
	#define  SETCONSOLECOLOR(COLOR) 
#endif // CLOG_PLATFORM_WINDOWS

/// <summary>
/// CLog 用于日志记录，日志的等级目前分为三个，正常，警告和错误。
/// 调用Log成员函数，以用来对日志进行记录
/// </summary>
namespace CL {
	class CLOG_API CLog
	{
	public:
		/// <summary>
		/// 日志等级
		/// </summary>
		enum  LogLevel
		{
			Normal,
			Notice,
			Warning,
			Error
		};
	public:
		/// <summary>
		/// 日志系统的主要调用部分，用于记录日志
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="level">日志等级，用来调整记录的设置</param>
		/// <param name="format">日志的记录文本，占位符参照c#的{0}，{1}形式</param>
		/// <param name="...args">需要记录的数据</param>
		template<typename... Args>
		void Log(LogLevel level, const char* format, Args... args);
		/// <summary>
		/// 获取系统时间---字符串式
		/// </summary>
		/// <returns></returns>
		static std::string& Time() noexcept;
	private:
		/// <summary>
		/// 用于可变参的解包，使用了c++17的constexpr if 所以可以看情况来改
		/// </summary>
		/// <typeparam name="_T"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="value"></param>
		/// <param name="...args"></param>
		template<typename _T, typename ...Args>
		void MutiParamToString(_T value, Args ...args);
	private:
		/// <summary>
		/// 参数包的映射，unsigned int 指明在第几个，与format中数字对应
		/// </summary>
		std::map<unsigned int, std::string> pMap;
		/// <summary>
		/// 用于存储参数位置的数据
		/// </summary>
		unsigned int index = 0;
	};

	template<typename ...Args>
	inline void CLog::Log(LogLevel level, const char* format, Args ...args)
	{
		MutiParamToString(args...);
		std::string str;
		//虽然不怎么追求性能但还是尽可能的减少内存的再分配
		str.resize(sizeof(format));
		str += Time() + " ";
		bool pairStatus = false;
		std::string stValue;
		//此处开始进行文本的填充
		for (; *format != '\0'; ++format) {

			if (*format == '{') {
				if (pairStatus) {
					//如果已经进入了匹配模式，那么就表明之前匹配的无效
					//并且将另存的字符加入到主字符串中,清除另存字符串
					str += stValue;
					stValue.clear();
				}
				else
				{
					//进入匹配状态
					pairStatus = true;
				}
				continue;
			}
			if (*format == '}') {
				if (pairStatus) {
					//如果已经进入了匹配模式，那么这就是终点。
					//退出匹配模式
					pairStatus = false;

					int va = -1;
					try {
						va = std::stoi(stValue);
					}
					catch (const std::invalid_argument& e) {
						std::cout << stValue << " is not a valid nums" << std::endl;
					}
					catch (const std::out_of_range& e) {
						std::cout << stValue << " is out of range" << std::endl;
					}
					if (va != -1)
						str += pMap[va];
					stValue.clear();
				}
				else {
					//这就是一个无效终点。
					//暂时不做任何事
				}
				continue;
			}

			if (pairStatus) {
				//进入匹配模式，在{}之间的字符将单独存储
				stValue += *format;
			}
			else {
				//不在匹配模式就将字符存储在主字符串中
				str += *format;
			}
		}
		switch (level)
		{
		case CL::CLog::Normal:
			// 输出白色文本
			SETCONSOLECOLOR(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case CL::CLog::Notice:
			// 输出绿色文本
			SETCONSOLECOLOR(FOREGROUND_GREEN);
			break;
		case CL::CLog::Warning:
			// 输出黄色文本
			SETCONSOLECOLOR(FOREGROUND_RED | FOREGROUND_GREEN);
			break;
		case CL::CLog::Error:
			// 输出红色文本
			SETCONSOLECOLOR(FOREGROUND_RED);
			break;
		default:
			break;
		}
		str += "\n";
		std::cout << str;
		//恢复白色文本
		SETCONSOLECOLOR(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		index = 0;
		pMap.clear();
	}
	template<typename _T, typename ...Args>
	inline void CLog::MutiParamToString(_T value, Args ...args)
	{
		std::string str;
		if constexpr (std::is_fundamental<_T>::value) {
			str = std::to_string(value);
		}
		if constexpr (std::is_same<_T, const char*>::value || std::is_same<_T, std::string>::value) {
			str = value;
		}
		pMap.insert(std::make_pair(index, str));
		if constexpr (sizeof...(args) > 0) {
			++index;
			MutiParamToString(args...);
		}
	}
}