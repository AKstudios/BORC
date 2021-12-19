@echo off
pushd ..
git add firmware/*.cpp firmware/*.h firmware/*.ino firmware/push.bat
git commit -m "Changes for Notch Controller temperature control"
git push origin dev
popd
