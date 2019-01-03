/*
 *  ksokoban - a Sokoban game by KDE
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

#include "MoveSequence.h"
#include "LevelMap.h"
#include "Move.h"

MoveSequence::MoveSequence (Move *_move, LevelMap *_map, bool _undo) {
  assert (_move->finished_);

  move_ = _move;
  map_ = _map;
  undo_ = _undo;

  if (undo_) {
    pos_ = move_->moveIndex_-2;

    xDest_ = x_ = move_->moves_[move_->moveIndex_-1]&0x7f;
    yDest_ = y_ = (move_->moves_[move_->moveIndex_-1]>>8)&0x7f;
  } else {
    pos_ = 1;

    xDest_ = x_ = move_->moves_[0]&0x7f;
    yDest_ = y_ = (move_->moves_[0]>>8)&0x7f;
  }

  newStep ();
}

bool
MoveSequence::newStep () {
  if (pos_>=move_->moveIndex_ || pos_<0) return false;

  xDest_ = move_->moves_[pos_]&0x7f;
  yDest_ = (move_->moves_[pos_]>>8)&0x7f;
  if (undo_) push_ = (move_->moves_[pos_+1]&0x80)==0x80;
  else       push_ = (move_->moves_[pos_]&0x80)==0x80;

  xd_ = yd_ = 0;
  if (xDest_ < x_) xd_ = -1;
  if (xDest_ > x_) xd_ =  1;
  if (yDest_ < y_) yd_ = -1;
  if (yDest_ > y_) yd_ =  1;

  if (undo_) pos_--;
  else       pos_++;

  return true;
}

bool
MoveSequence::next () {
  if (x_ == xDest_ && y_ == yDest_ && !newStep ()) return false;

  x_ += xd_;
  y_ += yd_;

  if (undo_) {
    if (push_) return map_->unpush (x_, y_);
    else return map_->unstep (x_, y_);
  } else {
    if (push_) return map_->push (x_, y_);
    else return map_->step (x_, y_);
  }
}
