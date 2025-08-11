#include "inductance.h"

// ��в�ȺͲ�ϵ�� ����������֮��Ĳ��ˣ����ֻ�ܶ����ȫ�ֱ���float
#define A_ 1.2f
#define B_ 1.7f
#define C_ 0.5f

uint8 flag1 = 0;

/* ������� */
adc_struct aaddcc = {0}; // ������adc�ɼ��ṹ��

const uint16 MAX_ADC[NUM] = {2000, 2000, 2000, 2000, 2000};// ÿ�θ�����к���������Ҫ����
//float MAX_ADC[NUM] = {MAX_TRANSVERSE, MAX_PORTRAINT,3700,  MAX_TRANSVERSE, MAX_PORTRAINT};
uint16 AD_value[NUM][10];  
//uint16 AD_value[NUM][4];    // ԭʼ�ɼ������
uint16 ad_ave[NUM] = {0};     // �˲������ĵ��
float AD_ONE[NUM] = {0};      // ��һ����ĵ��
// uint16 middle[3] = {0};
float adc_left_dir = 0.0;
float adc_right_dir = 0.0;

void direction_adc_init(void) {// ��У�ADC����ʼ��
    adc_init(ADC_P00, ADC_SYSclk_DIV_2); //  in1�Һ�
    adc_init(ADC_P01, ADC_SYSclk_DIV_2); // in2��ǰ

    adc_init(ADC_P05, ADC_SYSclk_DIV_2); //  in3���

    adc_init(ADC_P06, ADC_SYSclk_DIV_2); //  in4��ǰ
    adc_init(ADC_P13, ADC_SYSclk_DIV_2); //  in5�к�
}

void cleanADC(void) {
    uint8 i;
    for (i = 0; i < NUM; i++)
        ad_ave[i] = 0;
}

//--------------------------------ADC
// ADC�ɼ����ݣ���һ������Ⱥ����error����  
void direction_adc_get(void)
{
    int i;
    cleanADC();

    for (i = 0; i < 10; i++) {// 6·��У�ÿ·�ɼ�5��ֵ����һ�δ���
        AD_value[0][i] = adc_once(ADC_P05, ADC_12BIT); // ���
        AD_value[1][i] = adc_once(ADC_P06, ADC_12BIT); // ����

        AD_value[2][i] = adc_once(ADC_P13, ADC_12BIT); // �к���

        AD_value[3][i] = adc_once(ADC_P00, ADC_12BIT); // �Һ�
        AD_value[4][i] = adc_once(ADC_P01, ADC_12BIT); // ����
    }

    for (i = 0; i < 10; i++) {
        ad_ave[0] += AD_value[0][i];
        ad_ave[1] += AD_value[1][i];
        ad_ave[2] += AD_value[2][i];
        ad_ave[3] += AD_value[3][i];
        ad_ave[4] += AD_value[4][i];
    }
    
    for (i = 0; i < NUM; i++) {
        ad_ave[i] /= 10;
        if (ad_ave[i] > MAX_ADC[i])  ad_ave[i] = MAX_ADC[i];
    }

    // ��һ�� normalization
    for (i = 0; i < NUM; i++)
        AD_ONE[i] = 100 * (float)ad_ave[i] / MAX_ADC[i];
	
    // �����ж� �ĵ��
    // if (flag == 0 && AD_ONE[0] >= 20 && AD_ONE[3] >= 20) { //����
    //     encoder_temp = encoder_ave;
		
		
    //     if (AD_ONE[1] >= AD_ONE[4] * 2 && AD_ONE[1] >= 30) {
	// 		flag = 2;
	// 		flag1 = 1;  //���Ʊ�����ֵֻ��¼��һ�μ�⵽����
	// 		flag_huandao = 0;
	// 	}
    //     if (AD_ONE[4] >= AD_ONE[1] * 2 && AD_ONE[4] >= 30) {
	// 		flag = 2;
	// 		flag1 = 1;  //���Ʊ�����ֵֻ��¼��һ�μ�⵽����
	// 		flag_huandao = 1;
	// 	}
    // }
    
    // �����ж� ����
    // if (flag == 0 && flag1 == 0 && AD_ONE[2] >= 60) {
    // if (flag == 0 && ((AD_ONE[0] >= 25 && AD_ONE[3] >= 25 && AD_ONE[2] >= 35 && (AD_ONE[1] <= 20 || AD_ONE[4] <= 20)))) {
    // if (flag == 0 && ((AD_ONE[0] + AD_ONE[3]) >= 40.0f)) {
	// 	encoder_temp = encoder_ave;
	// 	flag = 1;
    // }

    // ��Ⱥͼ���
    aaddcc.last_err_dir = aaddcc.err_dir;

    if (AD_ONE[0] + AD_ONE[1] + AD_ONE[3] + AD_ONE[4] < 6)
        aaddcc.err_dir = aaddcc.last_err_dir;

    // else {
    //     adc_left_dir = sqrt(A_ * AD_ONE[0] * AD_ONE[0] + B_ * AD_ONE[1] * AD_ONE[1]);
	// 	adc_right_dir = sqrt(A_ * AD_ONE[3] * AD_ONE[3] + B_ *   AD_ONE[4] * AD_ONE[4]);
	// 	//adc_left_dir = sqrt(AD_ONE[0] * AD_ONE[0]);
	// 	//adc_right_dir = sqrt(AD_ONE[4] * AD_ONE[4]); 
                  
    //     aaddcc.err_dir = 50 * ((adc_left_dir - adc_right_dir) / (adc_left_dir + adc_right_dir));
    // }
    
	else {
        // if (flag_key_fast == 1)
		//     aaddcc.err_dir = 30 * (1.2 * (AD_ONE[0] - AD_ONE[3]) + 1.7 * (AD_ONE[1] - AD_ONE[4])) /
		// 					  (1.2 * (AD_ONE[0] + AD_ONE[3]) + 0.5 * fabs(AD_ONE[1] - AD_ONE[4]));
        // else
            aaddcc.err_dir = 30 * (1.0 * (AD_ONE[0] - AD_ONE[3]) + 1.0 * (AD_ONE[1] - AD_ONE[4])) /
                              (1.0 * (AD_ONE[0] + AD_ONE[3]) + 1.0 * fabs(AD_ONE[1] - AD_ONE[4]));
	}
    
    // ʮ�ֲ���
    // if (flag == 0 && AD_ONE[2] < 13) 
    //     aaddcc.err_dir *= (1 + (13 - AD_ONE[2]) * 0.1);
}
