/*
 *  ksokoban - a Sokoban game for KDE
 *  Copyright (C) 1998  Anders Widell  <d95-awi@nada.kth.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <kapp.h>
#include <kimageio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "MainWindow.H"


static const char *description = I18N_NOOP("KDE Game");

static const char *version = "v0.0.1";


int
main (int argc, char **argv)
{
  KAboutData aboutData( "ksokoban", I18N_NOOP("KSokoban"), 
    version, description, KAboutData::License_GPL, 
    "(c) 1998, Anders Widell");
  aboutData.addAuthor("Anders Widell",0, "d95-awi@nada.kth.se");
  KCmdLineArgs::init( argc, argv, &aboutData );

  QApplication::setColorSpec(QApplication::ManyColor);

  KApplication app;
  KImageIO::registerFormats();

  MainWindow *widget = new MainWindow();
  app.setMainWidget(widget);
  widget->show();

  int rc = app.exec();

  delete widget;

  return rc;
}
