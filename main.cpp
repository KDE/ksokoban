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


#include <KAboutData>
#include <KAboutData>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "MainWindow.h"


static const char version[] = "0.5.0";


int
main (int argc, char **argv)
{
  QApplication app(argc, argv);

  KAboutData aboutData("ksokoban", "ksokoban",
		       version,
		       "The japanese warehouse keeper game",
		       KAboutLicense::GPL,
		       "(c) 1998 Anders Widell <awl@hem.passagen.se>\n(c) 2012 Lukasz Kalamlacki",
               QString(),
               "http://www.shlomifish.org/open-source/projects/ksokoban/",
               "shlomif@cpan.org"
               );
  aboutData.addAuthor("Shlomi Fish", "For porting to Qt5/KF5 and doing other cleanups",
              "shlomif@cpan.org",
              "http://www.shlomifish.org/");
  aboutData.addAuthor("Lukasz Kalamlacki", "For rewriting the original ksokoban game from kde3 to kde4",
		      "kalamlacki@gmail.com",
		      "http://sf.net/projects/ksokoban");
  aboutData.addAuthor("Anders Widell", "For writing the original ksokoban game",
		      "awl@hem.passagen.se",
		      "http://hem.passagen.se/awl/ksokoban/");
  aboutData.addCredit("David W. Skinner",
		      "For contributing the Sokoban levels included in this game",
		      "sasquatch@bentonrea.com",
		      "http://users.bentonrea.com/~sasquatch/");
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
  parser.addPositionalArgument(QLatin1String("[file]"), "Level collection file to load");

  QApplication::setColorSpec(QApplication::ManyColor);



  MainWindow *widget = new MainWindow();
  widget->show();

  if (parser.positionalArguments().count() > 0) {
    widget->openURL(parser.positionalArguments().at(0));
  }


  QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));


  return app.exec();
}
