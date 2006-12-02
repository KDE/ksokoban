project(ksokoban)

add_subdirectory( data ) 
add_subdirectory( images ) 

########### next target ###############

set(images_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/images/goal.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/halfstone_1.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/halfstone_2.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/halfstone_3.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/halfstone_4.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/man.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/object.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/saveman.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/stone_1.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/stone_2.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/stone_3.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/stone_4.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/stone_5.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/stone_6.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/treasure.png
        ${CMAKE_CURRENT_SOURCE_DIR}/images/starfield.png)

if (WIN32)
  set (BIN2C ${CMAKE_BINARY_DIR}/bin/bin2c)
else (WIN32)
  set (BIN2C images/bin2c)
endif(WIN32)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/data.c COMMAND ${BIN2C}
		   ARGS \"\" ${CMAKE_CURRENT_SOURCE_DIR}/levels/level.data ${images_FILES} 
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/images/bin2c ${CMAKE_CURRENT_SOURCE_DIR}/levels/level.data ${images_FILES} 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
	
set(ksokoban_SRCS
   ${CMAKE_CURRENT_BINARY_DIR}/data.c
   Bookmark.cpp 
   History.cpp 
   HtmlPrinter.cpp 
   ImageData.cpp 
   InternalCollections.cpp 
   LevelCollection.cpp 
   LevelMap.cpp 
   MainWindow.cpp 
   Map.cpp 
   MapDelta.cpp 
   ModalLabel.cpp 
   Move.cpp 
   MoveSequence.cpp 
   PathFinder.cpp 
   PlayField.cpp 
   StaticImage.cpp 
   main.cpp )

kde4_automoc(${ksokoban_SRCS})

kde4_add_executable(ksokoban ${ksokoban_SRCS})

target_link_libraries(ksokoban  ${KDE4_KDE3SUPPORT_LIBS} )

install(TARGETS ksokoban  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############
install( FILES ksokobanui.rc  DESTINATION  ${DATA_INSTALL_DIR}/ksokoban )





#original Makefile.am contents follow:

#APPSDIR = $(kde_appsdir)/Games/TacticStrategy
#
#SUBDIRS=data images levels
#
#bin_PROGRAMS = ksokoban
#ksokoban_SOURCES = Bookmark.cpp History.cpp HtmlPrinter.cpp ImageData.cpp InternalCollections.cpp LevelCollection.cpp LevelMap.cpp MainWindow.cpp Map.cpp MapDelta.cpp ModalLabel.cpp Move.cpp MoveSequence.cpp PathFinder.cpp PlayField.cpp StaticImage.cpp main.cpp
#ksokoban_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#ksokoban_LDADD = $(LIB_KIO) $(LIB_KDE3SUPPORT)
#
#noinst_HEADERS  = Bookmark.h History.h HtmlPrinter.h ImageData.h InternalCollections.h LevelCollection.h LevelMap.h MainWindow.h Map.h MapDelta.h ModalLabel.h Move.h MoveSequence.h PathFinder.h PlayField.h Queue.h StaticImage.h
#
#METASOURCES= MainWindow.moc ModalLabel.moc PlayField.moc
#
#INCLUDES = $(all_includes)
#
#EXTRA_DIST=AUTHORS NEWS README TODO
#
## we need theese deps for the automatic generation of other deps
#StaticImage.o: images/data.c
#InternalCollections.o: levels/data.c
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/ksokoban.pot
