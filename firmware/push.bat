@echo off
pushd ..
git add firmware/*.cpp firmware/*.h firmware/*.ino firmware/push.bat
git commit -m "aprintf() and asprintf()"
git push origin dev
popd
