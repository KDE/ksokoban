
kde_icon_DATA = ksokoban.xpm

apps_DATA = ksokoban.kdelnk
appsdir = $(kde_appsdir)/Games

html_DATA = index.html index-1.html index-2.html index-3.html logotp3.gif
htmldir = $(kde_htmldir)/en/ksokoban

EXTRA_DIST = $(kde_icon_DATA) $(apps_DATA) $(html_DATA) index.sgml index.nif mini-ksokoban.xpm

#all: index.html
#
#$(html_DATA): index.sgml
#	ksgml2html $< en

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-ksokoban.xpm $(kde_icondir)okoban.xpm
	$(mkinstalldirs) $(kde_icondir)/large
	$(INSTALL_DATA) $(srcdir)/large-ksokoban.xpm $(kde_icondir)/large/ksokoban.xpm

uninstall-local:
	rm -f $(kde_icondir)okoban.xpm
	rm -f $(kde_icondir)/large/ksokoban.xpm
	rm -rf $(htmldir)
