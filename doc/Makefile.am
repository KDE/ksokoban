
docs_DATA = index.html index-1.html index-2.html logotp3.gif index-3.html

docsdir = $(kde_htmldir)/en/ksokoban

EXTRA_DIST = $(docs_DATA) index.sgml index.nif

doc: index.sgml index.nif
	ksgml2html index en
