#include "../include/logic.h"
#include <math.h>

void Logic_Init(DeployLogicState* logicState, AcuStatus* acuStatus) {
    logicState->delta_v = 0.0;
    logicState->last_accel_x = 0.0;
    logicState->stuck_counter = 0;

    acuStatus->currentState = STATE_NORMAL;
    acuStatus->currentError = ERR_NONE;
    acuStatus->isTriggered = false;
}

bool Logic_CheckFailSafe(AcuStatus* acuStatus, DeployLogicState* logicState, const ImuData* rawData) {
    // 1. Out of Range (범위 초과 고장) 검사
    if (fabs(rawData->accel_x) > THRESHOLD_OUT_OF_RANGE ||
        fabs(rawData->accel_y) > THRESHOLD_OUT_OF_RANGE ||
        fabs(rawData->accel_z) > THRESHOLD_OUT_OF_RANGE) {
        
        acuStatus->currentState = STATE_FAULT;
        acuStatus->currentError = ERR_OUT_OF_RANGE;
        return true; // 고장 발생 (true)
    }

    // 2. Stuck-at (고착 고장) 검사 - 값이 미세하게도 변하지 않는지 확인
    if (fabs(rawData->accel_x - logicState->last_accel_x) < 0.0001) {
        logicState->stuck_counter++;
        if (logicState->stuck_counter >= STUCK_AT_TIME_LIMIT) {
            acuStatus->currentState = STATE_FAULT;
            acuStatus->currentError = ERR_STUCK_AT;
            return true;
        }
    } else {
        logicState->stuck_counter = 0; // 값이 변하면 카운터 초기화
    }
    
    logicState->last_accel_x = rawData->accel_x;
    return false; // 정상 상태 (false)
}

void Logic_EvaluateDeployment(AcuStatus* acuStatus, DeployLogicState* logicState, double filtered_x, double raw_y) {
    // 1. Delta-V 적분 (차량의 감속 방향인 음수일 때만 누적)
    if (filtered_x < 0) {
        logicState->delta_v += filtered_x;
    } else {
        // 감속이 끝나고 양수가 되면 누적된 에너지를 서서히 방전시킴 (Leakage Factor)
        logicState->delta_v *= 0.9; 
    }

    // 2. Arming (이중 안전) 조건: X축이 전개 수준이거나, Y축이 순간적으로 크게 흔들리는지 교차 검증
    bool isArmed = (filtered_x < THRESHOLD_ARMING) || (fabs(raw_y) > fabs(THRESHOLD_ARMING));

    // 3. 최종 에어백 전개 판단 논리 (AND 조건)
    if ((logicState->delta_v <= THRESHOLD_DELTA_V) && isArmed) {
        acuStatus->currentState = STATE_DEPLOYED;
        acuStatus->isTriggered = true;
    }
}