ls | grep *.ino | cut -f1 -d'.' | xargs -I '{}' cp -T '{}.ino' '{}.cpp'

ls | grep *.cpp | xargs -I '{}' g++ '{}' -o test.exe
