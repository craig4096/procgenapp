#-------------------------------------------------
#
# Project created by QtCreator 2012-01-20T14:02:22
#
#-------------------------------------------------

QT       += core gui opengl

unix:LIBS += -lGLEW

TARGET = ../Bin/HonoursProject
TEMPLATE = app

OBJECTS_DIR = ../Obj


SOURCES += main.cpp\
    Heightmap.cpp \
    Application.cpp \
    math3d.cpp \
    TerrainModule.cpp \
    Viewport3D.cpp \
    Skybox.cpp \
    ImageManager.cpp \
    ColorSpline.cpp \
    Color.cpp \
    RenderableHeightmap.cpp \
    BSGModule.cpp \
    Mesh.cpp \
    Shader.cpp \
    ProcTexturingModule.cpp \
    Game/ShadersMap.cpp \
    Game/GameDemo.cpp \
    Game/DungeonEntrance.cpp \
    Terrain/VoronoiStencil.cpp \
    Terrain/VoronoiDiagram.cpp \
    Terrain/ThermalErosion.cpp \
    Terrain/TerrainGenerator.cpp \
    Terrain/PerlinNoise.cpp \
    Terrain/Normalization.cpp \
    Terrain/Modulate.cpp \
    Terrain/InverseThermalErosion.cpp \
    Terrain/HydraulicErosion.cpp \
    Terrain/HeightmapStack.cpp \
    Terrain/Blend.cpp \
    Structural/ShapeDatabase.cpp \
    Structural/Shape.cpp \
    Structural/RadialProbFunc.cpp \
    Structural/ConstProbFunc.cpp \
    Structural/BasicShapeGrammar.cpp \
    GameModule.cpp

HEADERS  += \
    Heightmap.h \
    Application.h \
    math3d.h \
    Array2D.h \
    TerrainModule.h \
    Viewport3D.h \
    Skybox.h \
    ImageManager.h \
    ColorSpline.h \
    Color.h \
    RenderableHeightmap.h \
    BSGModule.h \
    Mesh.h \
    Shader.h \
    ProcTexturingModule.h \
    defs.h \
    Game/ShadersMap.h \
    Game/GameDemo.h \
    Game/DungeonEntrance.h \
    Terrain/VoronoiStencil.h \
    Terrain/VoronoiDiagram.h \
    Terrain/ThermalErosion.h \
    Terrain/TerrainOperation.h \
    Terrain/TerrainGenerator.h \
    Terrain/PerlinNoise.h \
    Terrain/Normalization.h \
    Terrain/Modulate.h \
    Terrain/InverseThermalErosion.h \
    Terrain/HydraulicErosion.h \
    Terrain/HeightmapStack.h \
    Terrain/Blend.h \
    Structural/ShapeDatabase.h \
    Structural/Shape.h \
    Structural/RadialProbFunc.h \
    Structural/ProbabilityFunction.h \
    Structural/ConstProbFunc.h \
    Structural/BasicShapeGrammar.h \
    GameModule.h \
    Progress.h

FORMS    += application.ui \
    application.ui
