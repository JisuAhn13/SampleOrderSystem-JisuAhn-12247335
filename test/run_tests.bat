@echo off
chcp 65001 > nul
if not exist output mkdir output

set EXE=..\build\Debug\SampleOrderSystem.exe

echo.
echo ========================================
echo  SampleOrderSystem Dummy Input Tests
echo ========================================
echo.

echo [1/5] 재고 충분 경로  (시료등록 - 주문접수 - 승인 - 출고)
type dummy\scenario1.txt | %EXE% > output\scenario1_out.txt 2>&1
echo      완료 -^> output\scenario1_out.txt

echo.
echo [2/5] 재고 부족 + 생산  (PRODUCING 후 생산 현황 확인)
type dummy\scenario2.txt | %EXE% > output\scenario2_out.txt 2>&1
echo      완료 -^> output\scenario2_out.txt

echo.
echo [3/5] 주문 거절  (REJECTED 후 모니터링에서 미표시)
type dummy\scenario3.txt | %EXE% > output\scenario3_out.txt 2>&1
echo      완료 -^> output\scenario3_out.txt

echo.
echo [4/5] FIFO 큐 순서  (3건 주문 승인 후 대기 큐 확인)
type dummy\scenario4.txt | %EXE% > output\scenario4_out.txt 2>&1
echo      완료 -^> output\scenario4_out.txt

echo.
echo [5/5] 엣지 케이스  (잘못된 입력 처리)
type dummy\edge_cases.txt | %EXE% > output\edge_cases_out.txt 2>&1
echo      완료 -^> output\edge_cases_out.txt

echo.
echo ========================================
echo  완료! output\ 폴더를 확인하세요.
echo ========================================
echo.
dir output\*.txt /b
pause
