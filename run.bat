cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd .\vcpkg\
.\bootstrap-vcpkg.bat
vcpkg install crow
vcpkg integrate install
