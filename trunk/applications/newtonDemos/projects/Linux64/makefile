#*******************************************************
#
# Newton game dynamics 
# copy right by Julio Jerez 2003 - 2010 
# Copyright (c) <2010> <Newton Game Dynamics>
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely
#
#*******************************************************

#*******************************************************
#
# SDK Sample tutorial: 
#
#*******************************************************  


# ******************************************************
#
# project source files
#
# ******************************************************


# generic sdk flags
SDK_FLAGS = -O0 -c -Wall -D_LINUX_VER -g -pg
#SDK_FLAGS = -m32 -O3 -c -Wall -Werror -D_LINUX_VER -g 
#SDK_FLAGS = -m32 -O3 -c -Wall -D_LINUX_VER -g 

PLATFORM = Linux64

SDK_MAIN = ../../sdkDemos
SDK_DEMOS = ../../sdkDems/demos
SDK_TOOLBOX = ../../sdkDemos/toolBox
NEWTON_CORE = ../../../../coreLibrary_200/projets/linux64
SDK_NEWTON = ../../../../packages/$(PLATFORM)
SDK_MATH = ../../../../packages/projects/$(PLATFORM)/dMath
SDK_SCENE = ../../../../packages/projects/$(PLATFORM)/dScene
SDK_JOINTS = ../../../../packages/projects/$(PLATFORM)/dCustomJoints
SDK_CONTAINERS = ../../../../packages/projects/$(PLATFORM)/dContainers
SDK_XML = ../../../../packages/projects/$(PLATFORM)/thirdParty/tinyxml
QT_HEADERS = /usr/include/qt4


LIBS  = -L/usr/lib -lGL -lGLU -lGLEW -lpthread -lQtCore -lQtGui 
			

# this use newton as shared or static library
NEWTON_LIB = \
	$(SDK_NEWTON)/libNewton.a \
	$(SDK_NEWTON)/libdMath.a \
	$(SDK_NEWTON)/libdScene.a \
	$(SDK_NEWTON)/libdContainers.a \
	$(SDK_NEWTON)/libdJointLibrary.a 
			 

SDK_INCLUDE_PATH = -I $(SDK_MAIN) \
		   -I $(SDK_DEMOS) \
		   -I $(SDK_XML) \
		   -I $(SDK_MATH) \
		   -I $(SDK_SCENE) \
		   -I $(SDK_JOINTS) \
		   -I $(SDK_NEWTON) \
		   -I $(SDK_TOOLBOX) \
		   -I $(SDK_CONTAINERS) \
		   -I $(QT_HEADERS) 


SDK_MAIN_SRC = \
		$(SDK_MAIN)/NewtonDemos.cpp  
#		$(SDK_MAIN)/DemoCamera.cpp \
#		$(SDK_MAIN)/DemoEntity.cpp \
#		$(SDK_MAIN)/DemoMesh.cpp \
#		$(SDK_MAIN)/DemoSceneManager.cpp \
#		$(SDK_MAIN)/PhysicsUtils.cpp \


SDK_DEMOS_SRC = \
#	$(SDK_DEMOS)/BasicFriction.cpp \
#	$(SDK_DEMOS)/BasicSphereSpin.cpp \
#	$(SDK_DEMOS)/ClosestDistance.cpp 
#	$(SDK_DEMOS)/BasicRestitution.cpp \
#	$(SDK_DEMOS)/ArchimedesBuoyancy.cpp \
#	$(SDK_DEMOS)/BasicCustomJoints.cpp \
#	$(SDK_DEMOS)/BasicRobots.cpp \
#	$(SDK_DEMOS)/BasicRagDoll.cpp \
#	$(SDK_DEMOS)/BasicStacking.cpp \
#	$(SDK_DEMOS)/CompoundCollision.cpp \
#	$(SDK_DEMOS)/ConvexCast.cpp \
#	$(SDK_DEMOS)/Jengas.cpp \
#	$(SDK_DEMOS)/LegacyJoints.cpp \
#	$(SDK_DEMOS)/MeshCollision.cpp \
#	$(SDK_DEMOS)/PrecessingTops.cpp \
#	$(SDK_DEMOS)/Pyramids.cpp \
#	$(SDK_DEMOS)/RayCastCar.cpp \
#	$(SDK_DEMOS)/ScaledCollision.cpp \
#	$(SDK_DEMOS)/SpecialTrackJoints.cpp \
#	$(SDK_DEMOS)/Towers.cpp \
#	$(SDK_DEMOS)/TriggersAndForceFields.cpp \
#	$(SDK_DEMOS)/UnstableStacks.cpp \
#	$(SDK_DEMOS)/CharacterController.cpp \
#	$(SDK_DEMOS)/Walls.cpp 
#	$(SDK_DEMOS)/BuildingDestruction.cpp \
#	$(SDK_DEMOS)/RealTimeSimpleDestruction.cpp \
#	$(SDK_DEMOS)/PrefabSimpleDestruction.cpp \


SDK_TOOLBOX_SRC = \
#	$(SDK_TOOLBOX)/DebugDisplay.cpp \
#	$(SDK_TOOLBOX)/DGCamera.cpp \
#	$(SDK_TOOLBOX)/dRuntimeProfiler.cpp \
#	$(SDK_TOOLBOX)/HeightFieldPrimitive.cpp \
#	$(SDK_TOOLBOX)/HiResTimer.cpp  \
#	$(SDK_TOOLBOX)/LevelPrimitive.cpp \
#	$(SDK_TOOLBOX)/MousePick.cpp \
#	$(SDK_TOOLBOX)/OpenGlUtil.cpp \
#	$(SDK_TOOLBOX)/PlaneCollision.cpp \
#	$(SDK_TOOLBOX)/RenderPrimitive.cpp \
#	$(SDK_TOOLBOX)/ScenePrimitive.cpp \
#	$(SDK_TOOLBOX)/SkyBox.cpp \
#	$(SDK_TOOLBOX)/TargaToOpenGl.cpp \
#	$(SDK_TOOLBOX)/toolbox_stdafx.cpp \
#	$(SDK_TOOLBOX)/UserHeightFieldCollision.cpp 

		
SDK_SRCS = $(SDK_MAIN_SRC) $(SDK_DEMOS_SRC) $(SDK_TOOLBOX_SRC) 

SDK_OBJS = $(SDK_SRCS:.cpp=.o)

COMPILER = g++
FLAGS = $(SDK_FLAGS) $(SDK_INCLUDE_PATH)


.SUFFIXES : .o .cpp
.cpp.o :
	$(COMPILER) $(FLAGS) -o $@ $<	

# main target
NewtonDemos : $(SDK_OBJS) 
	make -C $(NEWTON_CORE)
	make -C $(SDK_XML)
	make -C $(SDK_MATH)
	make -C $(SDK_SCENE)
	make -C $(SDK_JOINTS)
	make -C $(SDK_CONTAINERS)
	gcc $? $(NEWTON_LIB) $(LIBS) $(THIRD_PARTY_LIBRARIES)  -o $@.elf	


# clean all objects target
clean :
	make clean -C $(SDK_XML)
	make clean -C $(SDK_MATH)
	make clean -C $(SDK_SCENE)
	make clean -C $(SDK_JOINTS)
	make clean -C $(SDK_CONTAINERS)
	rm $(SDK_OBJS)
	touch $(SDK_SRCS)
	

	
	
	
	
