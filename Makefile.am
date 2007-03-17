APPSDIR = $(kde_appsdir)/Games/TacticStrategy

SUBDIRS=levels data images

bin_PROGRAMS = ksokoban
ksokoban_SOURCES = Bookmark.cpp History.cpp HtmlPrinter.cpp ImageData.cpp InternalCollections.cpp LevelCollection.cpp LevelMap.cpp MainWindow.cpp Map.cpp MapDelta.cpp ModalLabel.cpp Move.cpp MoveSequence.cpp PathFinder.cpp PlayField.cpp StaticImage.cpp main.cpp
ksokoban_LDFLAGS = $(all_libraries) $(KDE_RPATH)
ksokoban_LDADD = $(LIB_KIO)

noinst_HEADERS  = Bookmark.h History.h HtmlPrinter.h ImageData.h InternalCollections.h LevelCollection.h LevelMap.h MainWindow.h Map.h MapDelta.h ModalLabel.h Move.h MoveSequence.h PathFinder.h PlayField.h Queue.h StaticImage.h

METASOURCES= MainWindow.moc ModalLabel.moc PlayField.moc

INCLUDES = $(all_includes)

EXTRA_DIST=AUTHORS NEWS README TODO

# we need theese deps for the automatic generation of other deps
StaticImage.o: images/data.c
InternalCollections.o: levels/data.c

messages:
	$(XGETTEXT) *.cpp -o $(podir)/ksokoban.pot
