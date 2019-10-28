#ifndef _PATHOPERATION_HEADER_
#define _PATHOPERATION_HEADER_

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#include "..\include\DUtility.h"
#include <sys/stat.h>
#include <ctype.h>
#include <io.h>

namespace DUtility {

	
	/*!
	 * \class DPath
	 *
	 * \brief path operation
	 *
	 * \author Damons
	 * \date 十月 2018
	 */
	class DU_DLL_API DPath
	{
	public:
		enum class PathType {
			PT_Unknown,
			PT_Windows,// windows style with separator "\\"
			PT_Unix, // unix style with separator "/"
#ifdef _WIN32
			PT_Native = PT_Windows
#else
			PT_Native = PT_Unix
#endif
		};

	public:
		///@brief constructor
		DPath():m_bIsAbsolute(false),m_pathType(PathType::PT_Native){}
		///@brief constructor
		DPath(std::string _path_str, PathType _type = PathType::PT_Native) :m_originString(_path_str) {
			Parse(_type);
		}
		///@brief constructor
		DPath(const char *_path_str, PathType _type = PathType::PT_Native):m_originString(_path_str){
			Parse(_type);
		}
		///@brief constructor
		DPath(const DPath &path): m_pathType(path.m_pathType), m_path(path.m_path), m_bIsAbsolute(path.m_bIsAbsolute) {
		}
		///@brief constructor
		DPath(DPath &&path): m_pathType(path.m_pathType), m_path(std::move(path.m_path)),m_bIsAbsolute(path.m_bIsAbsolute) {
		}

		~DPath() { std::vector<std::string >().swap(m_path); }

	public:
		///@brief initial fuction
		void init(std::string _path_str, PathType _type = PathType::PT_Native) {
			m_originString = _path_str;
			Parse(_type);
		}
		///@brief initial fuction
		void init(const char *_path_str, PathType _type = PathType::PT_Native) {
			m_originString = std::string(_path_str);
			Parse(_type);
		}
	public:
		//************************************  
		// @brief : get path size not bytes size 
		//			eg. D:\\test\\test.txt returned 3
		//				..\\test returned 2
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: directory depth
		// @param : void  
		// @note  : if path is relative,only returned depth relatively
		//************************************ 
		size_t length() const { return m_path.size(); }
		//************************************  
		// @brief : is path empty
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : void  
		//************************************ 
		bool empty() const { return m_path.empty(); }
		//************************************  
		// @brief : is path a absolute path  
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: true if path is a absolute path otherwise false
		// @param : void  
		//************************************ 
		bool is_absolute() const { return m_bIsAbsolute; }
		//************************************  
		// @brief : make path absolute path  
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: absolute path
		// @param : void  
		//************************************ 
		std::string  make_absolute() const {
			if (m_bIsAbsolute)
				return m_originString;

#ifdef _WIN32
			std::wstring value = s2ws(m_originString);
			std::wstring out(MAX_PATH, '\0');
			DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH, &out[0], NULL);
			if (length == 0)
				throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));

			std::string res = ws2s(out);
			return res;
			
#else
			char temp[PATH_MAX];
			if (realpath(m_originString.c_str(), temp) == NULL)
				throw std::runtime_error("Internal error in realpath(): " + std::string(strerror(errno)));
			return path(temp);
