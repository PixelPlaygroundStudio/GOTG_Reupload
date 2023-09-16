@echo off
cd game_assembly
call msbuild WiwaGameAssembly.sln /p:Configuration=Release /p:Platform=x64 -m