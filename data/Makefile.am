
kde_icon_DATA = ksokoban.xpm

apps_DATA = ksokoban.desktop
appsdir = $(kde_appsdir)/Games

EXTRA_DIST = $(kde_icon_DATA) $(apps_DATA) mini-ksokoban.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-ksokoban.xpm $(kde_minidir)/ksokoban.xpm

uninstall-local:
	rm -f $(kde_minidir)/ksokoban.xpm
	rm -rf $(htmldir)
