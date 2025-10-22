//#pragma once

//#pragma comment(lib, "ole32.lib")


#ifndef PROFINETMANAGED_d
#define PROFINETMANAGED_d

//#define net_api __declspec(dllexport) 
#define net_api extern "C" __declspec(dllexport)

#if defined(DLL_EXPORTS)
#define MYFUNC_API __declspec(dllexport)
#else
#define MYFUNC_API __declspec(dllimport)
#endif


#undef PROFINET_INPUT
#undef PROFINET_OUTPUT
#define PROFINET_INPUT
#define PROFINET_OUTPUT

typedef int netHandle;
typedef unsigned short uint16;
typedef short int16;
typedef int int32;



typedef int netStatus;
#define net_succeed 0
#define net_handle_error -1
#define net_comm_error -2




#include <string>
#include <vector>
#include <map>

template<typename T>
struct Result {
	int isSuccess;
	std::vector<T> value;
	std::string message;

	Result()
		: isSuccess(0), value(), message("") {}

	Result(int status, const std::vector<T>& value, const std::string& message)
	{
		isSuccess = status;
		this->value = value;
		this->message = message;
	}
};






//template<typename T>
//class profinetResult2
//{
//public:
//	int status;
//
//	std::vector<T> values;
//
//	profinetResult2(int status, std::vector<T> values)
//	{
//		this->status = status;
//		this->values = values;
//	}
//};

enum PROFINET_TYPE
{
	OMRON_FINS,
	OMRON_CIP,
	MELSEC_MC_BIN,
	MODBUS_TCP,
	KEYENCE,
	SIEMENS
};

/// <summary>
/// 西门子的PLC类型，目前支持的访问类型
/// </summary>
enum Siemens_PlcType
{
	/// <summary>
	/// 1200系列
	/// </summary>
	S1200 = 1,
	/// <summary>
	/// 300系列
	/// </summary>
	S300 = 2,
	/// <summary>
	/// 400系列
	/// </summary>
	S400 = 3,
	/// <summary>
	/// 1500系列PLC
	/// </summary>
	S1500 = 4,
	/// <summary>
	/// 200的smart系列
	/// </summary>
	S200Smart = 5,
	/// <summary>
	/// 200系统，需要额外配置以太网模块
	/// </summary>
	S200 = 6
};


enum Trans_Data_Format
{
	/// <summary>
	/// 按照顺序排序
	/// </summary>
	ABCD = 0,
	/// <summary>
	/// 按照单字反转
	/// </summary>
	BADC = 1,
	/// <summary>
	/// 按照双字反转
	/// </summary>
	CDAB = 2,
	/// <summary>
	/// 按照倒序排序
	/// </summary>
	DCBA = 3,
};





namespace ProfinetManaged {

	enum class StopBits {
		None = 0,
		One = 1,
		Two = 2,
		OnePointFive = 3
	};

	enum class Parity {
		None = 0,
		Odd = 1,
		Even = 2,
		Mark = 3,
		Space = 4
	};

	MYFUNC_API Result<int> ResultValues1();

	MYFUNC_API std::map<int, std::string> get_error_list();

	MYFUNC_API std::map<int, bool> get_state_list_native();

	MYFUNC_API bool get_connect_state(int handle);

	MYFUNC_API std::string get_last_error_native(int handle);

	MYFUNC_API bool remove(int handle);


	/*net_api Result<int> ResultValues2();*/

	//3/27 添加一个重载  SiemensPlcType 仅在西门子plc时有效
	MYFUNC_API Result<int> profinet_create(PROFINET_INPUT const char* ip, PROFINET_INPUT int port, PROFINET_INPUT PROFINET_TYPE type, PROFINET_OUTPUT netHandle* handle, int SiemensPlcType);
	MYFUNC_API Result<int> profinet_create_melsec_mc(PROFINET_INPUT const char* ip, PROFINET_INPUT int port, PROFINET_OUTPUT netHandle* handle);
	MYFUNC_API  Result<int> profinet_create_omron_fins(PROFINET_INPUT const char* ip, PROFINET_INPUT int port, PROFINET_OUTPUT netHandle* handle);
	//plcType 参考 #SiemensPlcTYPE
	MYFUNC_API Result<int> profinet_create_Siemens_s7(int plcType, PROFINET_INPUT const char* ip, int port, PROFINET_OUTPUT netHandle* handle);

