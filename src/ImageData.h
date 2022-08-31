/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include "StoneIndex.h"

#include <QImage>
#include <QPixmap>

class QPainter;

class ImageData
{
public:
    virtual ~ImageData();

public:
    int resize(int size);
    int size() const
    {
        return size_;
    }

    void wall(QPainter &p, int x, int y, int index, bool left, bool right);

protected:
    ImageData();

    void image2pixmap(const QImage &img, QPixmap &xpm, qreal dpr, bool diffuse = true);

    const QPixmap &upperLarge(int index);
    const QPixmap &lowerLarge(int index);
    const QPixmap &leftSmall(int index);
    const QPixmap &rightSmall(int index);

protected:
    static constexpr int SMALL_STONES = 4;
    static constexpr int LARGE_STONES = 6;
    static constexpr int NO_OF_IMAGES = (SMALL_STONES + LARGE_STONES);

    QImage images_[NO_OF_IMAGES];

    QPixmap smallStone_xpm_[SMALL_STONES];
    QPixmap largeStone_xpm_[LARGE_STONES];

    StoneIndex stoneIndex_;

    int size_ = 0;
    int halfSize_  = 0;
    int halfdeviceSize_ = 0;
};

#endif /* IMAGEDATA_H */
