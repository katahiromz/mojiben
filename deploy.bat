set VERSION=3.8
set README=README.txt
set READMEJA=README_ja.txt
set LICENSE=LICENSE.txt
set HISTORY=HISTORY.txt
set NAME1=mojiben-%VERSION%-part1
set NAME2=mojiben-%VERSION%-part2
set NAME3=mojiben-%VERSION%-part3
set NAME4=mojiben-%VERSION%-part4
set NAME5=mojiben-%VERSION%-part5
set NAME6=mojiben-%VERSION%-part6
set NAME7=mojiben-%VERSION%-part7
set NAME8=mojiben-%VERSION%-part8
set NAME9=mojiben-%VERSION%-part9
if not exist "build\Release\FuriganaCtl.dll" echo "No FuriganaCtl.dll found" && exit 1
if not exist "build\Release\mojiben1.exe" echo "No mojiben1.exe found" && exit 1
if not exist "build\Release\mojiben2.exe" echo "No mojiben2.exe found" && exit 1
if not exist "build\Release\mojiben3.exe" echo "No mojiben3.exe found" && exit 1
if not exist "build\Release\mojiben4.exe" echo "No mojiben4.exe found" && exit 1
if not exist "build\Release\mojiben5.exe" echo "No mojiben5.exe found" && exit 1
if not exist "build\Release\mojiben6.exe" echo "No mojiben6.exe found" && exit 1
if not exist "build\Release\mojiben7.exe" echo "No mojiben7.exe found" && exit 1
if not exist "build\Release\mojiben8.exe" echo "No mojiben8.exe found" && exit 1
if not exist "build\Release\mojiben9.exe" echo "No mojiben9.exe found" && exit 1
if not exist "%NAME1%" mkdir "%NAME1%"
if not exist "%NAME2%" mkdir "%NAME2%"
if not exist "%NAME3%" mkdir "%NAME3%"
if not exist "%NAME4%" mkdir "%NAME4%"
if not exist "%NAME5%" mkdir "%NAME5%"
if not exist "%NAME6%" mkdir "%NAME6%"
if not exist "%NAME7%" mkdir "%NAME7%"
if not exist "%NAME8%" mkdir "%NAME8%"
if not exist "%NAME9%" mkdir "%NAME9%"
copy "%README%" "%NAME1%"
copy "%README%" "%NAME2%"
copy "%README%" "%NAME3%"
copy "%README%" "%NAME4%"
copy "%README%" "%NAME5%"
copy "%README%" "%NAME6%"
copy "%README%" "%NAME7%"
copy "%README%" "%NAME8%"
copy "%README%" "%NAME9%"
copy "%READMEJA%" "%NAME1%"
copy "%READMEJA%" "%NAME2%"
copy "%READMEJA%" "%NAME3%"
copy "%READMEJA%" "%NAME4%"
copy "%READMEJA%" "%NAME5%"
copy "%READMEJA%" "%NAME6%"
copy "%READMEJA%" "%NAME7%"
copy "%READMEJA%" "%NAME8%"
copy "%READMEJA%" "%NAME9%"
copy "%LICENSE%" "%NAME1%"
copy "%LICENSE%" "%NAME2%"
copy "%LICENSE%" "%NAME3%"
copy "%LICENSE%" "%NAME4%"
copy "%LICENSE%" "%NAME5%"
copy "%LICENSE%" "%NAME6%"
copy "%LICENSE%" "%NAME7%"
copy "%LICENSE%" "%NAME8%"
copy "%LICENSE%" "%NAME9%"
copy "%HISTORY%" "%NAME1%"
copy "%HISTORY%" "%NAME2%"
copy "%HISTORY%" "%NAME3%"
copy "%HISTORY%" "%NAME4%"
copy "%HISTORY%" "%NAME5%"
copy "%HISTORY%" "%NAME6%"
copy "%HISTORY%" "%NAME7%"
copy "%HISTORY%" "%NAME8%"
copy "%HISTORY%" "%NAME9%"
copy "build\Release\mojiben1.exe" "%NAME1%"
copy "build\Release\mojiben2.exe" "%NAME2%"
copy "build\Release\mojiben3.exe" "%NAME3%"
copy "build\Release\mojiben4.exe" "%NAME4%"
copy "build\Release\mojiben5.exe" "%NAME5%"
copy "build\Release\mojiben6.exe" "%NAME6%"
copy "build\Release\mojiben7.exe" "%NAME7%"
copy "build\Release\mojiben8.exe" "%NAME8%"
copy "build\Release\mojiben9.exe" "%NAME9%"
copy "build\Release\FuriganaCtl.dll" "%NAME4%"
copy "build\Release\FuriganaCtl.dll" "%NAME5%"
copy "build\Release\FuriganaCtl.dll" "%NAME6%"
copy "build\Release\FuriganaCtl.dll" "%NAME7%"
copy "build\Release\FuriganaCtl.dll" "%NAME8%"
copy "build\Release\FuriganaCtl.dll" "%NAME9%"
copy "font\Piza-P-Gothic.ttf" "%NAME3%"
copy "font\Piza-P-Gothic.ttf" "%NAME4%"
copy "font\Piza-P-Gothic.ttf" "%NAME5%"
copy "font\Piza-P-Gothic.ttf" "%NAME6%"
copy "font\Piza-P-Gothic.ttf" "%NAME7%"
copy "font\Piza-P-Gothic.ttf" "%NAME8%"
copy "font\Piza-P-Gothic.ttf" "%NAME9%"
