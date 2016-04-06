noinst_HEADERS = Application.h MultiPlatformHelper.h OISManager.h SceneHelper.h CoreConfig.h SoundManager.h GameManager.h GameObject.h Perlin.h Simulator.h BulletContactCallback.h CollisionContext.h OgreMotionState.h NetManager.h

bin_PROGRAMS = assignment2
assignment2_CPPFLAGS = -I$(top_srcdir) -std=c++11 -Wunused-variable
assignment2_SOURCES = Application.cpp main.cpp OISManager.cpp SoundManager.cpp GameManager.cpp Simulator.cpp GameObject.cpp Perlin.cpp OgreMotionState.cpp CollisionContext.cpp BulletContactCallback.cpp NetManager.cpp
assignment2_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) $(CEGUI_CFLAGS)
assignment2_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS) $(CEGUI_LIBS) $(CEGUI_OGRE_LIBS)
assignment2_LDFLAGS = -lOgreOverlay -lboost_system -lSDL -lSDL_mixer -lSDL_net

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
