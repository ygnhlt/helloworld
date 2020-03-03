#include "key.h"
#include "led.h"
#include "usart.h"
#include "relay.h"
#include "TG.h"


extern Pro_flag_s MCU_flag;

void key_init(void)
{
    PA_DDR_DDR3=0;
    PA_CR1_C13=1;

    PF_DDR_DDR4=0;
    PF_CR1_C14=1;

    PB_DDR_DDR7=0;
    PB_CR1_C17=1;

    PB_DDR_DDR6=0;
    PB_CR1_C16=1;
    
}




u8 key_get(void)
{
  
    u8 temp=0;
    if(MCU_flag.Keys==0)
    {
       if(Key1_re==0)
            temp|=0x02;
        if(Key2_re==0)
            temp|=0x01;
        if(Key3_re==0)
            temp|=0x04;
        if(Key4_re==0)
            temp|=0x08;
    }
    else if(MCU_flag.Keys==1)
    {
       if(Key1_re==0)
            temp|=0x01;
        if(Key2_re==0)
            temp|=0x02;
        if(Key3_re==0)
            temp|=0x02;
        if(Key4_re==0)
            temp|=0x08;
    }
    else
    {
        if(Key1_re==0)
            temp|=0x01;
        if(Key2_re==0)
            temp|=0x02;
        if(Key3_re==0)
            temp|=0x04;
        if(Key4_re==0)
            temp|=0x08;
    }
    return temp;
}

u8 	key_reg;				
u16	key_count;				
u8	key_reg_out;
u8 GetKey_mode(void)
{
	u8 i;////,temp_count,j,k;
	u8 key_mode;
	u16  temp;
	u16  key_temp=0;
	u16  key_temp0=0;
	//u16  key_temp1=0;
	//********key get sub**********************
	key_temp0=(u16)key_get();
	
	if(key_temp0>0)
	{
		key_temp=key_temp0;
	}
	else
	{	
		//key_temp=key_temp1;
		key_temp=0;
	}
	//key_temp=0x04;
	//*****************************************
	if(key_temp>0)
	{
		if(key_reg==0)
		{
			for(i=0;i<16;i++)
			{
				if(key_temp& (1<<i))
				{
					if(key_temp0>0)
						key_reg=i+1;
					else
						key_reg=i+17;
					break;
				}
			}
		}
		else
		{
			if(key_temp0>0)
				temp=key_temp&(1<<(key_reg-1));
			else
				temp=key_temp&(1<<(key_reg-17));
			if(temp>0)
			{
				key_count++;
				if((key_count>key_short_down)&(key_long_down>key_count))
				{
					key_mode=1;
					key_reg_out=key_reg;
				}
				else if(key_count>key_long_down)
				{
					key_mode=2;
					key_reg_out=key_reg;
				}
			}
			else
			{
				if((key_count>key_short_down)&(key_long_down>key_count))
				{
					key_mode=3;
					key_reg_out=key_reg;
				}
				else if(key_count>key_long_down)
				{
					key_mode=4;
					key_reg_out=key_reg;
				}
				key_count=0;
				key_reg=0;
			}
		}
	}
	else
	{
		if(key_reg>0)
		{
			if((key_count>key_short_down)&(key_long_down>key_count))
			{
				key_mode=3;
				key_reg_out=key_reg;
			}
			else if(key_count>key_long_down)
			{
				key_mode=4;
				key_reg_out=key_reg;
			}
			key_count=0;
			key_reg=0;
		}
		
	}
	return key_mode;
}
extern u16 Clear_num;



