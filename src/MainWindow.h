/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KMainWindow>
#include <KHelpMenu>
#include <QUrl>
#include "Bookmark.h"
#include "InternalCollections.h"

class PlayField;
class QMenu;
class QAction;
class QFocusEvent;
class QDragEnterEvent;
class QDropEvent;
class LevelCollection;

class MainWindow : public KMainWindow {
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow() override;

  void openURL(const QUrl &_url);

public Q_SLOTS:
  void changeCollection(int id);
  void updateAnimMenu(int id);
  void setBookmark(int id);
  void goToBookmark(int id);

  void loadLevels();

protected:
  void focusInEvent(QFocusEvent*) override;
  void createCollectionMenu(QMenu* collection_);
  //virtual void dragEnterEvent(QDragEnterEvent*);
  void dropEvent(QDropEvent*) override;

private:
  InternalCollections internalCollections_;
  LevelCollection *externalCollection_;
  QMenuBar        *menu_;
  PlayField       *playField_;
  Bookmark        *bookmarks_[10];
  int              currentCollection_;

  QMenu      *game_;
  QMenu      *collection_;
  QMenu      *animation_;
  QMenu      *bookmarkMenu_;
  QMenu      *setBM_;
  QMenu      *goToBM_;
  QAction    *qa_slow, *qa_medium, *qa_fast, *qa_off, *setBM_act[10], *goToBM_act[10], **level_act;
  KHelpMenu        *help_;
  int              checkedCollection_;
  int              checkedAnim_;

  void updateBookmark(int num);

  void updateAnim(int val);
};

#endif  /* MAINWINDOW_H */
