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
#include <QDir>

int main(int argc, char **argv)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("skladnik");

    KAboutData aboutData(QStringLiteral("skladnik"),
                         i18n("Skladnik"),
                         QStringLiteral(SKLADNIK_VERSION_STRING),
                         i18n("The japanese warehouse keeper game"),
                         KAboutLicense::GPL,
                         i18n("(c) 1998 Anders Widell <awl@hem.passagen.se>\n(c) 2012 Lukasz Kalamlacki"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/skladnik"));
    aboutData.addAuthor(i18n("Shlomi Fish"),
                        i18n("For porting to Qt5/KF5 and doing other cleanups"),
                        QStringLiteral("shlomif@cpan.org"),
                        QStringLiteral("https://www.shlomifish.org/"));
    aboutData.addAuthor(i18n("Lukasz Kalamlacki"),
                        i18n("For rewriting the original ksokoban game from kde3 to kde4"),
                        QStringLiteral("kalamlacki@gmail.com"),
                        QStringLiteral("https://sourceforge.net/projects/ksokoban/"));
    aboutData.addAuthor(i18n("Anders Widell"),
                        i18n("For writing the original ksokoban game"),
                        QStringLiteral("awl@hem.passagen.se"),
                        QStringLiteral("http://hem.passagen.se/awl/ksokoban/"));
    aboutData.addCredit(i18n("David W. Skinner"),
                        i18n("For contributing the Sokoban levels included in this game"),
                        QStringLiteral("sasquatch@bentonrea.com"),
                        QStringLiteral("http://users.bentonrea.com/~sasquatch/"));

    KAboutData::setApplicationData(aboutData);

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("skladnik")));

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("URL"), i18n("Level collection file to load."), QStringLiteral("[URL}"));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    auto *widget = new MainWindow();
    widget->show();

    const QStringList positionalArguments = parser.positionalArguments();
    if (!positionalArguments.isEmpty()) {
        const QString currentPath = QDir::currentPath();
        const QUrl levelFileUrl = QUrl::fromUserInput(positionalArguments.first(), currentPath, QUrl::AssumeLocalFile);
        widget->openURL(levelFileUrl);
    }

    return app.exec();
}
