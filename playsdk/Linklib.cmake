#添加公共依赖的库
set(PLAYSDK_DEPEND_LIBS)

link_directories(${PROJECT_SOURCE_DIR}/playsdk/third_party/ffmpeg/lib)
set(PLAYSDK_DEPEND_LIBS libavutil.lib libavcodec.lib libavfilter.lib libavformat.lib libswscale.lib libswresample.lib libavdevice.lib zlib.lib bcrypt.lib)

set(APP_DEPEND_LIBS ${PLAYSDK_DEPEND_LIBS})