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
/// CLog ������־��¼����־�ĵȼ�Ŀǰ��Ϊ����������������ʹ���
/// ����Log��Ա����������������־���м�¼
/// </summary>
namespace CL {
	class CLOG_API CLog
	{
	public:
		/// <summary>
		/// ��־�ȼ�
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
		/// ��־ϵͳ����Ҫ���ò��֣����ڼ�¼��־
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="level">��־�ȼ�������������¼������</param>
		/// <param name="format">��־�ļ�¼�ı���ռλ������c#��{0}��{1}��ʽ</param>
		/// <param name="...args">��Ҫ��¼������</param>
		template<typename... Args>
		void Log(LogLevel level, const char* format, Args... args);
		/// <summary>
		/// ��ȡϵͳʱ��---�ַ���ʽ
		/// </summary>
		/// <returns></returns>
		static std::string& Time() noexcept;
	private:
		/// <summary>
		/// ���ڿɱ�εĽ����ʹ����c++17��constexpr if ���Կ��Կ��������
		/// </summary>
		/// <typeparam name="_T"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="value"></param>
		/// <param name="...args"></param>
		template<typename _T, typename ...Args>
		void MutiParamToString(_T value, Args ...args);
	private:
		/// <summary>
		/// ��������ӳ�䣬unsigned int ָ���ڵڼ�������format�����ֶ�Ӧ
		/// </summary>
		std::map<unsigned int, std::string> pMap;
		/// <summary>
		/// ���ڴ洢����λ�õ�����
		/// </summary>
		unsigned int index = 0;
	};

	template<typename ...Args>
	inline void CLog::Log(LogLevel level, const char* format, Args ...args)
	{
		MutiParamToString(args...);
		std::string str;
		//��Ȼ����ô׷�����ܵ����Ǿ����ܵļ����ڴ���ٷ���
		str.resize(sizeof(format));
		str += Time() + " ";
		bool pairStatus = false;
		std::string stValue;
		//�˴���ʼ�����ı������
		for (; *format != '\0'; ++format) {

			if (*format == '{') {
				if (pairStatus) {
					//����Ѿ�������ƥ��ģʽ����ô�ͱ���֮ǰƥ�����Ч
					//���ҽ������ַ����뵽���ַ�����,�������ַ���
					str += stValue;
					stValue.clear();
				}
				else
				{
					//����ƥ��״̬
					pairStatus = true;
				}
				continue;
			}
			if (*format == '}') {
				if (pairStatus) {
					//����Ѿ�������ƥ��ģʽ����ô������յ㡣
					//�˳�ƥ��ģʽ
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
					//�����һ����Ч�յ㡣
					//��ʱ�����κ���
				}
				continue;
			}

			if (pairStatus) {
				//����ƥ��ģʽ����{}֮����ַ��������洢
				stValue += *format;
			}
			else {
				//����ƥ��ģʽ�ͽ��ַ��洢�����ַ�����
				str += *format;
			}
		}
		switch (level)
		{
		case CL::CLog::Normal:
			// �����ɫ�ı�
			SETCONSOLECOLOR(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case CL::CLog::Notice:
			// �����ɫ�ı�
			SETCONSOLECOLOR(FOREGROUND_GREEN);
			break;
		case CL::CLog::Warning:
			// �����ɫ�ı�
			SETCONSOLECOLOR(FOREGROUND_RED | FOREGROUND_GREEN);
			break;
		case CL::CLog::Error:
			// �����ɫ�ı�
			SETCONSOLECOLOR(FOREGROUND_RED);
			break;
		default:
			break;
		}
		str += "\n";
		std::cout << str;
		//�ָ���ɫ�ı�
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