#endif
		}
		//************************************  
		// @brief : check whether path is exist 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : void  
		//************************************ 
		bool exists() const {
			/*
			inline bool exists_test0 (const std::string& name) {
			ifstream f(name.c_str());
			return f.good();
			}

			inline bool exists_test1 (const std::string& name) {
			if (FILE *file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
			} else {
			return false;
			}
			}

			inline bool exists_test2 (const std::string& name) {
			return ( access( name.c_str(), F_OK ) != -1 );
			}

			inline bool exists_test3 (const std::string& name) {
			struct stat buffer;
			return (stat (name.c_str(), &buffer) == 0);
			}
			*/
			struct stat buffer;
			return (stat(m_originString.c_str(), &buffer) == 0);
		}
		//************************************  
		// @brief : if path is a file ,get file size  then return 0
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: file size
		// @param : void  
		//************************************ 
		size_t file_size() const {
			if (is_directory())
				return 0;
			std::ifstream ifile(m_originString);
			ifile.seekg(0, std::ios_base::end);//seek to end
			size_t sz = ifile.tellg();
			ifile.close();
			return sz;
		}
		//************************************  
		// @brief : check whether path is a directory 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : void  
		//************************************ 
		bool is_directory() const {
			struct stat sb;
			if (stat(m_originString.c_str(), &sb))
				return false;
			return S_IFDIR &sb.st_mode;
		}
		//************************************  
		// @brief : check whether path is a file 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : void  
		//************************************ 
		bool is_file() const {
			struct stat sb;
			if (stat(m_originString.c_str(), &sb))
				return false;
			return !(S_IFDIR&sb.st_mode);
		}
		//************************************  
		// @brief : get file extension 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: file extension without "."
		// @param : void  
		//************************************ 
		std::string extension() const {
			const std::string &name = filenamewithextension();
			size_t pos = name.find_last_of(".");
			if (pos == std::string::npos)
				return "";
			return name.substr(pos + 1);
		}
		//************************************  
		// @brief : get file name contain extension 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: filename with extension eg. "test.txt"
		// @param : void  
		//************************************ 
		std::string filenamewithextension() const {
			if (empty() || is_directory())
				return "";
			const std::string &last = m_path.back();
			return last;
		}
		//************************************  
		// @brief : get file name without extension 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: filename without extension eg. "test"
		// @param : void  
		//************************************ 
		std::string filenamewithoutextension() const {
			if (empty() || is_directory())
				return "";
			const std::string name = m_path.back();
			size_t pos = name.find_last_of(".");
			if (pos == std::string::npos)
				return "";
			return name.substr(0,pos);
		}
		//************************************  
		// @brief : return this path's parent path 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: parent path 
		// @param : void  
		// @note  : if path is not absolute return empty
		//************************************ 
		std::string parent_path() const {
			std::string result="";
			if (m_bIsAbsolute && m_path.size()) {
				size_t until = m_path.size() - 1;
				for (size_t i = 0; i < until; ++i) {
					if (m_pathType == PathType::PT_Windows)
						result = result + m_path[i] + "\\";
					else
						result = result + m_path[i] + "/";
				}
			}
			
			return result;
		}
		//************************************  
		// @brief : change this path's extension and reparse again 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : std::string _ext_new: new extension  
		//************************************ 
		void changeExtension(std::string _ext_new) {
			if (is_file()) {
				size_t pos = m_originString.find_last_of(".");
				if (pos == std::string::npos)
					std::runtime_error("can not find file extension string \".\"");
				m_originString = m_originString.substr(0, pos+1) + _ext_new;
				Parse(m_pathType);
			}
		}
		//************************************  
		// @brief : return a string with extension replaced with  _ext_new
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: new path string 
		// @param : std::string _ext_new  
		// @note  : does not change this path
		//************************************ 
		std::string GetPathStringWithExtensionReplaced(std::string _ext_new) {
			std::string result="";
			if (is_file()) {
				size_t pos = m_originString.find_last_of(".");
				if (pos == std::string::npos)
					std::runtime_error("can not find file extension string \".\"");
				result = m_originString.substr(0, pos + 1) + _ext_new;
			}
			return result;
		}
		//************************************  
		// @brief : return a string with filename replaced with  _fn_new
		//			eg. D:\\test\\test.txt -> replace test.txt with _fn_new
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: new path string 
		// @param : std::string _fn_new  
		// @note  : does not change this path
		//************************************ 
		std::string GetPathStringWithFileNameReplaced(std::string _fn_new) {
			std::string result = "";
			if (is_file()) {
				size_t until = m_path.size() - 1;
				for (size_t i = 0; i < until; ++i) {
					if (m_pathType == PathType::PT_Windows)
						result = result + m_path[i] + "\\";
					else
						result = result + m_path[i] + "/";
				}

				result = result + _fn_new;
			}
			return result;
		}

		//************************************  
		// @brief : return a string with filename prefix replaced with  _fn_new
		//			eg. D:\\test\\test.txt -> replace test with _fn_new
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: new path string 
		// @param : std::string _fn_new  
		// @note  : does not change this path
		//************************************ 
		std::string GetPathStringWithFileNamePrefixReplaced(std::string _fn_new) {
			std::string result = "";
			if (is_file()) {
				size_t until = m_path.size() - 1;
				for (size_t i = 0; i < until; ++i) {
					if (m_pathType == PathType::PT_Windows)
						result = result + m_path[i] + "\\";
					else
						result = result + m_path[i] + "/";
				}
				std::string fname = m_path.back();
				size_t pos = fname.find_last_of(".");
				if (pos == std::string::npos)
					std::runtime_error("can not find file extension string \".\"");
				fname = _fn_new + fname.substr(pos);
				result = result + fname;
			}
			return result;
		}
