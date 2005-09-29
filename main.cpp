/*
 *  ksokoban - a Sokoban game for KDE
 *  Copyright (C) 1998-2000  Anders Widell  <awl@passagen.se>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// #include <kuniqueapplication.h>
#include <kapplication.h>
#include <kimageio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "MainWindow.h"


static const char description[] = I18N_NOOP("The japanese warehouse keeper game");

static const char version[] = "0.4.2";


static KCmdLineOptions options[] =
{
     { "+[file]", I18N_NOOP("Level collection file to load"), 0 },
     KCmdLineLastOption  // End of options.
};


int
main (int argc, char **argv)
{
  KAboutData aboutData("ksokoban", I18N_NOOP("KSokoban"),
		       version, description, KAboutData::License_GPL,
		       "(c) 1998-2001  Anders Widell", 0,
		       "http://hem.passagen.se/awl/ksokoban/");
  aboutData.addAuthor("Anders Widell", 0,
		      "awl@passagen.se",
		      "http://hem.passagen.se/awl/");
  aboutData.addCredit("David W. Skinner",
		      I18N_NOOP("For contributing the Sokoban levels included in this game"),
		      "sasquatch@bentonrea.com",
		      "http://users.bentonrea.com/~sasquatch/");
  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions(options);
//   KUniqueApplication::addCmdLineOptions();

//   if (!KUniqueApplication::start())
//     return 0;

  QApplication::setColorSpec(QApplication::ManyColor);

//   KUniqueApplication app;
  KApplication app;
//   KImageIO::registerFormats();

  MainWindow *widget = new MainWindow();
  app.setMainWidget(widget);
  widget->show();

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if (args->count() > 0) {
    widget->openURL(args->url(0));
  }
  args->clear();

  QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

  int rc = app.exec();

//   delete widget;

  return rc;
}
