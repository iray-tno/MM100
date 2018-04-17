clang++ -target x86_64-pc-windows-gnu ./ans.cpp -O3

powershell -C (Measure-Command {"((Get-Content ./input.txt | ./a.exe > hoge.txt))"}).TotalMilliseconds
