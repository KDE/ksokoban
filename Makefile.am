
APPSDIR = $(kde_appsdir)/Games



SUBDIRS=images levels

bin_PROGRAMS = ksokoban
ksokoban_SOURCES = HiRes.C History.C LevelMap.C LowRes.C MainWindow.C MedRes.C Move.C PathFinder.C PlayField.C StaticImage.C main.C HiRes.H History.H ImageData.H LevelMap.H LowRes.H MainWindow.H MedRes.H Move.H PathFinder.H PlayField.H Queue.H StaticImage.H MapDelta.C MapDelta.H Map.C Map.H MoveSequence.C MoveSequence.H ModalLabel.C ModalLabel.H HtmlPrinter.C HtmlPrinter.H

BUILT_SOURCES=MainWindow.moc.C PlayField.moc.C ModalLabel.moc.C

INCLUDES = @all_includes@

LDFLAGS = @all_libraries@ $(KDE_RPATH)
LDADD = -lkdeui -lkdecore -lqt -lXext -lX11 -lz $(LIBSOCKET)

DISTCLEANFILES = $(BUILTSOURCES)
EXTRA_DIST=TODO ksokoban.kdelnk NEWS

export ksokoban_orig_CXXFLAGS = @ksokoban_orig_CXXFLAGS@

%.moc.C: %.H
	$(MOC) $^ -o $@

AUTOMAKE_OPTIONS = foreign

install-data-local:
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) ksokoban.kdelnk $(APPSDIR)


uninstall-local:
	rm -f $(APPSDIR)/ksokoban.kdelnk
	rmdir $(APPSDIR) || true

