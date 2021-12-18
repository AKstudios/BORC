@echo off
pushd ..
git add firmware/*.cpp firmware/*.h firmware/push.bat
git commit -m "Changes from Doug"
git push origin dev
popd
