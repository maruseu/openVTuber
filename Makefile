CC = gcc

PROJ_NAME = vtube

DEFINES = -DCSM_TARGET_LINUX_GL -DGLEW_STATIC

INCLUDES = -Istb -I/usr/include/opencv4 \
-I../Framework/src -isystem../Core/include

FLAGS = -O3 -DNDEBUG -std=c++14 -lstdc++

LNK_LIBS = -lm -ldl -lX11 -lpthread -lglfw -lOpenGL -lGLX -lGLU -lGLEW \
-lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_highgui \
-lcblas -llapack -ldlib -lrt

LNK_OBJS = libFramework.a ../Core/lib/linux/x86_64/libLive2DCubismCore.a

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

all: $(OBJS) libFramework.a
	$(CC) $(FLAGS) -no-pie $(OBJS) $(LNK_LIBS) $(LNK_OBJS) -o $(PROJ_NAME)

clean:
	rm -f $(OBJS) libFramework.a fw/* $(PROJ_NAME)

libFramework.a: buildFramework.sh
	./buildFramework.sh $(CC) $(FLAGS) $(DEFINES) $(INCLUDES)

-include $(DEPS)

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(DEFINES) $(INCLUDES) -MMD -MP -o $@ -c $<
