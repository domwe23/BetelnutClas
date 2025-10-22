

#if _MSC_VER > 1000
#pragma once
#endif //_MSC_VER > 1000

#if _WIN32_WINNT < _WIN32_WINNT_WIN2K
#error MXMPAC need _WIN32_WINNT >= _WIN32_WINNT_WIN2K.
#endif

#include <wtypes.h>

#ifdef __cplusplus
extern "C" {
#endif			

//********************************************************************************************************************************************************************
//                                                      1 系统设置函数  
//********************************************************************************************************************************************************************
//1.0 筛选功能初始化函数 必须在 MCF_Open_Net() 前调用
short WINAPI MCF_Sorting_Init_Net                     (unsigned short StationNumber = 0);

//1.1 控制卡打开关闭函数                              [1,100]                          [0,99]                          宏定义1.1                     
short WINAPI MCF_Open_Net                             (unsigned short Connection_Number,unsigned short *Station_Number, unsigned short *Station_Type);  
short WINAPI MCF_Get_Open_Net                         (unsigned short *Connection_Number,unsigned short *Station_Number, unsigned short *Station_Type);  
short WINAPI MCF_Close_Net                            (); 
//1.2 链接超时紧急停止函数                             [0,60000]
short WINAPI MCF_Set_Link_TimeOut_Net                 (unsigned long Time_1MS,unsigned long TimeOut_Output,unsigned short StationNumber = 0); 
//    链接超时紧急停止触发使能函数        
short WINAPI MCF_Set_Trigger_Output_Bit_Net           (unsigned short Bit_Output_Number,unsigned short Bit_Output_Enable,unsigned short StationNumber = 0);
//1.3 链接监测函数
short WINAPI MCF_Get_Link_State_Net                   (unsigned short  StationNumber = 0);  
//********************************************************************************************************************************************************************
//                                                     2 通用输入输出函数
//********************************************************************************************************************************************************************
//2.1 通用IO全部输出函数                               [OUT31,OUT0]                     [0,99]   [10000,10099]               
short WINAPI MCF_Set_Output_Net                       (unsigned long  All_Output_Logic, unsigned short StationNumber = 0);                                                    
short WINAPI MCF_Get_Output_Net                       (unsigned long *All_Output_Logic, unsigned short StationNumber = 0);       
//2.2 通用IO按位输出函数                               宏定义2.3.1                      宏定义2.3.2                      [0,99]  [10000,10099]   
short WINAPI MCF_Set_Output_Bit_Net                   (unsigned short Bit_Output_Number,unsigned short Bit_Output_Logic, unsigned short StationNumber = 0);                                                           
short WINAPI MCF_Get_Output_Bit_Net                   (unsigned short Bit_Output_Number,unsigned short *Bit_Output_Logic,unsigned short StationNumber = 0); 
//2.4 通用IO全部输入函数                               [Input31,Input0]                 [Input48,Input32]               [0,99]  [10000,10099]  
short WINAPI MCF_Get_Input_Net                        (unsigned long *All_Input_Logic1, unsigned long *All_Input_Logic2,unsigned short StationNumber = 0); 
//2.5 通用IO按位输入函数                               宏定义2.4.1                      宏定义2.4.2                     [0,99] 
short WINAPI MCF_Get_Input_Bit_Net                    (unsigned short Bit_Input_Number, unsigned short *Bit_Input_Logic,unsigned short StationNumber = 0);  
//********************************************************************************************************************************************************************
//                                                      3 轴专用输入输出函数
//********************************************************************************************************************************************************************
//3.1 伺服使能设置函数                                 宏定义0.0           宏定义3.1                   [0,99] 
short WINAPI MCF_Set_Servo_Enable_Net                 (unsigned short Axis,unsigned short  Servo_Logic,unsigned short StationNumber = 0); 
short WINAPI MCF_Get_Servo_Enable_Net                 (unsigned short Axis,unsigned short *Servo_Logic,unsigned short StationNumber = 0);
//3.2 伺服报警复位设置函数                             宏定义0.0           宏定义3.2                   [0,99] 
short WINAPI MCF_Set_Servo_Alarm_Reset_Net            (unsigned short Axis,unsigned short  Alarm_Logic,unsigned short StationNumber = 0);    
short WINAPI MCF_Get_Servo_Alarm_Reset_Net            (unsigned short Axis,unsigned short *Alarm_Logic,unsigned short StationNumber = 0); 
//3.3 伺服报警输入获取函数                             宏定义0.0           宏定义3.3                            [0,99]
short WINAPI MCF_Get_Servo_Alarm_Net                  (unsigned short Axis,unsigned short *Servo_Alarm_State,   unsigned short StationNumber = 0);
//********************************************************************************************************************************************************************
//                                                      4 轴设置函数
//********************************************************************************************************************************************************************
//4.1 脉冲通道输出设置函数                             宏定义0.0           宏定义4.1                 [0,99] 
short WINAPI MCF_Set_Pulse_Mode_Net                   (unsigned short Axis,unsigned long  Pulse_Mode,unsigned short StationNumber = 0);                                                       
short WINAPI MCF_Get_Pulse_Mode_Net                   (unsigned short Axis,unsigned long *Pulse_Mode,unsigned short StationNumber = 0);    
//4.2 位置设置函数                                     宏定义0.0           [-2^31,(2^31-1)] [0,99] 
short WINAPI MCF_Set_Position_Net                     (unsigned short Axis,long  Position,  unsigned short StationNumber = 0);                                                         
short WINAPI MCF_Get_Position_Net                     (unsigned short Axis,long *Position,  unsigned short StationNumber = 0);   
//4.3 编码器设置函数                                  宏定义0.0           [-2^31,(2^31-1)] [0,99] 
short WINAPI MCF_Set_Encoder_Net                      (unsigned short Axis,long  Encoder,   unsigned short StationNumber = 0);                                                            
short WINAPI MCF_Get_Encoder_Net                      (unsigned short Axis,long *Encoder,   unsigned short StationNumber = 0);  
//4.4 速度获取                                        宏定义0.0           [-2^15,(2^15-1)]    [-2^15,(2^15-1)]   [0,99] 
short WINAPI MCF_Get_Vel_Net                          (unsigned short Axis,double *Command_Vel,double *Encode_Vel,unsigned short StationNumber = 0);        
//********************************************************************************************************************************************************************
//                                                      5 轴硬件触发停止运动函数
//********************************************************************************************************************************************************************
//5.1 通用IO输入复用：做为紧急停止函数                 宏定义2.4.1                      宏定义5.1                [0,99] 
short WINAPI MCF_Set_EMG_Bit_Net                      (unsigned short EMG_Input_Number, unsigned short  EMG_Mode,unsigned short StationNumber = 0); 
short WINAPI MCF_Set_EMG_Output_Net                   (unsigned short EMG_Input_Number, unsigned short  EMG_Mode,unsigned long EMG_Output,unsigned short StationNumber = 0);     
short WINAPI MCF_Set_EMG_Output_Enable_Net            (unsigned short Bit_Output_Number,unsigned short Bit_Output_Enable,unsigned short StationNumber = 0);
//5.11 轴状态触发停止运动查询函数                      宏定义0.0           MC_Retrun.h[0,28]      [0,99] 
short WINAPI MCF_Get_Axis_State_Net                   (unsigned short Axis,unsigned short *Reason,unsigned short StationNumber = 0); 
//********************************************************************************************************************************************************************
//                                                      7 点位运动控制函数
//********************************************************************************************************************************************************************
//7.1 速度控制函数                                     宏定义0.0           (0,10M]P/S    (0,1T]P^2/S  [0,99] 
short WINAPI MCF_JOG_Net                              (unsigned short Axis,double dMaxV, double dMaxA,unsigned short StationNumber = 0);                                                  
//7.4 单轴曲线函数                                     宏定义0.0           [0,dMaxV]      (0,10M]P/S    (0,1T]P^2/S   (0,100T]P^3/S [0,dMaxV]      宏定义0.4               [0,99] 
short WINAPI MCF_Set_Axis_Profile_Net                 (unsigned short Axis,double  dV_ini,double dMaxV, double  dMaxA,double  dJerk,double  dV_end,unsigned short  Profile,unsigned short StationNumber = 0);     
short WINAPI MCF_Get_Axis_Profile_Net                 (unsigned short Axis,double *dV_ini,double *dMaxV,double *dMaxA,double *dJerk,double *dV_end,unsigned short *Profile,unsigned short StationNumber = 0);  
//7.5 单轴运动函数                                     宏定义0.0           [-2^31,(2^31-1)]  宏定义0.3                    [0,99] 
short WINAPI MCF_Uniaxial_Net                         (unsigned short Axis,long dDist,       unsigned short Position_Mode,unsigned short StationNumber = 0);                                                           
//7.6 单轴停止曲线函数                                 宏定义0.0           (0,1T]P^2/S   (0,100T]P^3/S 宏定义0.4               [0,99] 
short WINAPI MCF_Set_Axis_Stop_Profile_Net            (unsigned short Axis,double  dMaxA,double  dJerk,unsigned short  Profile,unsigned short StationNumber = 0);                    
short WINAPI MCF_Get_Axis_Stop_Profile_Net            (unsigned short Axis,double *dMaxA,double *dJerk,unsigned short *Profile,unsigned short StationNumber = 0);
//7.7 轴停止函数                                       宏定义0.0           宏定义7.7                     [0,99] 
short WINAPI MCF_Axis_Stop_Net                        (unsigned short Axis,unsigned short Axis_Stop_Mode,unsigned short StationNumber = 0); 
/********************************************************************************************************************************************************************
                                                      16 光源控制器函数
********************************************************************************************************************************************************************/
//16.1 设置光源模式(1MS阻塞函数)                       宏定义16.1.1         0：关闭 1:24V常亮 2:24V频闪 3:48V爆闪  
short WINAPI MCF_Set_Light_Mode_Net                   (unsigned short Channel,unsigned short Light_Mode,unsigned short StationNumber = 0);
//16.2 设置电流保护(1MS阻塞函数)                       宏定义16.1.1           常亮:[0,2000] 频闪[0,14999] 单位：MA      
short WINAPI MCF_Set_Light_Current_Net                (unsigned short Channel,unsigned short Max_Current,unsigned short StationNumber = 0);
short WINAPI MCF_Get_Light_Current_1_4_Net            (unsigned short *Current_1,unsigned short *Current_2,unsigned short *Current_3,unsigned short *Current_4,unsigned short StationNumber = 0);
short WINAPI MCF_Get_Light_Current_5_8_Net            (unsigned short *Current_5,unsigned short *Current_6,unsigned short *Current_7,unsigned short *Current_8,unsigned short StationNumber = 0);
//16.3 设置光源输出(1MS阻塞函数)                       宏定义16.1.1           常亮:[0,255] 频闪[0,1000]
short WINAPI MCF_Set_Light_Output_Net                 (unsigned short Channel,unsigned short Light_Size,unsigned short StationNumber = 0);
///********************************************************************************************************************************************************************
//                                                       17 系统函数
//********************************************************************************************************************************************************************
//17.1 模块版本号                                     [0x00000000,0xFFFFFFFF] [0,99] 
short WINAPI MCF_Get_Version_Net                      (unsigned long *Version,unsigned short StationNumber = 0);                                                  
//17.2 序列号                                         [0x00000000,0xFFFFFFFF] [0,99] 
short WINAPI MCF_Get_Serial_Number_Net                (INT64 *Serial_Number,unsigned short StationNumber = 0);     
//17.3 模块运行时间                                   [0x00000000,0xFFFFFFFF] [0,99]    单位：秒
short WINAPI MCF_Get_Run_Time_Net                     (unsigned long *Run_Time,unsigned short StationNumber = 0); 
//17.4 Flash 读写功能目前暂时大小2Kbytes,也即定义一个 unsigned int Array[256] 存放数据
short WINAPI MCF_Flash_Write_Net                      (unsigned long Pass_Word_Setup,unsigned long *Flash_Write_Data,unsigned short StationNumber = 0);
short WINAPI MCF_Flash_Read_Net                       (unsigned long Pass_Word_Check,unsigned long *Flash_Read_Data,unsigned short StationNumber = 0);
//17.8 系统定时回调函数
short WINAPI MCF_Set_CallBack_Net                     (long CallBack,unsigned long Time_1MS);

/********************************************************************************************************************************************************************
                                                      101 关闭自动筛选功能并清除来料,相机,吹气计数      注意：调用该函数后才可以设置102,103,104项目参数    
********************************************************************************************************************************************************************/
//101.1 设置参数前必须调用先关闭筛选功能
short WINAPI MCF_Sorting_Close_Net                                (unsigned short StationNumber = 0);
/********************************************************************************************************************************************************************
                                                      102 设置来料检测功能,用户根据需要设置             注意：自动筛选时禁止设置
********************************************************************************************************************************************************************/
//102.1 物件最大最小尺寸
short WINAPI MCF_Sorting_Set_Piece_Size_Net                       (unsigned long Max_Size, unsigned long Min_Size,unsigned short StationNumber = 0); 
//102.2 物件安全距离,安全时间
short WINAPI MCF_Sorting_Set_Piece_Place_Net                      (unsigned long Min_Distance, unsigned long Min_Time_Intervel,unsigned short StationNumber = 0); 
//102.3 来料检测设置
//      来料检测使能(默认Bit_Output_0,Bit_Output_1开,0：关 1：开) [Bit_Input_0,Bit_Input_3]
short WINAPI MCF_Sorting_Set_Input_Enable_Net                     (unsigned short Bit_Input_Number,unsigned short Bit_Input_Enable,unsigned short StationNumber = 0);
//      来料检测电平(默认全部低电平，  0：低电平  1：高电平)      [Bit_Input_0,Bit_Input_3]
short WINAPI MCF_Sorting_Set_Input_Logic_Net                      (unsigned short Bit_Input_Number,unsigned short Bit_Input_Logic,unsigned short StationNumber = 0);
//      来料检测编码器(默认全部跟随Axis_1编码器)                  [Bit_Input_0,Bit_Input_3]                            0:命令 1:编码器(默认)
short WINAPI MCF_Sorting_Set_Input_Source_Net                     (unsigned short Bit_Input_Number,unsigned short Axis,unsigned short Source,unsigned short StationNumber = 0);
//      来料检测捕获位置(默认全部捕获前部，  0：前部  1：中间)    [Bit_Input_0,Bit_Input_3]
short WINAPI MCF_Sorting_Set_Input_Position_Net                   (unsigned short Bit_Input_Number,unsigned short Mode,unsigned short StationNumber = 0);
//      来料检测自动清除编码器(清除完自动关闭使能)                 [Bit_Input_0,Bit_Input_3]
short WINAPI MCF_Sorting_Set_Input_Clear_Encoder_Net              (unsigned short Bit_Input_Number,unsigned short Enable,unsigned short StationNumber = 0);
//102.4 DI00 检测不连续物件强制保持连续
short WINAPI MCF_Sorting_Set_Piece_Keep_Net                       (unsigned long Keep_Length, unsigned short StationNumber = 0); 
//102.5 DI00 物件检测无料超时停止轴运动(默认时间0,表示不启动)      [0,60000]             &Array[Channel00,Channel15],0:低 1:高 2:关闭  
short WINAPI MCF_Sorting_Set_Input_0_TimeOut_Net                  (unsigned long Time_1MS,unsigned long *TimeOut_Output,unsigned short StationNumber = 0); 
//                                                                 [0,15]                [DO00,DO47]         
short WINAPI MCF_Sorting_Set_Input_0_Config_Net                   (unsigned short Channel,unsigned short Bit_Input_Number,unsigned short StationNumber = 0); 
//102.6 通用IO按位输入滤波函数                                    [Bit_Input_0,Bit_Input_3]        [1,100]MS                     [0,99] 
//short WINAPI MCF_Set_Input_Filter_Time_Bit_Net                    (unsigned short Bit_Input_Number, unsigned long Filter_Time_1MS,unsigned short StationNumber = 0); 
//102.7 多个来料检测绑定相机触发,OK吹气,NG吹气                    [Bit_Input_1]                    [2,]                                 [2,]
short WINAPI MCF_Sorting_Set_Input_Bind_Net                       (unsigned short Bit_Input_Number,unsigned short Camera_Start_Number,unsigned short Bond_Start_Number,unsigned short StationNumber = 0); 
/********************************************************************************************************************************************************************
                                                      103 设置OK,NG安全保护参数,用户根据需要设置       注意：自动筛选时禁止设置
********************************************************************************************************************************************************************/
//103.1 物件吹气OK超时停止轴运动(默认时间0,表示不启动)              [0,60000]             &Array[DO00,DO15],0:低 1:高 2:关闭 
short WINAPI MCF_Sorting_Set_Trig_Blow_OK_TimeOut_Net             (unsigned long Time_1MS,unsigned long *TimeOut_Output,unsigned short StationNumber = 0); 
//103.2 物件吹气连续NG停止轴运动(默认时间0,表示不启动)              [0,60000]              &Array[DO00,DO15],0:低 1:高 2:关闭  
short WINAPI MCF_Sorting_Set_Trig_Blow_NG_NumberOut_Net           (unsigned long NG_Number,unsigned long *NumberOut_Output,unsigned short StationNumber = 0); 
//103.3 HMC3432S/HMC3412S 可以设置物件重新检测确定功能，以此判定是否误吹
short WINAPI MCF_Sorting_Set_Blow_Check_Again_Net                 (unsigned short Bit_Input_Number,unsigned short Bit_Input_Logic,long Input_Position,unsigned long Piece_Size,
	                                                               unsigned short Blow_OK_Check,
																   unsigned short Blow_NG_Check,
																   unsigned short Blow_1_Check,
																   unsigned short Blow_2_Check,
																   unsigned short Blow_3_Check,
																   unsigned short Blow_4_Check,
																   unsigned short Blow_5_Check,
																   unsigned short Blow_6_Check,
	                                                               unsigned short StationNumber = 0);
short WINAPI MCF_Sorting_Get_Blow_Check_Lose_Number_Net           (unsigned long *Lose_Number,unsigned short StationNumber = 0);
/********************************************************************************************************************************************************************
                                                      104 设置相机吹气参数,用户必须设置                注意：自动筛选时禁止设置
********************************************************************************************************************************************************************/
//104.1 HMC3432S 可以配置相机和吹气个数                             [1,30]                        [1,30]
short WINAPI MCF_Sorting_Camera_Blow_Config_Net                   (unsigned short Camera_Number,unsigned short Blow_Number,unsigned short StationNumber = 0); 
//104.2 设置相机参数                                                                              与检测装置的相对位置   编码器反馈的运行方向 
short WINAPI MCF_Sorting_Set_Camera_Net                           (unsigned short Camera_Number,long Camera_Position,unsigned short Motion_Dir,unsigned short Action_Mode,unsigned short Action_IO,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Camera_Light_Frequency_Net           (unsigned short Camera_Number,unsigned short Light_Number,unsigned short Frequency_Enable,unsigned short StationNumber = 0);
//    设置光源提前触发时间偏移                                                                  [100,1000] 单位：us
short WINAPI MCF_Sorting_Set_Camera_Light_Early_Net               (unsigned short Camera_Number,unsigned short Early_Time,unsigned short StationNumber = 0);
//    设置相机反馈下降沿超时停止轴                                                                                              [0,1000] 单位：MS
short WINAPI MCF_Sorting_Set_Camera_TimeOut_Net                   (unsigned short Camera_Number,unsigned short Bit_Input_Number,unsigned short Time_1MS,unsigned short StationNumber = 0);
//104.3 设置触发相机拍照后,延时多少毫秒计数增加1,一般设置为 大于控制卡从触发相机拍照到软件出图像结果需要的时间 [0,655] 单位：ms
short WINAPI MCF_Sorting_Set_Trig_Camera_Delay_Count_Net          (unsigned short Camera_Number,double Camera_Delay_Count_MS,unsigned short StationNumber = 0); 
//104.4 设置OK吹气参数
short WINAPI MCF_Sorting_Set_Blow_OK_Net                          (                            long Blow_OK_Position,unsigned short Motion_Dir,unsigned short Action_Mode,unsigned short Action_IO,unsigned short StationNumber = 0); 
//104.5 设置NG吹气参数
short WINAPI MCF_Sorting_Set_Blow_NG_Net                          (                            long Blow_NG_Position,unsigned short Motion_Dir,unsigned short Action_Mode,unsigned short Action_IO,unsigned short StationNumber = 0);
//104.6 设置吹气1到30参数                                          [1,30]                  
short WINAPI MCF_Sorting_Set_Blow_Net                             (unsigned short Blow_Number, long Blow_Position,   unsigned short Motion_Dir,unsigned short Action_Mode,unsigned short Action_IO,unsigned short StationNumber = 0); 
//104.7 设置相机拍照,OK吹气,NG吹气，气阀吹气位置偏移，偏移大小为物件大小的比例                  [0,100]
short WINAPI MCF_Sorting_Set_Camera_Trig_Offset_Net               (unsigned short Camera_Number,short Size_Ratio,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Blow_OK_Trig_Offset_Net              (                             short Size_Ratio,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Blow_NG_Trig_Offset_Net              (                             short Size_Ratio,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Blow_Trig_Offset_Net                 (unsigned short Blow_Number,  short Size_Ratio,unsigned short StationNumber = 0); 
/********************************************************************************************************************************************************************
                                                      105 自动筛选功能启动函数                         注意：调用该函数后禁止设置102,103,104项目参数
********************************************************************************************************************************************************************/
//105.1 筛选启动函数,在设置好参数后启动
short WINAPI MCF_Sorting_Start_Net                                (unsigned short Mode = 0,unsigned short StationNumber = 0);
/********************************************************************************************************************************************************************
                                                      106 物料图像结果处理
********************************************************************************************************************************************************************/
//106.0 设置相机处理结果最短时间和最大超时时间,允许连续超时最大个数
short WINAPI MCF_Sorting_Set_Camera_Handle_Time_Net               (unsigned short Camera_Number,double Handle_Time_1MS,double Handle_TimeOut_1MS,unsigned long Handle_TimeOut_Number,unsigned short StationNumber = 0); 
//106.1 吹气模式0：用户综合所有相机结果后发送吹气指令 
//      用户在图像处理回调函数中调用该函数通知图像处理结果
short WINAPI MCF_Sorting_Set_Camera_Result_Data_Net               (unsigned short Camera_Number,unsigned long Result_Data,unsigned short StationNumber = 0);
//      用户开辟线程检查物料最新的图像结果
short WINAPI MCF_Sorting_Get_Camera_Result_Updata_Net             (unsigned short Camera_Number,unsigned long *Piece_Number,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Get_Camera_Result_Buffer_Net             (unsigned short Camera_Number,unsigned long  Piece_Number,unsigned long *Result_Buffer,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Get_Camera_Handle_Time_Net               (unsigned short Camera_Number,unsigned long  Piece_Number,unsigned long *Handle_Time,unsigned short StationNumber = 0); 
//      用户根据图像结果吹气
short WINAPI MCF_Sorting_Set_Trig_Blow_OK_Net                     (unsigned long  Piece_Number,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Trig_Blow_NG_Net                     (unsigned long  Piece_Number,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Trig_Blow_Net                        (unsigned short Blow_Number,unsigned long Piece_Number,unsigned short StationNumber = 0); 
//106.2 吹气模式1：用户直接发送每个相机结果，控制卡自动综合结果后吹气
short WINAPI MCF_Sorting_Set_Camera_Result_OK_Net                 (unsigned short Camera_Number,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Camera_Result_NG_Net                 (unsigned short Camera_Number,unsigned short StationNumber = 0); 

short WINAPI MCF_Sorting_Get_Blow_Result_OK_Net                   (unsigned long  *Result_OK_Number,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Get_Blow_Result_NG_Net                   (unsigned long  *Result_NG_Number,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Get_Blow_Result_Miss_Net                 (unsigned long  *Result_Miss_Number,unsigned short StationNumber = 0); 
//106.3 吹气模式2：用户不需要发送相机结果，控制卡通过IO综合结果后吹气,全程通过硬件实现,可以直接替代PLC       (0,500]
short WINAPI MCF_Sorting_Set_Camera_Result_Input_OK_Net           (unsigned short Camera_Number,unsigned short Input_Number,unsigned short Logic,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Set_Camera_Result_Input_NG_Net           (unsigned short Camera_Number,unsigned short Input_Number,unsigned short Logic,unsigned short StationNumber = 0); 
/********************************************************************************************************************************************************************
                                                      107 物件,相机,吹气状态监测函数
********************************************************************************************************************************************************************/
//107.0 筛选监测函数
short WINAPI MCF_Sorting_Get_State_Net                            (unsigned short *State,unsigned short StationNumber = 0);  
//107.1 获取DI00物件不合格计数个数
short WINAPI MCF_Sorting_Get_Piece_Pass_Net                       (unsigned short Piece_Input_Number,unsigned long *Piece_Pass,unsigned short StationNumber = 0);
short WINAPI MCF_Sorting_Get_Piece_Pass_Size_Net                  (unsigned short Piece_Input_Number,unsigned long *Piece_Pass_Size,unsigned short StationNumber = 0);
short WINAPI MCF_Sorting_Get_Piece_Pass_Size_Max_Net              (unsigned short Piece_Input_Number,unsigned long *Piece_Pass_Size_Max,unsigned short StationNumber = 0);
short WINAPI MCF_Sorting_Get_Piece_Pass_Size_Min_Net              (unsigned short Piece_Input_Number,unsigned long *Piece_Pass_Size_Min,unsigned short StationNumber = 0);
short WINAPI MCF_Sorting_Get_Piece_Pass_Dist_Net                  (unsigned short Piece_Input_Number,unsigned long *Piece_Pass_Dist,unsigned short StationNumber = 0);
short WINAPI MCF_Sorting_Get_Piece_Pass_Time_Net                  (unsigned short Piece_Input_Number,unsigned long *Piece_Pass_Time,unsigned short StationNumber = 0);
//107.2 获取DI00物件计数个数
short WINAPI MCF_Sorting_Get_Piece_State_Net                      (unsigned short Piece_Input_Number,         //物件计数的输入端口号
                                                                   unsigned long *Piece_Find,                 //物件匹配统计数量
																   unsigned long *Piece_Size,                 //物件大小，10组
																   unsigned long *Piece_Distance_To_next,     //物件间距，10组
																   unsigned long *Piece_Cross_Camera,         //物件经过所有相机个数
																   unsigned short StationNumber = 0); 	
//107.3 获取判断控制卡触发相机拍照计数,图像结果输出结果一定要先于延时后的控制卡相机拍照计数，而且要一一对应,否则做为图像异常或者漏拍处理
short WINAPI MCF_Sorting_Get_Trig_Camera_Count_Net                (unsigned short Camera_Number,unsigned long *Trig_Camera_Count,unsigned short StationNumber = 0); 
//107.4 获取OK,NG触发计数
short WINAPI MCF_Sorting_Get_Trig_Blow_NG_Count_Net               (unsigned long *Trig_Blow_NG_Count,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Get_Trig_Blow_OK_Count_Net               (unsigned long *Trig_Blow_OK_Count,unsigned short StationNumber = 0); 
//107.5 获取OK,NG漏触发计数
short WINAPI MCF_Sorting_Get_Lose_Blow_NG_Count_Net               (unsigned long *Lose_Blow_NG_Count,unsigned short StationNumber = 0); 
short WINAPI MCF_Sorting_Get_Lose_Blow_OK_Count_Net               (unsigned long *Lose_Blow_OK_Count,unsigned short StationNumber = 0); 
//107.6 获取气阀吹气计数
short WINAPI MCF_Sorting_Get_Trig_Blow_Count_Net                  (unsigned short Blow_Number,unsigned long *Trig_Blow_Count,unsigned short StationNumber = 0); 


short WINAPI MCF_Set_Compare_Config_Net               (unsigned short Channel,unsigned short  Enable,unsigned short  Compare_Source,unsigned short StationNumber = 0);

short WINAPI MCF_Add_Compare_Point_Net                (unsigned short Channel,long  Position,unsigned short Dir, unsigned short Action,unsigned short Actpara,unsigned short StationNumber = 0);

short WINAPI MCF_Get_Compare_Config_Net               (unsigned short Channel,unsigned short *Enable,unsigned short *Compare_Source,unsigned short StationNumber = 0);

short WINAPI MCF_Get_Input_Fall_Count_Bit_Net         (unsigned short Bit_Input_Number, unsigned long *Input_Count_Fall,unsigned long *Lock_Data_Buffer,unsigned short StationNumber = 0);

#ifdef __cplusplus
}
#endif






