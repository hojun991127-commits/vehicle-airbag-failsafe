#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/acu_types.h"
#include "../include/filter.h"
#include "../include/logic.h"

int main(int argc, char* argv[]) {
    // 커맨드 라인 인자로 CSV 파일 경로를 받음
    if (argc < 2) {
        printf("사용법: %s <csv_파일_경로>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("에러: '%s' 파일을 열 수 없습니다.\n", argv[1]);
        return 1;
    }

    MovingAverageFilter filterX;
    DeployLogicState logicState;
    AcuStatus acuStatus;

    // 시스템 초기화
    Filter_Init(&filterX);
    Logic_Init(&logicState, &acuStatus);

    char line[256];
    fgets(line, sizeof(line), file); // 첫 줄(헤더) 무시

    printf("==========================================\n");
    printf(" ACU(Airbag Control Unit) 시뮬레이션 시작\n");
    printf(" 대상 파일: %s\n", argv[1]);
    printf("==========================================\n");

    ImuData rawData;
    while (fgets(line, sizeof(line), file)) {
        // CSV 쉼표 구분자 파싱
        if (sscanf(line, "%lf,%lf,%lf,%lf", &rawData.time_ms, &rawData.accel_x, &rawData.accel_y, &rawData.accel_z) == 4) {

            // 1. Fail-Safe 진단 로직 수행
            if (Logic_CheckFailSafe(&acuStatus, &logicState, &rawData)) {
                printf("[Time: %.0f ms] 🚨 FAULT DETECTED: ", rawData.time_ms);
                if (acuStatus.currentError == ERR_OUT_OF_RANGE) printf("센서 범위 초과 노이즈 (Out of Range)\n");
                if (acuStatus.currentError == ERR_STUCK_AT) printf("센서 값 고착 오류 (Stuck-at)\n");
                printf("  => Fail-Safe 작동! 에어백 전개 회로를 강제 차단(Symptom Lock)합니다.\n");
                break; // 에러 발생 시 즉각 시스템 셧다운 (시뮬레이션 종료)
            }

            // 2. 신호 처리 (X축 이동 평균 필터 적용)
            double filtered_x = Filter_Update(&filterX, rawData.accel_x);

            // 3. 전개 알고리즘 수행
            Logic_EvaluateDeployment(&acuStatus, &logicState, filtered_x, rawData.accel_y);

            // 에어백 전개 신호(Trigger)가 발생하면 즉시 출력 후 종료
            if (acuStatus.isTriggered) {
                printf("[Time: %.0f ms] 💥 CRASH DETECTED! 에어백 전개 (Deploy)!\n", rawData.time_ms);
                printf("  => Delta-V 누적치: %.2f, 필터링된 X축 가속도: %.2f G\n", logicState.delta_v, filtered_x);
                break; 
            }
        }
    }

    if (!acuStatus.isTriggered && acuStatus.currentState == STATE_NORMAL) {
        printf("시뮬레이션 종료: 정상 주행 완료 (에어백 전개 안 됨)\n");
    }

    printf("==========================================\n\n");
    fclose(file);
    return 0;
}