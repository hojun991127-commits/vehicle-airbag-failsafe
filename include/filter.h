#ifndef FILTER_H
#define FILTER_H

#include "acu_types.h"

#define FILTER_WINDOW_SIZE 10   // 10개의 샘플을 평균내어 노이즈 제거

// 정적 링 버퍼 기반 이동 평균 필터 구조체
typedef struct{
    double buffer[FILTER_WINDOW_SIZE];
    int index;
    double sum;
    int count;  // 초기 버퍼가 덜 찼을 때를 대비한 카운트
} MovingAverageFilter;

// 필터 함수 프로토타입
void Filter_Init(MovingAverageFilter* filter);
double Filter_Update(MovingAverageFilter* filter, double new_value);

#endif