#ifndef _DLOGGER_HEADER_
#define _DLOGGER_HEADER_

#include "../include/spdlog/spdlog.h"
#include "../include/spdlog/sinks/stdout_color_sinks.h"
#include "../include/spdlog/sinks/basic_file_sink.h"
#include "../include/DUtility.h"

/*!
* \class DLog
*
* \brief  this is a simple wrap of spdlog: https://github.com/gabime/spdlog
*		   see details in github
*		   a simple wraper for only use
* \note   if NOLOG define is used, nothing will be output
* \author Damons
* \date Ê®ÔÂ 2018
*/

using namespace spdlog;
using namespace sinks;

	#ifdef NOLOG
		class DU_DLL_API DLog
		{
			public:
				DLog() {
					std::cout << " NoLog define is used , no any log will be output" << std::endl;
				}
				~DLog() {}
			public:
				void init(std::string _logger_name) {
				}
				void init(std::string _logger_name, std::string _log_file_name) {
				}
				void warn(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
				}
				void info(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
				}
				void critical(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
				}
				void debug(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
				}
				void error(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
				}
		};
	#else
		class DU_DLL_API DLog
		{
			public:
				DLog() {
				}
				~DLog() {}
			public:
				void init(std::string _logger_name) {
					m_log_handle = spdlog::stdout_color_mt(_logger_name);
				}
				void init(std::string _logger_name,std::string _log_file_name) {
					m_log_handle = spdlog::basic_logger_mt(_logger_name, _log_file_name);
				}
				///@brief warn 
				void warn(std::string _msg,std::string _function_name = "",unsigned int _line_number = 0) {
					if(_line_number)
						m_log_handle->warn("{0} in fuction : {1} at line :{2:d}", _msg, _function_name, _line_number);
					else
						m_log_handle->warn("{0}", _msg);
				}
				///@brief info 
				void info(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
					if (_line_number)
						m_log_handle->info("{0} in fuction : {1} at line :{2:d}", _msg, _function_name, _line_number);
					else
						m_log_handle->info("{0}", _msg);
				}
				///@brief critical
				void critical(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
					if (_line_number)
						m_log_handle->critical("{0} in fuction : {1} at line :{2:d}", _msg, _function_name, _line_number);
					else
						m_log_handle->critical("{0}", _msg);
				}
				///@brief debug 
				void debug(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
					if (_line_number)
						m_log_handle->debug("{0} in fuction : {1} at line :{2:d}", _msg, _function_name, _line_number);
					else
						m_log_handle->debug("{0}", _msg);
				}
				///@brief error 
				void error(std::string _msg, std::string _function_name = "", unsigned int _line_number = 0) {
					if (_line_number)
						m_log_handle->error("{0} in fuction : {1} at line :{2:d}", _msg, _function_name, _line_number);
					else
						m_log_handle->error("{0}", _msg);
				}
			protected:
				//spdlog handle
				std::shared_ptr<logger> m_log_handle;
		};
	#endif

#endif// 2018/10/25