@echo off
pushd ..
git add firmware/*.cpp firmware/*.h firmware/*.ino firmware/push.bat
git commit -m "Simulate temperature via serial server"
git push origin dev
popd
