
#include "Exti.h"
#include "IORedefine.h"
#include "phase_detect.h"
#include "control_new.h"
#include "mode_detect.h"
#include "delay.h"
#include "motor.h"
#include "com.h"
static PhaseSeq_Type phaseseq_status;//枚举变量，相序状态
static CapSeq_TypeDef Cap_Seq;//序列捕获结构体
bit phase_seq = 1;
u8 phase_update_ok=0;

/*************************INT0,INT1 配置********************************/

static void PhsaeSeqIO_Config()
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Mode = EXT_MODE_RiseFall;
    EXTI_InitStructure.EXTI_Polity = PolityHigh;
    EXTI_InitStructure.EXTI_Interrupt = ENABLE;
    Ext_Inilize(EXT_INT0,&EXTI_InitStructure);
    Ext_Inilize(EXT_INT1,&EXTI_InitStructure);
    EA = 1;
    
}

static void CapSeq_Init(CapSeq_TypeDef* pCapSeq)
{
    pCapSeq->cap_sequence = 0;
    pCapSeq->cap_cnt = 0;
    pCapSeq->capok_flag = 0;
    EX0 = 1;
    EX1 = 1;
}

/*************相序检测初始化配置**************/
void PhaseSeq_Detect_Config()
{
    PhsaeSeqIO_Config();
    CapSeq_Init(&Cap_Seq); 
    phase_update_ok = 0;
    while(phase_update_ok !=1)
    {
        PhaseSeq_Update();
    }
}



/*************相序更新函数****************/
void PhaseSeq_Update()//主函数执行
{
    static u8 err_cnt = 0;
    u8 seq;
    
    if(Cap_Seq.capok_flag == 1)
    {
        seq = Cap_Seq.cap_sequence;
        switch(seq)
        {
            case 0x1E:case 0x87:case 0xE1:case 0x78:
            {
                if(phaseseq_status != PHASESEQ_POSITIVE)  phaseseq_status = PHASESEQ_POSITIVE;
                phase_seq = 1;
                phase_update_ok = 1;
                if(err_cnt != 0) err_cnt = 0;   
                break;
            }
            case 0x2D:case 0xD2:case 0x4B:case 0xB4:
            {
                if(phaseseq_status != PHASESEQ_REVERSE)  phaseseq_status = PHASESEQ_REVERSE;
                phase_seq = 0;
                phase_update_ok = 1;
                if(err_cnt != 0) err_cnt = 0; 
                break;
            }
            default:
            {
                err_cnt++;
                if(err_cnt>=10)
                {
                    err_cnt = 0;
                    phaseseq_status = PHASESEQ_LACK;
                    mode = MODE_LACK_PHASE; 
                    phase_update_ok = 1;
                }
                break;
            }
        }
        
        
        CapSeq_Init(&Cap_Seq);
    }
}



/****************缺相模式函数*********************/
void LackPhase_Mode()
{
    u8 ep_on = 1;
    Motor_Stop();
    ERR_OUT
    while(mode == MODE_LACK_PHASE)
    {
		#ifdef WATCH_DOG
		WDT_CONTR = WATCH_DOG_RSTVAL;
		#endif
 
        if(timer1_200ms_flag == 1)
        {
            timer1_200ms_flag = 0;
            PhaseSeq_Update();
            if(phaseseq_status != PHASESEQ_LACK)    break;
            TransmitFrameToSlaver();
        }      
    }
    
    SystemMode_Detect();
    ERR_OFF
}

/************相序捕获函数*****************/
/* description:负责填充捕获的序列    */
void PhaseSeq_Cap_A(CapSeq_TypeDef* pCapSeq,u8 capvalue)//放在外部中断里
{
    pCapSeq->cap_sequence <<= 2;
    pCapSeq->cap_sequence |= (capvalue & 0x03);
    pCapSeq->cap_cnt++;
    if(pCapSeq->cap_cnt >= 6)//采集满一个周期的序列，标志置1，关闭中断不再采集
    {
        pCapSeq->cap_cnt = 0;
        pCapSeq->capok_flag = 1;
        EX0 = 0;
        EX1 = 0;
    }   
}
void PhaseSeq_Cap_B(CapSeq_TypeDef* pCapSeq,u8 capvalue)//放在外部中断里
{
    pCapSeq->cap_sequence <<= 2;
    pCapSeq->cap_sequence |= (capvalue & 0x03);
    pCapSeq->cap_cnt++;
    if(pCapSeq->cap_cnt >= 6)//采集满一个周期的序列，标志置1，关闭中断不再采集
    {
        pCapSeq->cap_cnt = 0;
        pCapSeq->capok_flag = 1;
        EX0 = 0;
        EX1 = 0;
    }   
}
/********************* INT0 *************************/
void Ext_INT0 (void) interrupt INT0_VECTOR		
{
    u8  value_cap;
    EA = 0;
    value_cap = (P3 & 0x0c)>>2;
    PhaseSeq_Cap_A(&Cap_Seq,value_cap);
    EA = 1;

}

/********************* INT1*************************/
void Ext_INT1 (void) interrupt INT1_VECTOR
{
    u8  value_cap;
    EA = 0;
    value_cap = (P3 & 0x0c)>>2;
    PhaseSeq_Cap_B(&Cap_Seq,value_cap);
    EA = 1;
    
}

    