
link_libraries(glfw)

include_directories("${GLFW_SOURCE_DIR}/deps")

link_directories(/usr/local/lib)

link_directories(/usr/local/Cellar/assimp/5.0.1/lib)
link_directories(/usr/local/Cellar/assimp/5.0.0/lib)
link_directories(/usr/local/Cellar/glib/2.62.2/lib)
include_directories(/usr/local/Cellar/assimp/5.0.1/include)
include_directories(/usr/local/Cellar/assimp/5.0.0/include)
include_directories(/usr/local/Cellar/glib/2.62.2/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)

if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
    include_directories(${GLIB_PKG_INCLUDE_DIRS})
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

if (MATH_LIBRARY)
    link_libraries("${MATH_LIBRARY}")
endif()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (WIN32)
    set(ICON glfw.rc)
elseif (APPLE)
    set(ICON glfw.icns)
endif()

if (${CMAKE_VERSION} VERSION_EQUAL "3.1.0" OR
    ${CMAKE_VERSION} VERSION_GREATER "3.1.0")
    set(CMAKE_C_STANDARD 99)
else()
    # Remove this fallback when removing support for CMake version less than 3.1
    add_compile_options("$<$<C_COMPILER_ID:AppleClang>:-std=c99>"
                        "$<$<C_COMPILER_ID:Clang>:-std=c99>"
                        "$<$<C_COMPILER_ID:GNU>:-std=c99>")

endif()

set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h"
            "${GLFW_SOURCE_DIR}/deps/glad_gl.c")
set(GLAD_GLAD "${GLFW_SOURCE_DIR}/deps/glad/glad.h"
              "${GLFW_SOURCE_DIR}/deps/KHR/khrplatform.h"
              "${GLFW_SOURCE_DIR}/deps/glad.c")
set(GETOPT "${GLFW_SOURCE_DIR}/deps/getopt.h"
           "${GLFW_SOURCE_DIR}/deps/getopt.c")
set(TINYCTHREAD "${GLFW_SOURCE_DIR}/deps/tinycthread.h"
                "${GLFW_SOURCE_DIR}/deps/tinycthread.c")
set(STB "${GLFW_SOURCE_DIR}/deps/stb/stb_image.h")
set(SHADERS "${GLFW_SOURCE_DIR}/shaders/pos_col_frag.shader"
            "${GLFW_SOURCE_DIR}/shaders/pos_col_frag_col4.shader"
            "${GLFW_SOURCE_DIR}/shaders/pos_col_frag_img.shader"
            "${GLFW_SOURCE_DIR}/shaders/pos_col_frag_white.shader"
            "${GLFW_SOURCE_DIR}/shaders/pos_col_vertex.shader"
            "${GLFW_SOURCE_DIR}/shaders/pos_col_vertex_img.shader")
set(IMAGES  "${GLFW_SOURCE_DIR}/res/p1tank.png"
            "${GLFW_SOURCE_DIR}/res/awesomface.png"
            "${GLFW_SOURCE_DIR}/res/tuichu.png"
            "${GLFW_SOURCE_DIR}/res/cube_met.png")
set(NANOSUIT    "${GLFW_SOURCE_DIR}/res/nanosuit/LICENSE.txt"
                "${GLFW_SOURCE_DIR}/res/nanosuit/arm_dif.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/arm_showroom_ddn.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/arm_showroom_spec.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/body_dif.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/body_showroom_ddn.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/body_showroom_spec.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/glass_ddn.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/glass_dif.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/hand_dif.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/hand_showroom_ddn.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/hand_showroom_spec.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/helmet_diff.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/helmet_showroom_ddn.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/helmet_showroom_spec.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/leg_dif.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/leg_showroom_ddn.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/leg_showroom_spec.png"
                "${GLFW_SOURCE_DIR}/res/nanosuit/nanosuit.blend"
                "${GLFW_SOURCE_DIR}/res/nanosuit/nanosuit.obj1"
                "${GLFW_SOURCE_DIR}/res/nanosuit/nanosuit.mtl")

add_executable(learn_opengl_1 WIN32 MACOSX_BUNDLE learn.c ${ICON} ${GLAD_GLAD})
add_executable(learn_opengl_2 WIN32 MACOSX_BUNDLE ${ICON} ${GLAD_GLAD} ${SHADERS} ${STB} ${IMAGES}
               learn2.c utils.c)
add_executable(learn_opengl_3 WIN32 MACOSX_BUNDLE ${ICON} ${GLAD_GLAD} ${SHADERS} ${STB} ${IMAGES}
               learn3.c utils.c)
add_executable(learn_opengl_4 WIN32 MACOSX_BUNDLE ${ICON} ${GLAD_GLAD} ${SHADERS} ${STB} ${IMAGES} ${NANOSUIT}
               learn4.c utils.c camera.c shader.c texture2d.c
               component/transform.c component/image.c data/mesh.c data/mesh.h data/model.c data/model.h)

target_link_libraries(learn_opengl_3 PRIVATE cglm)
target_link_libraries(learn_opengl_4 PRIVATE cglm glib-2.0 assimp)

set(GUI_ONLY_BINARIES learn_opengl_1)
set(CONSOLE_BINARIES offscreen)

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (APPLE)
    set_target_properties(learn_opengl_1 PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "learn_opengl_1")
    set_target_properties(learn_opengl_2 PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "learn_opengl_2")
    set_target_properties(learn_opengl_3 PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "learn_opengl_3")
    set_target_properties(learn_opengl_4 PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "learn_opengl_4")

    set_source_files_properties(glfw.icns ${IMAGES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(glfw.icns ${NANOSUIT} PROPERTIES MACOSX_PACKAGE_LOCATION "nanosuit")

    set_source_files_properties(${SHADERS} PROPERTIES MACOSX_PACKAGE_LOCATION "Shaders")

    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES
                          MACOSX_BUNDLE_SHORT_VERSION_STRING ${GLFW_VERSION}
                          MACOSX_BUNDLE_LONG_VERSION_STRING ${GLFW_VERSION}
                          MACOSX_BUNDLE_ICON_FILE glfw.icns
                          MACOSX_BUNDLE_INFO_PLIST "${GLFW_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist.in")
endif()

