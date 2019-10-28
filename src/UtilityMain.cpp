#include "..\include\DLogger.h"
#include "..\include\DProgress.h"
#include "..\include\DTimer.h"
#include "..\include\spdlog/spdlog.h"
#include "..\include\spdlog/sinks/stdout_color_sinks.h"
#include "DPath.h"

using namespace DUtility;
void testlog() {

	auto console = spdlog::stdout_color_mt("console");
	console->info("Welcome to spdlog!");
	console->error("Some error message with arg: {}", 1);

	auto err_logger = spdlog::stderr_color_mt("stderr");
	err_logger->error("Some error message");

	// Formatting examples
	console->warn("{0} in fuction : {1} at line :{2}", "error", __func__, __LINE__);
	console->warn("Easy padding in numbers like {:08d}", 12);
	console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
	console->info("Support for floats {:03.2f}", 1.23456);
	console->info("Positional args are {1} {0}..", "too", "supported");
	console->info("{:<30}", "left aligned");

	spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");

	// Runtime log levels
	spdlog::set_level(spdlog::level::info); // Set global log level to info
	console->debug("This message should not be displayed!");
	console->set_level(spdlog::level::trace); // Set specific logger's log level
	console->debug("This message should be displayed..");

	// Customize msg format for all loggers
	spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	console->info("This an info message with custom format");

	// Compile time log levels
	// define SPDLOG_DEBUG_ON or SPDLOG_TRACE_ON
	SPDLOG_TRACE(console, "Enabled only #ifdef SPDLOG_TRACE_ON..{} ,{}", 1, 3.23);
	SPDLOG_DEBUG(console, "Enabled only #ifdef SPDLOG_DEBUG_ON.. {} ,{}", 1, 3.23);

}

void testpath() {
	//DPath m_path("E:\\PMP 相关资料整理\\串讲\\串讲课件.pdf");
	/*DPath m_path("..\\新建文件夹");
	bool ise = m_path.exists();
	bool isd = m_path.is_directory();
	bool isf = m_path.is_file();
	int sz = m_path.file_size();
	std::string ext = m_path.extension();
	std::string name = m_path.filenamewithextension();
	std::string nameonly = m_path.filenamewithoutextension();
	std::string pp = m_path.parent_path();
	std::string cwd = m_path.getcwd();
	std::string fd = m_path.make_absolute();
	//m_path.changeExtension("txt");
	std::string str = m_path.GetPathStringWithExtensionReplaced("txt");
	std::string drive = m_path.GetDriveName();
	*/
	DPath m_path("..\\新建文件夹");
	m_path.makedir();

}

int main() {
	system("pause");
	return 0;
}