	MYFUNC_API Result<int> modify_modbus_rtu_station(int handle, int station);

	MYFUNC_API Result<int> modbus_rtu_create(const char* portName, int baudRate, unsigned char station, int dataBits, StopBits stopBits, Parity parity, int* handle);
	//添加通讯端口
	MYFUNC_API Result<int> profinet_create_keyence_nanoovertcp(PROFINET_INPUT const char* ip, PROFINET_INPUT int port, PROFINET_OUTPUT netHandle* handle);

	//MYFUNC_API Result<int> profinet_create_melsec_fx_serial(PROFINET_INPUT const char* COM, int baudRate, int dataBits, int stopBits, int parity, PROFINET_OUTPUT netHandle* handle);

	/*net_api netStatus profinet_write_fx(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT unsigned char* value, char* msg);*/


	/*net_api profinetResult profinet_read_int16_fx(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API netStatus profinet_write_int16_fx(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int16* value, PROFINET_INPUT int len);*/


	MYFUNC_API Result<int> profinet_append_port(netHandle handle, int port);
	//设置多通讯端口	
	MYFUNC_API Result<int> profinet_set_multi_ports(netHandle handle, int* ports, int len);

	//绑定本地ip地址和端口 端口为0时 则可使用任何端口
	MYFUNC_API Result<int> profinet_bind_local(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* ip, PROFINET_INPUT int port);
	//设置通讯时字节顺序
	MYFUNC_API Result<int> profinet_set_data_fromat(PROFINET_INPUT netHandle handle, Trans_Data_Format dataFormt);
	MYFUNC_API Result<int> profinet_free(PROFINET_INPUT netHandle handle);
	//net_api netStatus profinet_disconnect(PROFINET_INPUT netHandle handle);
	// msg 需要使用 freee memory 释放
	//MYFUNC_API Result<int> profinet_get_last_error(PROFINET_INPUT netHandle handle, char* msg);
	MYFUNC_API void profinet_free_memory(PROFINET_INPUT void* msg);
	MYFUNC_API Result<int> profinet_network_info(PROFINET_INPUT netHandle handle, PROFINET_OUTPUT char** ip, PROFINET_OUTPUT int* port);
	//net_api netStatus profinet_connect_state(PROFINET_INPUT netHandle handle,PROFINET_OUTPUT bool* is_connected); 
	MYFUNC_API Result<int> profinet_ping(PROFINET_INPUT netHandle handle, int* ipState);
	MYFUNC_API Result<int> profinet_string_reverse(PROFINET_INPUT netHandle handle, bool reverse);
	MYFUNC_API Result<int> profinet_pipesocket_connect(PROFINET_INPUT netHandle handle, int sendTimeout = 3000, int receiveTimeout = 3000);
	MYFUNC_API Result<int> profinet_set_pipesocket(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int* ports, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_setTimeout(PROFINET_INPUT netHandle handle, int timeout);

	MYFUNC_API Result<double> profinet_read_double(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<float> profinet_read_float(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<uint16_t> profinet_read_uint16(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<short> profinet_read_int16(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_read_int32(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<bool> profinet_read_bool(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<std::string> profinet_read_string(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<unsigned char> profinet_read_byte(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_write_double(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT double* value, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_write_float(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT float* value, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_write_uint16(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT uint16* value, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_write_int16(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int16* value, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_write_int32(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT int32* value, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_write_bool(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_INPUT bool* value, PROFINET_INPUT int len);
	MYFUNC_API Result<int> profinet_write_string(PROFINET_INPUT netHandle handle, PROFINET_INPUT const char* address, PROFINET_OUTPUT const char* value, PROFINET_INPUT int len);

	MYFUNC_API Result<int> profinet_set_coda(PROFINET_INPUT const char* code);
}


#endif