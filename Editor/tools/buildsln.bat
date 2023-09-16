@echo off
start devenv %1 /rebuild %2
echo Building solution: %1
echo Configuration: %2