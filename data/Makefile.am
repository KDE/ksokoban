icon_DATA = ksokoban.xpm
icondir = $(kde_icondir)

apps_DATA = ksokoban.kdelnk
appsdir = $(kde_appsdir)/Games

html_DATA = index.html index-1.html index-2.html index-3.html logotp3.gif
htmldir = $(kde_htmldir)/en/ksokoban

EXTRA_DIST = $(icon_DATA) $(apps_DATA) $(html_DATA) index.sgml index.nif mini-ksokoban.xpm

#all: index.html
#
#$(html_DATA): index.sgml
#	ksgml2html $< en

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-ksokoban.xpm $(kde_minidir)/ksokoban.xpm

uninstall-local:
	rm -f $(kde_minidir)/ksokoban.xpm
	rm -rf $(htmldir)
