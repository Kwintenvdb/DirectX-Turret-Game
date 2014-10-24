//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.82
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "Logger.h"
#include <ctime>
#include <io.h>
#include <fcntl.h>


__int64 Logger::m_PerformanceTimerArr[] = {0};
double Logger::m_PcFreq = 0.0;
HANDLE Logger::m_ConsoleHandle = nullptr;

Logger::ConsoleLogger* Logger::m_ConsoleLogger = nullptr;
Logger::FileLogger* Logger::m_FileLogger = nullptr;
unsigned char Logger::m_BreakBitField = LogLevel::Error;
wchar_t* Logger::m_ConvertBuffer = new wchar_t[m_ConvertBufferSize];

void Logger::Initialize()
{
	for(int i = 0; i<MAX_PERFORMANCE_TIMERS; ++i)
		m_PerformanceTimerArr[i] = -1;

	LARGE_INTEGER li;
	if(QueryPerformanceFrequency(&li))
	{
		m_PcFreq = double(li.QuadPart)/1000.0;
		//LOG ERROR
	}

#if defined(DEBUG) | defined(_DEBUG)
	AllocConsole();
	m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	m_ConsoleLogger = new ConsoleLogger();

    int hCrt = _open_osfhandle((long) m_ConsoleHandle, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
#endif
}

void Logger::Release()
{
	ReleaseLoggers();

	delete[] m_ConvertBuffer;
	m_ConvertBuffer = nullptr;
}

int Logger::StartPerformanceTimer()
{
	int counter = 0;
	while(m_PerformanceTimerArr[counter] != -1)
	{
		++counter;
		if(counter == MAX_PERFORMANCE_TIMERS)
		{
			counter = -1;
			break;
		}
	}

	if(counter>=0)
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		m_PerformanceTimerArr[counter] = li.QuadPart;
	}

	return counter;
}

double Logger::StopPerformanceTimer(int timerId)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	double diff = double(li.QuadPart - m_PerformanceTimerArr[timerId])/m_PcFreq;

	m_PerformanceTimerArr[timerId] = -1;

	return diff;
}

void Logger::StartFileLogging(const wstring& fileName)
{
	SafeDelete(m_FileLogger);

	m_FileLogger = new FileLogger(fileName);
}

void Logger::StopFileLogging()
{
	SafeDelete(m_FileLogger);
}

void Logger::LogInfo(const wstring& msg, bool includeTimeStamp)
{
	Log(LogLevel::Info, msg, includeTimeStamp);
}

void Logger::LogWarning(const wstring& msg, bool includeTimeStamp)
{
	Log(LogLevel::Warning, msg, includeTimeStamp);
}

void Logger::LogError(const wstring& msg, bool includeTimeStamp)
{
	Log(LogLevel::Error, msg, includeTimeStamp);
}

void Logger::LogFixMe(const wstring& source, bool includeTimeStamp)
{
	Log(LogLevel::FixMe, source, includeTimeStamp);
}

bool Logger::LogHResult(HRESULT hr, const wstring& origin, bool includeTimeStamp)
{
	if(FAILED(hr))
	{
		if(FACILITY_WINDOWS == HRESULT_FACILITY(hr)) 
			hr = HRESULT_CODE(hr); 

		wstringstream ss;
		if(origin.size() != 0)
		{
			ss<<">ORIGIN:\n";
			ss<<origin;
			ss<<"\n\n";
		}
		ss<<"ERROR:\n";

		TCHAR* errorMsg; 
		if(FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 
			NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR)&errorMsg, 0, NULL) != 0) 
		{ 
			ss<<errorMsg;
		} 
		else
		{
			ss<<"Could not find a description for error ";
			ss<<hr;
			ss<<".";		 
		}

		Log(LogLevel::Error, ss.str(), includeTimeStamp);
		return true;
	}

	return false;
}

void Logger::LogFormat(LogLevel level, const wchar_t* format, ...)
{
	va_list ap;

	va_start(ap, format);
	_vsnwprintf_s(&m_ConvertBuffer[0], m_ConvertBufferSize, m_ConvertBufferSize, format, ap);
	va_end(ap);
	Log(level, wstring(&m_ConvertBuffer[0]));
}

void Logger::Log(LogLevel level, const wstring& msg, bool includeTimeStamp)
{
	wstringstream stream;

	if(includeTimeStamp)
	{
		SYSTEMTIME st;
        GetSystemTime(&st);
		stream << L"[" << st.wYear << L"-" << st.wMonth << L"-" << st.wDay << L" - ";
		stream << st.wHour << L":" << st.wMinute << L":" << st.wSecond << L":" << st.wMilliseconds << L"]";
	}

	switch(level)
	{
	case LogLevel::Info:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		stream<<L"[INFO]    ";
		break;
	case LogLevel::Warning:			
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
		stream<<L"[WARNING] ";
		break;
	case LogLevel::Error:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED);
		stream<<L"[ERROR]   ";
		break;
	case LogLevel::FixMe:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
		stream<<L"[FIX-ME]   ";
		break;
	}

	stream<<msg;
	stream<<L"\n";

	//Use specific loggers to log
	if(m_ConsoleLogger)
		m_ConsoleLogger->Log(stream.str());
	if(m_FileLogger)
		m_FileLogger->Log(stream.str());

	//if error, break
	if(level == LogLevel::Error)
	{
		MessageBox(0, msg.c_str(), L"ERROR" ,0);
	}

#if _DEBUG
	if((m_BreakBitField&level) == level) __asm { int 3 };
#else
	if((m_BreakBitField&level) == level) exit(-1);
#endif

}
