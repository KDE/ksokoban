APPSDIR = $(kde_appsdir)/Games

SUBDIRS=data images levels

bin_PROGRAMS = ksokoban
ksokoban_SOURCES = HiRes.C History.C LevelMap.C LowRes.C MainWindow.C MedRes.C Move.C PathFinder.C PlayField.C StaticImage.C main.C MapDelta.C Map.C MoveSequence.C ModalLabel.C HtmlPrinter.C Bookmark.C 

noinst_HEADERS  = HtmlPrinter.H Bookmark.H HiRes.H History.H ImageData.H LevelMap.H LowRes.H MainWindow.H MedRes.H Move.H PathFinder.H PlayField.H Queue.H StaticImage.H MapDelta.H ModalLabel.H MapDelta.H Map.H MoveSequence.H

METASOURCES= MainWindow.moc ModalLabel.moc PlayField.moc

INCLUDES = $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KDEUI) $(LIBZ)

EXTRA_DIST=AUTHORS NEWS README TODO

# we need theese deps for the automatic generation of other deps
StaticImage.C: images/data.c
LevelMap.C: levels/data.c
LowRes.C: images/20/data.c
MedRes.C: images/32/data.c
HiRes.C: images/40/data.c

# these are for automatic generation of deps, too
levels/data.c:
	(cd levels && $(MAKE) data.c)
images/data.c:
	(cd images && $(MAKE) data.c)
images/20/data.c:
	(cd images/20 && $(MAKE) data.c)
images/32/data.c:
	(cd images/32 && $(MAKE) data.c)
images/40/data.c:
	(cd images/40 && $(MAKE) data.c)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.C && mv messages.po ../po/ksokoban.pot
