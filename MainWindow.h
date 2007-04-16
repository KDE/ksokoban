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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kxmlguiwindow.h>
#include <kurl.h>
#include "Bookmark.h"
#include "InternalCollections.h"

class PlayField;
class QFocusEvent;
class QDragEnterEvent;
class QDropEvent;
class LevelCollection;
class KSelectAction;

// NOTE: make this app-wide constant?
// if so, then bookmarks menu can't be constructed in ksokobanui.rc -
// will need to create it dynamically instead
#define NUM_BOOKMARKS 10

class MainWindow : public KXmlGuiWindow {
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

  void openUrl(KUrl _url);

private slots:
  void changeCollection(int id);
  void slotAnimSpeedSelected(QAction*);
  void slotSetBookmark(QAction* bm);
  void slotGotoBookmark(QAction* bm);
  void loadLevels();

protected:
  void focusInEvent(QFocusEvent*);
  void createCollectionMenu();
  virtual void dragEnterEvent(QDragEnterEvent*);
  virtual void dropEvent(QDropEvent*);

private:
  void setupActions();
  void updateBookmark(int num);

  InternalCollections internalCollections_;
  LevelCollection *externalCollection_;
  PlayField       *playField_;
  Bookmark        *bookmarks_[NUM_BOOKMARKS];
  KSelectAction   *collectionsAct_;
};

#endif  /* MAINWINDOW_H */
