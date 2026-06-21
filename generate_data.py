import csv
import random
import math

# 설정값
SAMPLE_RATE = 10    # 10ms 단위 (100Hz)
TOTAL_SAMPLES = 500 # 5초 분량의 데이터

def generate_noise(amplitude):
    return random.uniform(-amplitude, amplitude);

def create_csv(filename, scenario_type):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Time_ms", "Accel_X_G", "Accel_Y_G", "Accel_Z_G"])

        stuck_value = random.uniform(1.5, 3.0)  # Stuck-at 고장 시 고정될 값

        for i in range(TOTAL_SAMPLES):
            time_ms = i * SAMPLE_RATE

            # 기본 주행 노이즈 (Z축: 중력가속도 1G 기본 반영)
            accel_x = generate_noise(0.1)
            accel_y = generate_noise(0.1)
            accel_z = 1.0 + generate_noise(0.1)

            if scenario_type == "A_NORMAL":
                pass

            elif scenario_type == "B_CRASH":
                # 2000ms(2초) 부근에서 정면 충돌 발생 (X축 급격한 음수 가속도, Y/Z축 흔들림)
                if 2000 <= time_ms <= 2150:
                    accel_x = -15.0 + generate_noise(2.0)   # 최대 15G의 충격
                    accel_y = generate_noise(3.0)
                    accel_z = 1.0 + generate_noise(5.0)

            elif scenario_type == "C_FAULT_STUCK":
                # 1500ms 이후 X축 센서 값이 특정 수치로 고장(Stuck-at)
                if time_ms >= 1500:
                    accel_x = stuck_value
                    accel_y = generate_noise(0.1)

            elif scenario_type == "C_FAULT_RANGE":
                # 2500ms에 물리적 한계치(+-16G)를 아득히 초과하는 쓰레기 값 발생
                if time_ms == 2500:
                    accel_x = 999.99
                    accel_y = -999.99
            
            # 소수점 4자리까지 포맷팅
            writer.writerow([time_ms, f"{accel_x:.4f}", f"{accel_y:.4f}", f"{accel_z:.4f}"])

    print(f"[{scenario_type}] 데이터 생성 완료 -> {filename}")

if __name__ == "__main__":
    create_csv("data/case_a_normal.csv", "A_NORMAL")
    create_csv("data/case_b_crash.csv", "B_CRASH")
    create_csv("data/case_c_stuck.csv", "C_FAULT_STUCK")
    create_csv("data/case_c_range.csv", "C_FAULT_RANGE")
    print("모든 가상 센서 데이터 생성이 완료되었습니다.")