#ifdef _WIN32
		//************************************  
		// @brief : get driver's name  
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: driver's string name
		// @param : void  
		//************************************ 
		std::string GetDriveName() {
			std::string fullname = make_absolute();
			std::string drive = fullname.substr(0, 1);
			return drive;
		}
		//************************************  
		// @brief : get executable path name 
		// @author: SunHongLei
		// @date  : 2018/11/02  
		// @return: void
		// @param : void  
		//************************************ 
		std::string GetExecutablePath() {
			wchar_t exeFullPath[MAX_PATH];
			GetModuleFileName(NULL, exeFullPath, MAX_PATH);
			std::wstring wstr(exeFullPath);
			std::string strPath = ws2s(wstr);
			int pos = strPath.find_last_of('\\');
			return strPath.substr(0, pos);
		}
#endif
		//************************************  
		// @brief : get current work directory 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : void  
		//************************************ 
		std::string getcwd() {
#ifdef _WIN32
			wchar_t temp[MAX_PATH];
			if (!_wgetcwd(&temp[0], MAX_PATH))
				throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
			std::wstring wstr(temp);
			return ws2s(wstr);
#else
			char temp[PATH_MAX];
			if (::getcwd(temp, PATH_MAX) == NULL)
				throw std::runtime_error("Internal error in getcwd(): " + std::string(strerror(errno)));
			return std::string(temp);
#endif
		}
		//************************************  
		// @brief : make multi-level directory  
		// @author: SunHongLei
		// @date  : 2018/11/01  
		// @return: void
		// @param : void 
		// @note  : path must be a directory 
		//************************************ 
		void makedir() {
			int psz = m_path.size();
			int cursz = 1;
			while (cursz!=(psz+1)) {
				std::string result = "";
				for (size_t i = 0; i < cursz; ++i) {
					if (m_pathType == PathType::PT_Windows)
						result = result + m_path[i] + "\\";
					else
						result = result + m_path[i] + "/";
				}
				struct stat buffer;
				if (stat(result.c_str(), &buffer) != 0)
					makedir(result);
				cursz++;
			}
		}
		//************************************  
		// @brief : make single-level directory  
		// @author: SunHongLei
		// @date  : 2018/11/01  
		// @return: bool true if success
		// @param : std::string : directory path  
		//************************************ 
		bool makedir(std::string _path) {
			struct stat buffer;
			if (stat(_path.c_str(), &buffer) == 0)
				return false;
#ifdef _WIN32
			std::wstring wstr = s2ws(_path);
			return CreateDirectoryW(wstr.c_str(), NULL) != 0;
#else
			return mkdir(_path.str().c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
#endif
		}
		//************************************  
		// @brief : get file name in dir 
		// @author: SunHongLei
		// @date  : 2018/11/13  
		// @return: void
		// @param : std::vector<std::string >  filenames : file names found in dir
		// @param : std::string _specified_type : specified file type only extension needed,
		//			if  do not need,then assign _specified_type ""
		// @param : bool issubdir    :  if true search subdir
		//************************************ 
		bool GetFileNamesInDirectory(std::vector<std::string > &filenames,std::string _specified_type="",bool issubdir = true) {
			if (!exists())
				return false;
			if (!is_directory())
				return false;

			filenames.clear();
			listfileindir(filenames, m_originString, _specified_type,issubdir);

			return true;
		}
	protected:
		//************************************  
		// @brief : parse given path 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : void  
		//************************************ 
		void Parse(PathType _type ) {
			m_pathType = _type;
			if (_type == PathType::PT_Windows) {
				SeparatePath("/\\");
				m_bIsAbsolute = m_originString.size() >= 2 && isalpha(m_originString[0]) && m_originString[1] == ':';
			}
			else {
				SeparatePath("/");
				m_bIsAbsolute = !m_originString.empty() && m_originString[0] == '/';
			}
		}
		//************************************  
		// @brief : separate path with separator 
		// @author: SunHongLei
		// @date  : 2018/10/27  
		// @return: void
		// @param : const std::string &delim :  separator 
		//************************************ 
		void SeparatePath(const std::string &delim) {
			m_path.clear();
			std::string::size_type lastPos = 0, pos = m_originString.find_first_of(delim, lastPos);
			while (lastPos != std::string::npos) {
				if (pos != lastPos)
					m_path.push_back(m_originString.substr(lastPos, pos - lastPos));
				lastPos = pos;
				if (lastPos == std::string::npos || lastPos + 1 == m_originString.length())
					break;
				pos = m_originString.find_first_of(delim, ++lastPos);
			}
		}
		//************************************  
		// @brief : find all the file in dir include subdir 
		// @author: SunHongLei
		// @date  : 2018/11/13  
		// @return: void
		// @param : std::vector<std::string > : filenames found in dir
		// @param : std::string dir  :  current search directory
		// @param : std::string type :  specified file type: only file extension:eg."txt"
		// @param : bool issubdir    :  if true search subdir
		//************************************ 
		void listfileindir(std::vector<std::string > &filenames, std::string &dir,std::string &type,bool issubdir) {
			intptr_t hFile = 0;//文件句柄
			struct _finddata_t fileinfo;//文件信息，声明一个存储文件信息的结构体  
			std::string p;//字符串，存放路径
			if ((hFile = _findfirst(p.assign(dir).append("\\*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
			{
				do
				{
					//如果是目录,迭代之（即文件夹内还有文件夹）  
					if ((fileinfo.attrib &  _A_SUBDIR))
					{
						//文件名不等于"."&&文件名不等于".."
						//.表示当前目录
						//..表示当前目录的父目录
						//判断时，两者都要忽略，不然就无限递归跳不出去了！
						if (issubdir&&strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
							listfileindir(filenames,p.assign(dir).append("\\").append(fileinfo.name),type, issubdir);
					}
					//如果不是,加入列表  
					else
					{
						std::string dirNameDest = p.assign(dir).append("\\").append(fileinfo.name);
						if (type.size()) {
							std::size_t sz_name = dirNameDest.size();
							std::string::size_type pos = dirNameDest.find_last_of(".");
							if (pos != std::string::npos) {
								std::string ext = dirNameDest.substr(pos + 1);
								if(ext == type)
									filenames.push_back(dirNameDest);
							}
						}
						else
							filenames.push_back(dirNameDest);
					}
				} while (0 == _findnext(hFile, &fileinfo));
				_findclose(hFile);//_findclose函数结束查找
			}
		}
	protected:
#ifdef _WIN32
			///@brief convert string to wstring
			std::wstring s2ws( const std::string &str) const
			{
				std::string temp = str;
				int size = MultiByteToWideChar(CP_ACP, 0, &temp[0], (int)temp.size(), NULL, 0);
				std::wstring result(size, 0);
				MultiByteToWideChar(CP_ACP, 0, &temp[0], (int)temp.size(), &result[0], size);
				return result;
			}
			///@brief convert wstring to string
			std::string ws2s(const std::wstring &wstr) const
			{
				int size = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
				std::string result(size, 0);
				WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &result[0], size, NULL, NULL);
				return result;
			}
#endif
	protected:
		std::vector<std::string > m_path;
		std::string m_originString;
		PathType m_pathType;// path type
		bool m_bIsAbsolute;// is path is absolute
	};
};
#endif// 2018/10/18