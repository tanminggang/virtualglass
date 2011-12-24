######################################################################
# Automatically generated by qmake (2.01a) Sat Apr 16 19:00:11 2011
######################################################################

QMAKE_LFLAGS_RELEASE += -static-libgcc
QMAKE_CFLAGS_RELEASE += -static-libgcc

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . yaml-cpp/include/ yaml-cpp/util/ yaml-cpp/src/ yaml-cpp/test/ glew/
QT += opengl
RESOURCES = virtualglass.qrc


unix:!macx {
  QMAKE_CXXFLAGS += -std=gnu++0x
}
macx {
  QMAKE_CXXFLAGS += -DUNORDERED_MAP_WORKAROUND
  ICON = virtualglass.incs
}
win32 {
  QMAKE_CXXFLAGS += -std=gnu++0x
}
LIBS += -lexpat

QMAKE_CFLAGS_DEBUG += -Wall -Werror -g
QMAKE_CFLAGS_RELEASE += -Wall -Werror -g
QMAKE_CXXFLAGS += -Wall -Werror -Wno-deprecated -g

# Input
HEADERS += glew/glew.h glew/glxew.h glew/wglew.h \

HEADERS += primitives.h constants.h Vector.hpp \ 
	pullplan.h pulltemplate.h subpulltemplate.h pickupplan.h \
	piece.h piecetemplate.h \
	pickuptemplate.h subpickuptemplate.h pickupplaneditorviewwidget.h \ 
	purecolorlibrarywidget.h \
	mainwindow.h niceviewwidget.h pullplanlibrarywidget.h piecelibrarywidget.h \
	pulltemplatelibrarywidget.h pickuptemplatelibrarywidget.h piecetemplatelibrarywidget.h \
	pullplaneditorviewwidget.h pullplaneditorwidget.h colorbarlibrarywidget.h \
	pieceeditorwidget.h coloreditorwidget.h \
	controller.h model.h geometry.h mesh.h \ 
	qgraphicshighlighteffect.h \

HEADERS += yaml-cpp/include/yaml-cpp/yaml.h \
	yaml-cpp/include/yaml-cpp/traits.h \
	yaml-cpp/include/yaml-cpp/stlnode.h \
	yaml-cpp/include/yaml-cpp/stlemitter.h \
	yaml-cpp/include/yaml-cpp/parser.h \
	yaml-cpp/include/yaml-cpp/ostream.h \
	yaml-cpp/include/yaml-cpp/null.h \
	yaml-cpp/include/yaml-cpp/noncopyable.h \
	yaml-cpp/include/yaml-cpp/nodeutil.h \
	yaml-cpp/include/yaml-cpp/nodereadimpl.h \
	yaml-cpp/include/yaml-cpp/nodeimpl.h \
	yaml-cpp/include/yaml-cpp/node.h \
	yaml-cpp/include/yaml-cpp/mark.h \
	yaml-cpp/include/yaml-cpp/ltnode.h \
	yaml-cpp/include/yaml-cpp/iterator.h \
	yaml-cpp/include/yaml-cpp/exceptions.h \
	yaml-cpp/include/yaml-cpp/eventhandler.h \
	yaml-cpp/include/yaml-cpp/emittermanip.h \
	yaml-cpp/include/yaml-cpp/emitter.h \
	yaml-cpp/include/yaml-cpp/emitfromevents.h \
	yaml-cpp/include/yaml-cpp/dll.h \
	yaml-cpp/include/yaml-cpp/conversion.h \
	yaml-cpp/include/yaml-cpp/anchor.h \
	yaml-cpp/include/yaml-cpp/aliasmanager.h \

HEADERS += yaml-cpp/src/token.h \
	yaml-cpp/src/tag.h \
	yaml-cpp/src/stringsource.h \
	yaml-cpp/src/streamcharsource.h \
	yaml-cpp/src/stream.h \
	yaml-cpp/src/singledocparser.h \
	yaml-cpp/src/setting.h \
	yaml-cpp/src/scantag.h \
	yaml-cpp/src/scanscalar.h \
	yaml-cpp/src/scanner.h \
	yaml-cpp/src/regeximpl.h \
	yaml-cpp/src/regex.h \
	yaml-cpp/src/ptr_vector.h \
	yaml-cpp/src/ptr_stack.h \
	yaml-cpp/src/nodeownership.h \
	yaml-cpp/src/nodebuilder.h \
	yaml-cpp/src/iterpriv.h \
	yaml-cpp/src/indentation.h \
	yaml-cpp/src/exp.h \
	yaml-cpp/src/emitterutils.h \
	yaml-cpp/src/emitterstate.h \
	yaml-cpp/src/directives.h \
	yaml-cpp/src/collectionstack.h \

SOURCES += glew/glew.c \

SOURCES += main.cpp \ 
	pullplan.cpp pulltemplate.cpp subpulltemplate.cpp \
	piece.cpp piecetemplate.cpp \
	pickupplan.cpp pickuptemplate.cpp subpickuptemplate.cpp pickupplaneditorviewwidget.cpp \
	purecolorlibrarywidget.cpp \
	model.cpp controller.cpp geometry.cpp mesh.cpp \
	mainwindow.cpp niceviewwidget.cpp pullplanlibrarywidget.cpp piecelibrarywidget.cpp \
	pulltemplatelibrarywidget.cpp pickuptemplatelibrarywidget.cpp piecetemplatelibrarywidget.cpp \
	pullplaneditorviewwidget.cpp pullplaneditorwidget.cpp colorbarlibrarywidget.cpp \ 
	qgraphicshighlighteffect.cpp \
	pieceeditorwidget.cpp coloreditorwidget.cpp \


SOURCES += yaml-cpp/src/tag.cpp \
	yaml-cpp/src/stream.cpp \
	yaml-cpp/src/singledocparser.cpp \
	yaml-cpp/src/simplekey.cpp \
	yaml-cpp/src/scantoken.cpp \
	yaml-cpp/src/scantag.cpp \
	yaml-cpp/src/scanscalar.cpp \
	yaml-cpp/src/scanner.cpp \
	yaml-cpp/src/regex.cpp \
	yaml-cpp/src/parser.cpp \
	yaml-cpp/src/ostream.cpp \
	yaml-cpp/src/null.cpp \
	yaml-cpp/src/nodeownership.cpp \
	yaml-cpp/src/nodebuilder.cpp \
	yaml-cpp/src/node.cpp \
	yaml-cpp/src/iterator.cpp \
	yaml-cpp/src/exp.cpp \
	yaml-cpp/src/emitterutils.cpp \
	yaml-cpp/src/emitterstate.cpp \
	yaml-cpp/src/emitter.cpp \
	yaml-cpp/src/emitfromevents.cpp \
	yaml-cpp/src/directives.cpp \
	yaml-cpp/src/conversion.cpp \
	yaml-cpp/src/aliasmanager.cpp \
