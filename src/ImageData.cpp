/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ImageData.h"

#include <QColor>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QPixmap>

#include <cassert>

ImageData::ImageData()
{
    random.seed(QRandomGenerator::global()->generate());
}

ImageData::~ImageData() = default;

void ImageData::expandIndex(int size)
{
    size++;
    assert(size < 2500);

    upperLargeIndex_.resize(size);
    lowerLargeIndex_.resize(size);
    leftSmallIndex_.resize(size);
    rightSmallIndex_.resize(size);

    for (int i = indexSize_; i < size; i++) {
        upperLargeIndex_[i] = random.bounded(LARGE_STONES);
        lowerLargeIndex_[i] = random.bounded(LARGE_STONES);
        leftSmallIndex_[i] = random.bounded(SMALL_STONES);
        rightSmallIndex_[i] = random.bounded(SMALL_STONES);
    }

    indexSize_ = size;
}

const QPixmap &ImageData::upperLarge(int index)
{
    assert(index >= 0);
    if (indexSize_ <= index)
        expandIndex(index);
    return largeStone_xpm_[(unsigned char)upperLargeIndex_[index]];
}

const QPixmap &ImageData::lowerLarge(int index)
{
    assert(index >= 0);
    if (indexSize_ <= index)
        expandIndex(index);
    return largeStone_xpm_[(unsigned char)lowerLargeIndex_[index]];
}

const QPixmap &ImageData::leftSmall(int index)
{
    assert(index >= 0);
    if (indexSize_ <= index)
        expandIndex(index);
    return smallStone_xpm_[(unsigned char)leftSmallIndex_[index]];
}

const QPixmap &ImageData::rightSmall(int index)
{
    assert(index >= 0);
    if (indexSize_ <= index)
        expandIndex(index);
    return smallStone_xpm_[(unsigned char)rightSmallIndex_[index]];
}

int ImageData::resize(int size)
{
    assert(size > 0);
    size &= ~1u;
    if (size == size_)
        return size;

    size_ = size;
    halfSize_ = size / 2;

    const qreal dpr = qApp->devicePixelRatio();
    const int deviceSize_ = size_ * dpr;
    halfdeviceSize_ = deviceSize_ / 2;

    for (int i = 0; i < SMALL_STONES; i++) {
        image2pixmap(images_[i].scaled(halfdeviceSize_, halfdeviceSize_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), smallStone_xpm_[i], dpr);
        //     smallStone_xpm_[i].convertFromImage(images_[i].smoothScale(halfSize_, halfSize_),
        //     QPixmap::ColorOnly|QPixmap::DiffuseDither|QPixmap::DiffuseAlphaDither|QPixmap::AvoidDither);
    }

    for (int i = 0; i < LARGE_STONES; i++) {
        image2pixmap(images_[SMALL_STONES + i].scaled(deviceSize_, halfdeviceSize_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), largeStone_xpm_[i], dpr);
        //     largeStone_xpm_[i].convertFromImage(images_[SMALL_STONES+i].smoothScale(size_, halfSize_) ,
        //     QPixmap::ColorOnly|QPixmap::DiffuseDither|QPixmap::DiffuseAlphaDither|QPixmap::AvoidDither);
    }

    objectImg_ = images_[SMALL_STONES + LARGE_STONES].scaled(deviceSize_, deviceSize_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Use copy() because if the size is not changed, smoothScale is not
    // really a copy
    // Use {[Geometry] height=753 width=781} to test

    if (objectImg_.width() == deviceSize_)
        objectImg_ = objectImg_.copy();

    image2pixmap(objectImg_, otherPixmaps_[0], dpr, false);
    brighten(objectImg_);
    image2pixmap(objectImg_, brightObject_, dpr, false);

    QImage img = images_[SMALL_STONES + LARGE_STONES + 1].scaled(deviceSize_, deviceSize_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (img.width() == deviceSize_)
        img = img.copy();

    image2pixmap(img, otherPixmaps_[1], dpr, false);
    brighten(img);
    image2pixmap(img, brightTreasure_, dpr, false);

    for (int i = 2; i < OTHER_IMAGES; i++) {
        image2pixmap(images_[SMALL_STONES + LARGE_STONES + i].scaled(deviceSize_, deviceSize_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), otherPixmaps_[i], dpr);
        //     otherPixmaps_[i].convertFromImage(images_[SMALL_STONES+LARGE_STONES+i].smoothScale(size_, size_),
        //     QPixmap::ColorOnly|QPixmap::OrderedDither|QPixmap::OrderedAlphaDither|QPixmap::AvoidDither);
    }
    return size_;
}

// Don't use DiffuseDither for the objects on the "floor" since
// it gives spurious dots on the floor around them

void ImageData::image2pixmap(const QImage &img, QPixmap &xpm, qreal dpr, bool diffuse)
{
    xpm = QPixmap::fromImage(img,
                             (diffuse ? (Qt::DiffuseDither | Qt::DiffuseAlphaDither) : (Qt::OrderedDither | Qt::OrderedAlphaDither)) | Qt::ColorOnly
                                 | Qt::AvoidDither);
    xpm.setDevicePixelRatio(dpr);
}

void ImageData::brighten(QImage &img)
{
    assert(img.depth() == 32);

    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            QRgb rgb = img.pixel(x, y);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);

            if (r > g && r > b) {
                // only modify redish pixels

                QColor col(r, g, b);
                QColor lcol = col.lighter(130);

                img.setPixel(x, y, lcol.rgb());
            }
        }
    }
}

void ImageData::wall(QPainter &p, int x, int y, int index, bool left, bool right)
{
    if (left)
        p.drawPixmap(x, y, upperLarge(index - 1), halfdeviceSize_, 0, -1, -1);
    else
        p.drawPixmap(x, y, leftSmall(index));

    if (right)
        p.drawPixmap(x + halfSize_, y, upperLarge(index), 0, 0, halfdeviceSize_, -1);
    else
        p.drawPixmap(x + halfSize_, y, rightSmall(index));

    p.drawPixmap(x, y + halfSize_, lowerLarge(index));
}

void ImageData::floor(QPainter &p, int x, int y)
{
    p.fillRect(x, y, size_, size_, QColor(0x67, 0x67, 0x67, 255));
}

void ImageData::goal(QPainter &p, int x, int y)
{
    p.drawPixmap(x, y, otherPixmaps_[2]);
}

void ImageData::man(QPainter &p, int x, int y)
{
    p.drawPixmap(x, y, otherPixmaps_[3]);
}

void ImageData::object(QPainter &p, int x, int y)
{
    p.drawPixmap(x, y, otherPixmaps_[0]);
}

void ImageData::saveman(QPainter &p, int x, int y)
{
    p.drawPixmap(x, y, otherPixmaps_[4]);
}

void ImageData::treasure(QPainter &p, int x, int y)
{
    p.drawPixmap(x, y, otherPixmaps_[1]);
}

void ImageData::brightObject(QPainter &p, int x, int y)
{
    p.drawPixmap(x, y, brightObject_);
}

void ImageData::brightTreasure(QPainter &p, int x, int y)
{
    p.drawPixmap(x, y, brightTreasure_);
}