u8 opare_flag;
void key_work(uchar key_g,uchar key_mode_g)
{
  // u8 order;
    
	if(key_mode_g==1)
	{
		if((opare_flag&0x01)==0)
		{
			//short down
			//-----
			
            TG_work5(key_g);
            ledHuXi_set(key_g,100);
            Key_send(key_g);
			//-----
			opare_flag|=0x01;
		}			
	}
	else if(key_mode_g==2)
	{
		if((opare_flag&0x10)==0)
		{
			//long down
			//-----
			//MCU_flag.keys_long=key_g;
		    ledHuXi_set(key_g,100);
            
			//-----
			opare_flag|=0x10;
		}
	}
	else if(key_mode_g==3)
	{
		//short up
		//-----
		
		ledHuXi_set(key_g,0);
		//-----
		opare_flag=0x00;
	}
	else if(key_mode_g==4)
	{
		//long up
		//-----
		//TG_work5_lv(MCU_flag.keys_long);
		//MCU_flag.keys_long=0;
		ledHuXi_set(key_g,0);
		//-----
		opare_flag=0x00;
	}			
}


void key_sub(void)
{
    u8 temp=0;
    if(MCU_flag.sysMode==_sysMode_set)
        return;
    temp=GetKey_mode();
    if(key_reg_out>0)
    {
	    key_work(key_reg_out, temp);
	    key_reg_out=0;
    }
}



void TG_work(u8 keys)
{
    if(keys==1)
    {
        if(MCU_flag.TG_BF1<100)
            MCU_flag.TG_BF1++;
    }
    if(keys==2)
    {
        if(MCU_flag.TG_BF1>0)
            MCU_flag.TG_BF1--;
    }

    if(keys==4)
    {
        if(MCU_flag.TG_BF2<100)
            MCU_flag.TG_BF2++;
    }
    if(keys==3)
    {
        if(MCU_flag.TG_BF2>0)
            MCU_flag.TG_BF2--;
    }
}
///按照等级分配调光，方便微信控制
void TG_Fen(u8 zhi,u8 num)
{
    //u8 temp[6]={0,45,55,65,80,110};
    u8 temp[6]={0,60,80,110,120,130};
    if(zhi>5)
        return;
    if(num==1)
        MCU_flag.TG_BF1=temp[zhi];
    if(num==2)
        MCU_flag.TG_BF2=temp[zhi];
}

u8 tg1_lv=0;
u8 tg2_lv=0;
/*
void TG_work5(u8 keys)
{
    //u8 temp[6]={0,45,55,65,80,110};
    u8 temp[6]={0,60,70,90,100,130};
    if(keys==1)
    {
        if(tg1_lv<4)
            tg1_lv++;
        MCU_flag.TG_BF1=temp[tg1_lv];
    }
    if(keys==2)
    {
        if(tg1_lv>0)
            tg1_lv--;
        MCU_flag.TG_BF1=temp[tg1_lv];
    }

    if(keys==4)
    {
        if(tg2_lv<3)
            tg2_lv++;
        MCU_flag.TG_BF2=temp[tg2_lv];
    }
    if(keys==3)
    {
        if(tg2_lv>0)
            tg2_lv--;
        MCU_flag.TG_BF2=temp[tg2_lv];
    }
}
*/

void TG_work5(u8 keys)
{
    //u8 temp[6]={0,45,55,65,80,110};
    u8 temp[6]={0,0x80,0x58,0x30,100,130};
    if(keys==1)
    {
        if(tg1_lv<3)
            tg1_lv++;
        MCU_flag.TG1_Value=temp[tg1_lv];
    }
    else if(keys==2)
    {
        if(tg1_lv>0)
            tg1_lv--;
        MCU_flag.TG1_Value=temp[tg1_lv];
    }

    else if(keys==4)
    {
        if(tg2_lv<3)
            tg2_lv++;
        MCU_flag.TG2_Value=temp[tg2_lv];
    }
    else if(keys==3)
    {
        if(tg2_lv>0)
            tg2_lv--;
        MCU_flag.TG2_Value=temp[tg2_lv];
    }

    if(MCU_flag.TG1_Value>0)
        MCU_flag.TG1_sta=1;
    else
        MCU_flag.TG1_sta=0;

    if(MCU_flag.TG2_Value>0)
        MCU_flag.TG2_sta=1;
    else
        MCU_flag.TG2_sta=0;
}

