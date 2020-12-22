#!/bin/bash
for i in $(find ../Framework/src -type f | grep '.cpp' | grep -v "D3D"); do
	printf "compiling %s | " ${filename}
	filename="${i##*/}"
	${@} -o fw/${filename%.cpp}.o -c ${i}
done
printf "\nLinking libFramework.a\n"
ar qc libFramework.a fw/*.o
ranlib libFramework.a
