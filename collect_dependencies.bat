@echo off

set DEST_DIR=dependencies

call setenv.bat

mkdir %DEST_DIR%

REM Copy the OSG dlls 
xcopy /D /Y "%OSG_ROOT%\bin\*.dll" %DEST_DIR%

REM Copy the OSG earth dlls 
xcopy /D /Y "%OSGEARTH%\bin\*.dll" %DEST_DIR%

REM Copy OSG deps
xcopy /D /Y "%OSG_DEPS%\bin\*.dll" %DEST_DIR%

REM Copy 3rd party deps
REM xcopy /D /Y "3rdParty\*.dll" %DEST_DIR%

REM Copy GDAL deps
xcopy /D /Y "%GDAL%\bin\*.dll" %DEST_DIR%

REM Copy over the necessary plugins
mkdir "%DEST_DIR%\%OSG_PLUGINS_FOLDER%"

xcopy /D /Y "%OSG_PLUGINS_DIR%\osgdb_*.dll" "%DEST_DIR%\%OSG_PLUGINS_FOLDER%"

xcopy /D /Y "%OSGEARTH_PLUGINS%\*.dll" "%DEST_DIR%\%OSG_PLUGINS_FOLDER%"

REM Copy over the GEOS dlls
xcopy /D /Y "%GEOS%\bin\*.dll" %DEST_DIR%

REM Delete any debug dlls
del %DEST_DIR%\*d.dll
del %DEST_DIR%\%OSG_PLUGINS_FOLDER%\*d.dll

