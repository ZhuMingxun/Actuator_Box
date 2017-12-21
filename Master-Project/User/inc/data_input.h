#ifndef _DATA_INPUT_H_
#define _DATA_INPUT_H_


#define     VA_CH           1

typedef struct
{
    signed char close_dir;
	u16 adcvalue_valvelow;
    u16 adcvalue_valvehigh;
    u16 sen;//������
    float margin;
    float res_input;
    float res_output;
	u16 cal_low;
	u16 cal_high;
	u16 setinput_low;
	u16 setinput_high;
    
}SystemParameter_TypeDef;//ϵͳ�����ṹ��
extern SystemParameter_TypeDef* pSystemParam;
void SystemParam_Init();
void ReadSystemPara();

typedef struct
{
    float adcvalue_refer_filtered;
    float adcvalue_input_filtered;
    float adcvalue_valve_filtered;
    float adcvalue_encoder_filtered;
    u16 adcvalue_refer;//�ο���ѹADC����ֵ
    u16 adcvalue_input;//�����ѹADC����ֵ
    u16 adcvalue_valve;//��λ��ѹADC����ֵ
    u16 adcvalue_encoder;//�������ɼ�ֵ
}DataADC_TypeDef;
extern DataADC_TypeDef* pData_ADC;

typedef struct
{
    float   voltage_vcc;//VCC��Դ��ѹ
    float   voltage_valve;//��λʵʱ��ѹ
    float   voltage_input;//��������ɼ��ĵ�ѹ
    
}DataVoltage_TypeDef;
extern DataVoltage_TypeDef* pData_Voltage;

typedef struct
{
    float   current_input;//�������
    float   opening;//ʵʱ����
    float   current_output;//�������
    
}DataAcquire_TypeDef;//���ݲɼ��ṹ��
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