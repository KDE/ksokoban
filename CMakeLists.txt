kde4_header()
project(ksokoban)

add_subdirectory( data ) 
add_subdirectory( images ) 
add_subdirectory( levels ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/ksokoban/levels/data.c COMMAND ../images/bin2c  ARGS \"\" ${CMAKE_SOURCE_DIR}/ksokoban/levels/level.data DEPENDS ${CMAKE_SOURCE_DIR}/ksokoban/images/bin2c ${CMAKE_SOURCE_DIR}/ksokoban/levels/level.data WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/ksokoban/levels/ )

set(images_FILES
	${CMAKE_SOURCE_DIR}/ksokoban/images/goal.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/halfstone_1.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/halfstone_2.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/halfstone_3.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/halfstone_4.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/man.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/object.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/saveman.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/stone_1.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/stone_2.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/stone_3.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/stone_4.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/stone_5.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/stone_6.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/treasure.png
	${CMAKE_SOURCE_DIR}/ksokoban/images/starfield.png)
	
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/ksokoban/images/data.c COMMAND ../images/bin2c  ARGS \"\" ${images_FILES} DEPENDS ${CMAKE_SOURCE_DIR}/ksokoban/images/bin2c WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/ksokoban/images/ )


set(ksokoban_SRCS ${CMAKE_SOURCE_DIR}/ksokoban/levels/data.c ${CMAKE_SOURCE_DIR}/ksokoban/images/data.c
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

target_link_libraries(ksokoban  ${KDE4_KDE3SUPPORT_LIBRARIES} )

install_targets(/bin ksokoban )


########### install files ###############


kde4_footer()



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