void TG_work5_lv(u8 keys)
{
    //u8 temp[6]={0,45,55,65,80,110};
    u8 temp[6]={0,60,80,110,120,130};
    if((keys==1)|(keys==2))
    {
        if(MCU_flag.TG_BF1==0)
            tg1_lv=0;
        else if((MCU_flag.TG_BF1>=temp[0])&(MCU_flag.TG_BF1<temp[1]))
            tg1_lv=1;
        else if((MCU_flag.TG_BF1>=temp[1])&(MCU_flag.TG_BF1<temp[2]))
            tg1_lv=2;
        else if((MCU_flag.TG_BF1>=temp[2])&(MCU_flag.TG_BF1<temp[3]))
            tg1_lv=3;
        else if((MCU_flag.TG_BF1>=temp[3])&(MCU_flag.TG_BF1<temp[4]))
            tg1_lv=4;
        else if((MCU_flag.TG_BF1>=temp[4])&(MCU_flag.TG_BF1<temp[5]))
            tg1_lv=5;
    }
    else if((keys==3)|(keys==4))
    {
        if(MCU_flag.TG_BF2==0)
            tg2_lv=0;
        else if((MCU_flag.TG_BF2>=temp[0])&(MCU_flag.TG_BF2<temp[1]))
            tg2_lv=1;
        else if((MCU_flag.TG_BF2>=temp[1])&(MCU_flag.TG_BF2<temp[2]))
            tg2_lv=2;
        else if((MCU_flag.TG_BF2>=temp[2])&(MCU_flag.TG_BF2<temp[3]))
            tg2_lv=3;
        else if((MCU_flag.TG_BF2>=temp[3])&(MCU_flag.TG_BF2<temp[4]))
            tg2_lv=4;
        else if((MCU_flag.TG_BF2>=temp[4])&(MCU_flag.TG_BF2<temp[5]))
            tg2_lv=5;
    }
    
}

u16 TG_work5long_ji=0;
void TG_work5long_sub(u16 tm)
{
    
    TG_work5long_ji++;
    if(TG_work5long_ji<tm)
        return;
    TG_work5long_ji=0;
    if(MCU_flag.keys_long>0)
    {
        TG_work5(MCU_flag.keys_long);
    }
}

u16 TG_worklong_ji=0;

void TG_worklong_sub(u16 tm)
{
     
    TG_work5long_ji++;
    if(TG_work5long_ji<tm)
        return;
    TG_work5long_ji=0;
    if(MCU_flag.keys_long>0)
    {
        if(MCU_flag.keys_long==1)
        {
            if(MCU_flag.TG_BF1<100)
                MCU_flag.TG_BF1++;
   
            
        }
        if(MCU_flag.keys_long==2)
        {
            if(MCU_flag.TG_BF1>0)
                MCU_flag.TG_BF1--;
            
        }

        if(MCU_flag.keys_long==4)
        {
            if(MCU_flag.TG_BF2<100)
                MCU_flag.TG_BF2++;
            
        }
        if(MCU_flag.keys_long==3)
        {
            if(MCU_flag.TG_BF2>0)
                MCU_flag.TG_BF2--;
           
        }
    }
}


