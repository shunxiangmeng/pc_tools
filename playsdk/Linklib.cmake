#添加公共依赖的库
set(PLAYSDK_DEPEND_LIBS)

link_directories(${PROJECT_SOURCE_DIR}/playsdk/thirdparty/ffmpeg/lib)
set(PLAYSDK_DEPEND_LIBS libavutil.lib libavcodec.lib libavfilter.lib libavformat.lib libswscale.lib libswresample.lib libavdevice.lib zlib.lib bcrypt.lib)

link_directories(${PROJECT_SOURCE_DIR}/playsdk/thirdparty/glfw/lib)
set(PLAYSDK_DEPEND_LIBS ${PLAYSDK_DEPEND_LIBS} glfw3_mt.lib)

set(PLAYSDK_DEPEND_LIBS ${PLAYSDK_DEPEND_LIBS} opengl32.lib)

link_directories(${PROJECT_SOURCE_DIR}/playsdk/thirdparty/OpenAL/libs/Win32)
link_directories(${PROJECT_SOURCE_DIR}/playsdk/thirdparty/OpenAL/libs/Win32/EFX-Util_MT)
set(PLAYSDK_DEPEND_LIBS ${PLAYSDK_DEPEND_LIBS} OpenAL32.lib EFX-Util.lib)

set(APP_DEPEND_LIBS ${PLAYSDK_DEPEND_LIBS})
