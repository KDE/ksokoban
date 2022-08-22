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

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

public:
    void openURL(const QUrl &_url);

public Q_SLOTS:
    void changeCollection(int id);
    void setBookmark(QAction *action);
    void goToBookmark(QAction *action);

    void loadLevels();

public:
    QSize sizeHint() const override;

protected:
    void focusInEvent(QFocusEvent *) override;
    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;

private:
    void updateBookmark(int num);
    void handleAnimSpeedSelected(QAction *action);
    void setupActions();
    void createCollectionMenu();

private:
    InternalCollections internalCollections_;
    LevelCollection *externalCollection_ = nullptr;
    PlayField *playField_;
    Bookmark *bookmarks_[10];

    QAction *qa_slow;
    QAction *qa_medium;
    QAction *qa_fast;
    QAction *qa_off;
    QAction *setBM_act[10];
    QAction *goToBM_act[10];
    KSelectAction *collectionsAct_;
};

#endif /* MAINWINDOW_H */