/////f1+Kwork+调光等级+对应路+nc+校验 
void KeyWork(u8 Kwork,u8 order,u8 zu,u8 quyu,u8 keyNum,u8 Cj)
{
    u8 i,sta,num;
    sta=order;
    num=zu;
    u8 keyi=MCU_flag.Keys+1;
    if(Kwork==_KWork_ZongGuan)
    {   
        for(i=0;i<keyi;i++)
        {
            if(((MCU_flag.key_Quan[i]&_KAuthority_ZongKong)>0)&((quyu&0xf0)==(MCU_flag.key_QuYu[i]&0xf0)))
            {
                if(MCU_flag.key_work[i]==_KWork_DengKong)
                {
                    if((i==0)|(i==1))
                        //MCU_flag.TG_BF1=0;
                        {tg_try(1,0);tg1_lv=0;}
                    else if((i==2)|(i==3))
                        //MCU_flag.TG_BF2=0;
                        {tg_try(2,0);tg2_lv=0;}
                       
                }
                    
            }
           
        }
        
    }
    else if(Kwork==_KWork_ZongKaiGuan)
    {
        
        for(i=0;i<keyi;i++)
        {
            if(((MCU_flag.key_Quan[i]&_KAuthority_ZongKong)>0)&((quyu&0xf0)==(MCU_flag.key_QuYu[i]&0xf0)))
            {
                if(sta==0)
                {
                   if((i==0)|(i==1))
                        //MCU_flag.TG_BF1=0;
                        {tg_try(1,0);tg1_lv=0;}
                    else if((i==2)|(i==3))
                        //MCU_flag.TG_BF2=0;
                        {tg_try(2,0);tg2_lv=0;}
                }
                else
                {
                    
                    
                    if((i==0)|(i==1))
                    {
                        //MCU_flag.TG_BF1=_TG_value;
                        //TG_work5_lv(1);    
                        tg_try(1,0x30);
                        tg1_lv=3;
                    }
                    else if((i==2)|(i==3))
                    {
                        //MCU_flag.TG_BF2=_TG_value;
                        //TG_work5_lv(3);
                        tg_try(2,0x30);
                        tg2_lv=3;
                    }
                   
                }
            }
        }
    }
    else if(Kwork==_KWork_ChangJing)
    {
        
        if(Cj>0)
        {
            if((sta>0)&(Cj==0x02))
            {//柔和模式控制调光效果
                tg_try(1,0x80);
                tg_try(2,0x80);
                tg1_lv=1;
                tg2_lv=1;
            }
            else if((sta>0)&(Cj==0x01))
            {//明亮模式控制调光效果
                tg_try(1,0x30);
                tg_try(2,0x30);
                tg1_lv=3;
                tg2_lv=3;
            }
            else
            {
                tg_try(1,0);
                tg_try(2,0);
                tg1_lv=0;
                tg2_lv=0;
            }
            /*
            for(i=0;i<keyi;i++)
            {
                if(((Cj&MCU_flag.key_Cj[i])>0)&((quyu&0x0f)==(MCU_flag.key_QuYu[i]&0x0f)))
                {
                    if(sta==0)
                    {
                        if((MCU_flag.key_Quan[i]&_KAuthority_NoClose)==0)
                        {
                            if((i==0)|(i==1))
                                MCU_flag.TG_BF1=0;
                            else if((i==2)|(i==3))
                                MCU_flag.TG_BF2=0;
                        }
                    }
                    else
                    {
                        if((i==0)|(i==1))
                        {
                            MCU_flag.TG_BF1=_TG_value;
                            TG_work5_lv(1);
                        }
                        else if((i==2)|(i==3))
                        {
                            MCU_flag.TG_BF2=_TG_value;
                            TG_work5_lv(3);
                        }
                        
                    }
                }
                else if(((Cj&MCU_flag.key_Cj[i])==0)&((quyu&0x0f)==(MCU_flag.key_QuYu[i]&0x0f)))
                {
                    if(sta==0)
                    {
                       
                    }
                    else
                    {
                        if((i==0)|(i==1))
                            MCU_flag.TG_BF1=0;
                        else if((i==2)|(i==3))
                            MCU_flag.TG_BF2=0;
                        
                    }
                }
            }
            */
            
        }
    }
    else if((Kwork==_KWork_DengKong)|(Kwork==_KWork_YeDeng))
    {
        
        
    }
    else if((Kwork==_KWork_TiaoGuang1ON)|(Kwork==_KWork_TiaoGuang2ON)|(Kwork==_KWork_TiaoGuang3ON)|(Kwork==_KWork_TiaoGuang4ON))
    {
        if(Kwork==MCU_flag.key_work[0])
            TG_Fen(sta,num);
    }
  
    
}





