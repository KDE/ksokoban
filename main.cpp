/*
 *  ksokoban - a Sokoban game by KDE
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


#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <KAboutData>
#include <KLocalizedString>

#include "MainWindow.h"


static const char version[] = "0.5.0";


int
main (int argc, char **argv)
{
  QApplication app(argc, argv);
  KLocalizedString::setApplicationDomain("ksokoban");

  KAboutData aboutData(QStringLiteral("ksokoban"), i18n("ksokoban"),
		       version,
		       i18n("The japanese warehouse keeper game"),
		       KAboutLicense::GPL,
		       i18n("(c) 1998 Anders Widell <awl@hem.passagen.se>\n(c) 2012 Lukasz Kalamlacki"),
               QString(),
               QStringLiteral("http://www.shlomifish.org/open-source/projects/ksokoban/")
               );
  aboutData.addAuthor(i18n("Shlomi Fish"), i18n("For porting to Qt5/KF5 and doing other cleanups"),
              QStringLiteral("shlomif@cpan.org"),
              QStringLiteral("http://www.shlomifish.org/"));
  aboutData.addAuthor(i18n("Lukasz Kalamlacki"), i18n("For rewriting the original ksokoban game from kde3 to kde4"),
		      QStringLiteral("kalamlacki@gmail.com"),
		      QStringLiteral("http://sf.net/projects/ksokoban"));
  aboutData.addAuthor(i18n("Anders Widell"), i18n("For writing the original ksokoban game"),
		      QStringLiteral("awl@hem.passagen.se"),
		      QStringLiteral("http://hem.passagen.se/awl/ksokoban/"));
  aboutData.addCredit(i18n("David W. Skinner"),
		      i18n("For contributing the Sokoban levels included in this game"),
		      QStringLiteral("sasquatch@bentonrea.com"),
		      QStringLiteral("http://users.bentonrea.com/~sasquatch/"));

  aboutData.setOrganizationDomain(QByteArray("kde.org"));
  aboutData.setDesktopFileName(QStringLiteral("org.kde.ksokoban"));

  KAboutData::setApplicationData(aboutData);

  app.setWindowIcon(QIcon::fromTheme(QStringLiteral("ksokoban")));

  QCommandLineParser parser;
  parser.addVersionOption();
  parser.addHelpOption();
  parser.addPositionalArgument(i18n("[file]"), i18n("Level collection file to load"));
  aboutData.setupCommandLine(&parser);
  parser.process(app);
  aboutData.processCommandLine(&parser);


  MainWindow *widget = new MainWindow();
  widget->show();

  if (parser.positionalArguments().count() > 0) {
    widget->openURL(parser.positionalArguments().at(0));
  }


  QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));


  return app.exec();
}
