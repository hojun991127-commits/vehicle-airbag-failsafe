#include "../include/filter.h"

void Filter_Init(MovingAverageFilter* filter) {
    filter->index = 0;
    filter->sum = 0.0;
    filter->count = 0;
    for (int i = 0; i < FILTER_WINDOW_SIZE; i++) {
        filter->buffer[i] = 0.0;
    }
}

double Filter_Update(MovingAverageFilter* filter, double new_value) {
    // 1. 가장 오래된 데이터를 sum에서 빼기
    filter->sum -= filter->buffer[filter->index];
    
    // 2. 새로운 데이터를 배열에 덮어쓰고 sum에 더하기
    filter->buffer[filter->index] = new_value;
    filter->sum += new_value;

    // 3. 인덱스를 다음 칸으로 이동 (배열 끝에 도달하면 0으로 순환)
    filter->index = (filter->index + 1) % FILTER_WINDOW_SIZE;

    // 4. 초기 상태에서 버퍼가 꽉 차지 않았을 때의 예외 처리
    if (filter->count < FILTER_WINDOW_SIZE) {
        filter->count++;
        return filter->sum / filter->count;
    }

    // 5. 버퍼가 꽉 찼을 경우 전체 크기로 나누어 평균 반환
    return filter->sum / FILTER_WINDOW_SIZE;
}