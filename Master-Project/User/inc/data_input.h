#ifndef _DATA_INPUT_H_
#define _DATA_INPUT_H_


#define     VA_CH           1

typedef struct
{
    signed char close_dir;
	u16 adcvalue_valvelow;
    u16 adcvalue_valvehigh;
    u16 sen;//灵敏度
    float margin;
    float res_input;
    float res_output;
	u16 cal_low;
	u16 cal_high;
	u16 setinput_low;
	u16 setinput_high;
    
}SystemParameter_TypeDef;//系统参数结构体
extern SystemParameter_TypeDef* pSystemParam;
void SystemParam_Init();
void ReadSystemPara();

typedef struct
{
    float adcvalue_refer_filtered;
    float adcvalue_input_filtered;
    float adcvalue_valve_filtered;
    float adcvalue_encoder_filtered;
    u16 adcvalue_refer;//参考电压ADC采样值
    u16 adcvalue_input;//输入电压ADC采样值
    u16 adcvalue_valve;//阀位电压ADC采样值
    u16 adcvalue_encoder;//编码器采集值
}DataADC_TypeDef;
extern DataADC_TypeDef* pData_ADC;

typedef struct
{
    float   voltage_vcc;//VCC电源电压
    float   voltage_valve;//阀位实时电压
    float   voltage_input;//输入电流采集的电压
    
}DataVoltage_TypeDef;
extern DataVoltage_TypeDef* pData_Voltage;

typedef struct
{
    float   current_input;//输入电流
    float   opening;//实时开度
    float   current_output;//输出电流
    
}DataAcquire_TypeDef;//数据采集结构体
extern DataAcquire_TypeDef* pData_Acquire;

extern u8 use_encoder_flag;
void  ADC_Config();
u16   Get_Vx_adc(u8 Vx_CH);
void Acquire_Data();
void Detect_ValveInput();
void Update_InputCurrent();
void DataSampInit();
float Get_InputCurrent(const DataVoltage_TypeDef* pvoltage,const SystemParameter_TypeDef* psystem);
#endif