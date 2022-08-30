/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMAGEDATA_H
#define IMAGEDATA_H

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
    void floor(QPainter &p, int x, int y);
    void goal(QPainter &p, int x, int y);
    void man(QPainter &p, int x, int y);
    void object(QPainter &p, int x, int y);
    void saveman(QPainter &p, int x, int y);
    void treasure(QPainter &p, int x, int y);
    void brightObject(QPainter &p, int x, int y);
    void brightTreasure(QPainter &p, int x, int y);

    const QImage &objectImg() const
    {
        return objectImg_;
    }

protected:
    ImageData();

    void expandIndex(int size);
    void image2pixmap(const QImage &img, QPixmap &xpm, qreal dpr, bool diffuse = true);
    void brighten(QImage &img);

    const QPixmap &upperLarge(int index);
    const QPixmap &lowerLarge(int index);
    const QPixmap &leftSmall(int index);
    const QPixmap &rightSmall(int index);

protected:
    static constexpr int SMALL_STONES = 4;
    static constexpr int LARGE_STONES = 6;
    static constexpr int OTHER_IMAGES = 5;
    static constexpr int NO_OF_IMAGES = (SMALL_STONES + LARGE_STONES + OTHER_IMAGES);

    QImage images_[NO_OF_IMAGES];

    QPixmap smallStone_xpm_[SMALL_STONES];
    QPixmap largeStone_xpm_[LARGE_STONES];
    QPixmap otherPixmaps_[OTHER_IMAGES];
    QPixmap brightObject_;
    QPixmap brightTreasure_;
    QImage objectImg_;

    int indexSize_ = 0;
    QByteArray upperLargeIndex_;
    QByteArray lowerLargeIndex_;
    QByteArray leftSmallIndex_;
    QByteArray rightSmallIndex_;

    int size_ = 0;
    int halfSize_  = 0;
    int halfdeviceSize_ = 0;
};

#endif /* IMAGEDATA_H */
