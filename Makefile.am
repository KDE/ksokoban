APPSDIR = $(kde_appsdir)/Games

SUBDIRS=data images levels

bin_PROGRAMS = ksokoban
ksokoban_SOURCES = HiRes.C History.C LevelMap.C LowRes.C MainWindow.C MedRes.C Move.C PathFinder.C PlayField.C StaticImage.C main.C HiRes.H History.H ImageData.H LevelMap.H LowRes.H MainWindow.H MedRes.H Move.H PathFinder.H PlayField.H Queue.H StaticImage.H MapDelta.C MapDelta.H Map.C Map.H MoveSequence.C MoveSequence.H ModalLabel.C ModalLabel.H HtmlPrinter.C HtmlPrinter.H Bookmark.C Bookmark.H

BUILT_SOURCES=MainWindow.moc.C PlayField.moc.C ModalLabel.moc.C

INCLUDES = $(all_includes)

LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KDEUI) $(LIBZ)

DISTCLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST=AUTHORS NEWS README TODO

# moc dependencies

MainWindow.C: MainWindow.moc.C

ModalLabel.C: ModalLabel.moc.C

PlayField.C: PlayField.moc.C

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

# Rule for building moc-files
%.moc.C: %.H
	$(MOC) $< -o $@

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.C && mv messages.po ../po/ksokoban.pot
