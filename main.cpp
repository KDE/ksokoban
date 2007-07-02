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



int
main (int argc, char **argv)
{
  KAboutData aboutData("ksokoban", 0, ki18n("KSokoban"),
		       version, ki18n(description), KAboutData::License_GPL,
		       ki18n("(c) 1998-2001  Anders Widell"), KLocalizedString(),
		       "http://hem.passagen.se/awl/ksokoban/");
  aboutData.addAuthor(ki18n("Anders Widell"), KLocalizedString(),
		      "awl@passagen.se",
		      "http://hem.passagen.se/awl/");
  aboutData.addCredit(ki18n("David W. Skinner"),
		      ki18n("For contributing the Sokoban levels included in this game"),
		      "sasquatch@bentonrea.com",
		      "http://users.bentonrea.com/~sasquatch/");
  KCmdLineArgs::init(argc, argv, &aboutData);

  KCmdLineOptions options;
  options.add("+[file]", ki18n("Level collection file to load"));
  KCmdLineArgs::addCmdLineOptions(options);
//   KUniqueApplication::addCmdLineOptions();

//   if (!KUniqueApplication::start())
//     return 0;

  QApplication::setColorSpec(QApplication::ManyColor);

//   KUniqueApplication app;
  KApplication app;
//

  MainWindow *widget = new MainWindow();
  widget->show();

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if (args->count() > 0) {
    widget->openUrl(args->url(0));
  }
  args->clear();

  QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

  int rc = app.exec();

//   delete widget;

  return rc;
}
