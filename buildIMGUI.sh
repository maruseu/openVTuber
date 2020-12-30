#!/bin/bash
src="imgui/imgui.cpp
imgui/imgui_demo.cpp
imgui/imgui_draw.cpp
imgui/imgui_tables.cpp
imgui/imgui_widgets.cpp
imgui/backends/imgui_impl_glfw.cpp
imgui/backends/imgui_impl_opengl3.cpp"

for i in $src; do
	printf "compiling %s | " ${filename}
	filename="${i##*/}"
	${@} -o imguiobjs/${filename%.cpp}.o -c ${i}
done
printf "\nLinking libFramework.a\n"
ar qc libIMGUI.a imguiobjs/*.o
ranlib libIMGUI.a

