@echo off
powershell -C ((Measure-Command {"((java -jar tester.jar -seed 3 -exec a.exe -novis > temp/score.txt))"}).TotalMilliseconds) > temp/time.txt

mkdir temp 2> nul

type .\temp\score.txt > .\temp\score2.txt
for /f "tokens=1,2,3" %%a in (.\temp\score2.txt) do (
    set SCORE=%%c
)

for /f %%a in (.\temp\time.txt) do (
    set TIME=%%a
)

echo score: %SCORE%, time: %TIME%
echo %SCORE%, %TIME%>> results.txt

del temp /Q
@echo on
