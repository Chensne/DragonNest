set path=%path%;%cd%\bin;
7za x -o"%cd%\tmp" arc\zlib-1.2.5.tar.gz 
7za x -o"%cd%\tmp" "%cd%\tmp\zlib-1.2.5.tar"
7za x -o"%cd%\tmp" arc\bzip2-1.0.6.tar.gz 
7za x -o"%cd%\tmp" "%cd%\tmp\bzip2-1.0.6.tar"
7za x -o"%cd%\tmp" arc\icu4c-4_6-src.zip 
7za x -o"%cd%\tmp" arc\boost_1_45_0.7z 
set ZLIB_SOURCE="%cd%\tmp\zlib-1.2.5"
set BZIP2_SOURCE="%cd%\tmp\bzip2-1.0.6"
set PYTHON_ROOT="%cd%\tmp\Python-2.5.5"
set PYTHON_VERSION=2.5
set ICU_PATH="%cd%\tmp\icu\source\common"

pushd tmp\boost_1_45_0\tools\build\v2\build\
move configure.jam configure.jam.org
popd
copy configure.jam tmp\boost_1_45_0\tools\build\v2\build\configure.jam

pushd tmp\boost_1_45_0
bjam --prefix=%cd%\..\..\.. --toolset=msvc-10.0 --build-type=complete link=static runtime-link=static
bjam --prefix=%cd%\..\..\.. --toolset=msvc-10.0 --build-type=complete link=static runtime-link=shared
popd
rmdir /s /q stage
rmdir /s /q boost
move tmp\boost_1_45_0\boost boost
move tmp\boost_1_45_0\stage stage
rmdir /s /q tmp

