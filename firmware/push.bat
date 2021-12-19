@echo off
pushd ..
git add firmware/*.cpp firmware/*.h firmware/*.ino firmware/push.bat
git commit -m "Sleepmanager AWAKE_MODE now calls on_wakeup_from_timer()"
git push origin dev
popd
