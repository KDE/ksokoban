APPSDIR = $(kde_appsdir)/Games

SUBDIRS=data images levels

bin_PROGRAMS = ksokoban
ksokoban_SOURCES = HiRes.C History.C LevelMap.C LowRes.C MainWindow.C MedRes.C Move.C PathFinder.C PlayField.C StaticImage.C main.C HiRes.H History.H ImageData.H LevelMap.H LowRes.H MainWindow.H MedRes.H Move.H PathFinder.H PlayField.H Queue.H StaticImage.H MapDelta.C MapDelta.H Map.C Map.H MoveSequence.C MoveSequence.H ModalLabel.C ModalLabel.H HtmlPrinter.C HtmlPrinter.H

SRCMETA=MainWindow.moc.C PlayField.moc.C ModalLabel.moc.C

INCLUDES = @all_includes@

LDFLAGS = @all_libraries@ $(KDE_RPATH)
LDADD = -lkdeui -lkdecore -lqt -lXext -lX11 -lz $(LIBSOCKET)

DISTCLEANFILES = $(SRCMETA)
EXTRA_DIST=TODO ksokoban.kdelnk NEWS

AUTOMAKE_OPTIONS = foreign

# moc dependencies

MainWindow.C: MainWindow.moc.C
MainWindow.moc.C: MainWindow.H
	$(MOC) MainWindow.H -o MainWindow.moc.C

ModalLabel.C: ModalLabel.moc.C
ModalLabel.moc.C: ModalLabel.H
	$(MOC) ModalLabel.H -o ModalLabel.moc.C

PlayField.C: PlayField.moc.C
PlayField.moc.C: PlayField.H
	$(MOC) PlayField.H -o PlayField.moc.C

