APPSDIR = $(kde_appsdir)/Games/TacticStrategy

SUBDIRS=data images levels

bin_PROGRAMS = ksokoban
ksokoban_SOURCES = Bookmark.C History.C HtmlPrinter.C ImageData.C InternalCollections.C LevelCollection.C LevelMap.C MainWindow.C Map.C MapDelta.C ModalLabel.C Move.C MoveSequence.C PathFinder.C PlayField.C StaticImage.C main.C

noinst_HEADERS  = Bookmark.H History.H HtmlPrinter.H ImageData.H InternalCollections.H LevelCollection.H LevelMap.H MainWindow.H Map.H MapDelta.H ModalLabel.H Move.H MoveSequence.H PathFinder.H PlayField.H Queue.H StaticImage.H

METASOURCES= MainWindow.moc ModalLabel.moc PlayField.moc

INCLUDES = $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KFILE) $(LIB_KSYCOCA)
# $(LIBZ)

EXTRA_DIST=AUTHORS NEWS README TODO

# we need theese deps for the automatic generation of other deps
StaticImage.o: images/data.c
LevelMap.o: levels/data.c

# these are for automatic generation of deps, too
levels/data.c:
	(cd levels && $(MAKE) data.c)
images/data.c:
	(cd images && $(MAKE) data.c)

messages:
	$(XGETTEXT) *.C -o $(podir)/ksokoban.pot

install-data-local:
	rm -rf $(kde_appsdir)/Games/ksokoban.desktop
