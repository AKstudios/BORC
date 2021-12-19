@echo off
pushd ..
git add firmware/*.cpp firmware/*.h firmware/*.ino firmware/push.bat
git commit -m "Changes from Doug for AWAKE_MODE"
git push origin dev
popd
