/*
    SPDX-FileCopyrightText: 1998-2000 Anders Widell <awl@passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "MainWindow.h"
#include "version.h"

#include <KAboutData>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("ksokoban");

    KAboutData aboutData(QStringLiteral("ksokoban"),
                         i18n("KSokoban"),
                         QStringLiteral(KSOKOBAN_VERSION_STRING),
                         i18n("The japanese warehouse keeper game"),
                         KAboutLicense::GPL,
                         i18n("(c) 1998 Anders Widell <awl@hem.passagen.se>\n(c) 2012 Lukasz Kalamlacki"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/ksokoban"));
    aboutData.addAuthor(i18n("Shlomi Fish"),
                        i18n("For porting to Qt5/KF5 and doing other cleanups"),
                        QStringLiteral("shlomif@cpan.org"),
                        QStringLiteral("http://www.shlomifish.org/"));
    aboutData.addAuthor(i18n("Lukasz Kalamlacki"),
                        i18n("For rewriting the original ksokoban game from kde3 to kde4"),
                        QStringLiteral("kalamlacki@gmail.com"),
                        QStringLiteral("http://sf.net/projects/ksokoban"));
    aboutData.addAuthor(i18n("Anders Widell"),
                        i18n("For writing the original ksokoban game"),
                        QStringLiteral("awl@hem.passagen.se"),
                        QStringLiteral("http://hem.passagen.se/awl/ksokoban/"));
    aboutData.addCredit(i18n("David W. Skinner"),
                        i18n("For contributing the Sokoban levels included in this game"),
                        QStringLiteral("sasquatch@bentonrea.com"),
                        QStringLiteral("http://users.bentonrea.com/~sasquatch/"));

    KAboutData::setApplicationData(aboutData);

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("ksokoban")));

    QCommandLineParser parser;
    parser.addPositionalArgument(i18n("[file]"), i18n("Level collection file to load"));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    MainWindow *widget = new MainWindow();
    widget->show();

    if (parser.positionalArguments().count() > 0) {
        widget->openURL(parser.positionalArguments().at(0));
    }

    return app.exec();
}
