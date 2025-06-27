echo -e "\e[94m***Configuring Project for Release***\e[0m"
./scripts-dev/configure.sh Release

echo -e "\e[94m***Building Project for Release***\e[0m"
cmake --build ./build --config Release --target all --

cd build/
echo -e "\e[94m***Pack Project for Release***\e[0m"
cpack -G ZIP -C Release
find ./ -name "*.zip" -exec cp -f -t ../ {} +

cd ..
./scripts-dev/clean.sh
