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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ktopwidget.h>
#include "Bookmark.H"
#include "InternalCollections.H"

class KMenuBar;
class PlayField;
class QPopupMenu;
class QFocusEvent;
class LevelCollection;

class MainWindow : public KTMainWindow {
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

public slots:
  void changeCollection(int id);
  void updateAnimMenu(int id);
  void setBookmark(int id);
  void goToBookmark(int id);

  void loadLevels();

protected:
  void focusInEvent(QFocusEvent *);
  void createCollectionMenu();

private:
  InternalCollections internalCollections_;
  LevelCollection *externalCollection_;
  KMenuBar        *menu_;
  PlayField       *playField_;
  Bookmark        *bookmarks_[10];
  int              currentCollection_;

  QPopupMenu      *game_;
  QPopupMenu      *collection_;
  QPopupMenu      *animation_;
  QPopupMenu      *bookmarkMenu_;
  QPopupMenu      *setBM_;
  QPopupMenu      *goToBM_;
  QPopupMenu      *help_;
  int              checkedCollection_;
  int              checkedAnim_;

  void updateBookmark(int num);

};

#endif  /* MAINWINDOW_H */
