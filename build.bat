clang++ -target x86_64-pc-windows-gnu ./ans.cpp -O3 -D LOCAL_DEBUG
java -jar tester.jar -exec a.exe -seed 3 -size 6 -delay 5
