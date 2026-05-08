@echo off
chcp 65001 > nul
echo ========================================
echo  SampleOrderSystem Dummy Input Tests
echo ========================================

set EXE=..\build\Debug\SampleOrderSystem.exe
set DUMMY=dummy

echo.
echo [시나리오 1] 재고 충분 경로...
type %DUMMY%\scenario1.txt | %EXE% > output\scenario1_out.txt 2>&1
echo 완료 - output\scenario1_out.txt 확인

echo.
echo [시나리오 2] 재고 부족 + 생산 경로...
type %DUMMY%\scenario2.txt | %EXE% > output\scenario2_out.txt 2>&1
echo 완료 - output\scenario2_out.txt 확인

echo.
echo [시나리오 3] 주문 거절 경로...
type %DUMMY%\scenario3.txt | %EXE% > output\scenario3_out.txt 2>&1
echo 완료 - output\scenario3_out.txt 확인

echo.
echo [시나리오 4] FIFO 큐 순서...
type %DUMMY%\scenario4.txt | %EXE% > output\scenario4_out.txt 2>&1
echo 완료 - output\scenario4_out.txt 확인

echo.
echo [엣지 케이스] 잘못된 입력 처리...
type %DUMMY%\edge_cases.txt | %EXE% > output\edge_cases_out.txt 2>&1
echo 완료 - output\edge_cases_out.txt 확인

echo.
echo ========================================
echo  모든 테스트 완료! output\ 폴더를 확인하세요.
echo ========================================
