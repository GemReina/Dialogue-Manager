@echo off

pushd ..
Vendor\Binaries\Premake\Windows\premake5.exe --file=BuildDialogueProject.lua vs2022
popd
pause