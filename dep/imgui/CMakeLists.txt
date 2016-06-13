# for automatic ImVec<->glm::vec conversion
include_directories("${glm_INCLUDE_DIRS}")


set(imgui_src
    imconfig.h
    imgui.cpp
    imgui.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    stb_rect_pack.h
    stb_textedit.h
    stb_truetype.h
    imgui_user.inl
)


add_library(imgui ${imgui_src})
#target_link_libraries(imgui)

