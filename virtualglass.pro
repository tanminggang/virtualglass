######################################################################
# Automatically generated by qmake (2.01a) Sat Apr 16 19:00:11 2011
######################################################################

!*-msvc* {
	QMAKE_LFLAGS_RELEASE += -static-libgcc
	QMAKE_CFLAGS_RELEASE += -static-libgcc
}

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . glew/
INCLUDEPATH += . jsoncpp/include
QT += opengl
RESOURCES = virtualglass.qrc
DEFINES += GLEW_MX

unix:!macx {
	QMAKE_CXXFLAGS += -std=gnu++0x
	LIBS += -lGLU
}
macx {
	QMAKE_CC = g++
	QMAKE_CFLAGS += -Qunused-arguments
	QMAKE_CXX = g++
	QMAKE_CXXFLAGS += -DUNORDERED_MAP_WORKAROUND -I src/jsoncpp/include 
	ICON = virtualglass.icns
	QMAKE_INFO_PLIST = virtualglass.Info.plist
}

*-msvc* {
	MYFLAGS = /DMSVC /Wall /WX /wd4711 /wd4706 /wd4146 /wd4996 /wd4619 /wd4826 /wd4305 /wd4244 /wd4738 /wd4710 /wd4640 /wd4350 /wd4701 /wd4628
	#The 4640 and 4350 warnings disabled above may hide YAML bugs
	#The 4701 is disabled because of a mis-detection in vector.hpp.
	#   ...well, I hope it is a mis-detection.
	QMAKE_CFLAGS_DEBUG += $${MYFLAGS}
	QMAKE_CFLAGS_RELEASE += $${MYFLAGS}
	QMAKE_CXXFLAGS += $${MYFLAGS}
}
#one hopes this catches mingw:
!*-msvc* {
	QMAKE_CFLAGS_DEBUG += -Wall -Werror -g
	QMAKE_CFLAGS_RELEASE += -Wall -Werror -g
        QMAKE_CXXFLAGS += -Wall -Werror -Wno-unused-parameter -Wno-deprecated -g
        win32 {
		QMAKE_CXXFLAGS += -std=gnu++0x
	}
	LIBS += -lexpat
}



HEADERS += vgapp.h \ 
	glew/glew.h \
	glew/glxew.h \
	glew/wglew.h \
	bubble.h \
	jsoncpp/include/json/autolink.h \
        jsoncpp/include/json/config.h \
        jsoncpp/include/json/features.h \
        jsoncpp/include/json/forwards.h \
        jsoncpp/include/json/json.h \
        jsoncpp/include/json/reader.h \
        jsoncpp/include/json/value.h \
        jsoncpp/include/json/writer.h \
        primitives.h \
        constants.h \
        Vector.hpp \
	globalglass.h \
	randomglass.h \
	glasslibrarywidget.h \
	dependancy.h \
	shape.h \
	templateparameter.h \
	pullplan.h \
	pulltemplate.h \
	subpulltemplate.h \
	pickupplan.h \
	piece.h \
	piecetemplate.h \
	pickuptemplate.h \
	subpickuptemplate.h \
	pickupplaneditorviewwidget.h \ 
	purecolorlibrarywidget.h \
	casing.h \
	mainwindow.h \
	niceviewwidget.h \
	pulltemplatelibrarywidget.h \
	pickuptemplatelibrarywidget.h \
	piecetemplatelibrarywidget.h \
	pullplaneditorviewwidget.h \
	pullplaneditorwidget.h \
	pieceeditorwidget.h \
	coloreditorwidget.h \
	geometry.h mesh.h \ 
	peelrenderer.h \
	asyncrenderwidget.h \
	asyncrenderinternal.h \
	asyncrenderthread.h \
	pullplanrenderdata.h \
	piecerenderdata.h \
	pullplanlibrarywidget.h \
	colorbarlibrarywidget.h \
	piecelibrarywidget.h \
	pullplancustomizeviewwidget.h \
	glassmime.h \
        glasscolor.h \
        SVG.hpp \
        Box.hpp \
        Matrix.hpp \
	glassfileio.h \
	piecegeometrythread.h \
	pullplangeometrythread.h \
	globalgraphicssetting.h \
	twistwidget.h \
	templatelibrarywidget.h \
	piececustomizeviewwidget.h \ 
	spline.h \
	globaldepthpeelingsetting.h \

SOURCES += main.cpp \ 
	vgapp.cpp \
	glew/glew.c \
	glasslibrarywidget.cpp \
	randomglass.cpp \
	templateparameter.cpp \
	pullplancustomizeviewwidget.cpp \
	pullplan.cpp \
	subpulltemplate.cpp \
	globalglass.cpp \
	piece.cpp \
	piecetemplate.cpp \
	pulltemplate.cpp \
	pickuptemplate.cpp \
	pickupplan.cpp \
	subpickuptemplate.cpp \
	pickupplaneditorviewwidget.cpp \
	purecolorlibrarywidget.cpp \
	casing.cpp \
	geometry.cpp \
	mesh.cpp \
	mainwindow.cpp \
	niceviewwidget.cpp \
	pulltemplatelibrarywidget.cpp \
	pickuptemplatelibrarywidget.cpp \
	piecetemplatelibrarywidget.cpp \
	pullplaneditorviewwidget.cpp \
	pullplaneditorwidget.cpp \ 
	pieceeditorwidget.cpp \
	coloreditorwidget.cpp \
	peelrenderer.cpp \
	asyncrenderwidget.cpp \
	asyncrenderinternal.cpp \
	asyncrenderthread.cpp \
	pullplanrenderdata.cpp \
	piecerenderdata.cpp \
	pullplanlibrarywidget.cpp \
	colorbarlibrarywidget.cpp \
	piecelibrarywidget.cpp \
	glassmime.cpp \
	glasscolor.cpp \
        jsoncpp/src/lib_json/json_reader.cpp \
        jsoncpp/src/lib_json/json_value.cpp \
	jsoncpp/src/lib_json/json_writer.cpp \
        SVG.cpp \
        bubble.cpp \
	glassfileio.cpp \
	piecegeometrythread.cpp \
	pullplangeometrythread.cpp \
	globalgraphicssetting.cpp \
	twistwidget.cpp \
	templatelibrarywidget.cpp \
	piececustomizeviewwidget.cpp \
	spline.cpp \
	globaldepthpeelingsetting.cpp \


