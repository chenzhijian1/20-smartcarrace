#include "headfile.h"

uint16 ad_result = 0;           //���ŵ�ѹ
uint16 battery_voltage = 0;     //��ص�ѹ

uint32 temp;

void voltage_init() {
	adc_init(ADC_P15, ADC_SYSclk_DIV_32);
}

uint16 read_voltage() {
	ad_result = adc_once(ADC_P15, ADC_12BIT);
	temp = (((uint32)ad_result * 3300) / 4096);  //�������ǰadc���ŵĵ�ѹ ���㹫ʽΪ ad_result*VCC/ADC�ֱ���    VCC��λΪmv
	battery_voltage =  temp * 11;//�������ŵ�ѹ  �ͷ�ѹ�������ֵ�����ص�ѹ ���㹫˾Ϊ   ���ŵ�ѹ*(R2+R3)/R3   R3Ϊ�ӵ�
	
	return battery_voltage;
}