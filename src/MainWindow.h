/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Bookmark.h"
#include "InternalCollections.h"

#include <KXmlGuiWindow>

#include <QUrl>

class PlayField;
class LevelCollection;
class KSelectAction;
class QAction;
class QFocusEvent;
class QDragEnterEvent;
class QDropEvent;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow() override;

    void openURL(const QUrl &_url);

public Q_SLOTS:
    void changeCollection(int id);
    void setBookmark(QAction *action);
    void goToBookmark(QAction *action);

    void loadLevels();

protected:
    void focusInEvent(QFocusEvent *) override;
    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;

    void createCollectionMenu();

private:
    void updateBookmark(int num);
    void handleAnimSpeedSelected(QAction *action);
    void setupActions();

private:
    InternalCollections internalCollections_;
    LevelCollection *externalCollection_;
    PlayField *playField_;
    Bookmark *bookmarks_[10];

    QAction *qa_slow, *qa_medium, *qa_fast, *qa_off, *setBM_act[10], *goToBM_act[10];
    KSelectAction *collectionsAct_;
};

#endif /* MAINWINDOW_H */
