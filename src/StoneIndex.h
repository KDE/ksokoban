/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef STONEINDEX_H
#define STONEINDEX_H

#include <QVector>

class StoneIndex
{
public:
    StoneIndex();

public:
    void setStoneCount(int largeStoneCount, int smallStoneCount);

    int upperLarge(int index) const;
    int lowerLarge(int index) const;
    int leftSmall(int index) const;
    int rightSmall(int index) const;

private:
    void expandIndex(int size) const;

private:
    int m_largeStoneCount = 0;
    int m_smallStoneCount = 0;

    mutable int m_indexSize = 0;
    mutable QVector<uchar> m_upperLargeIndex;
    mutable QVector<uchar> m_lowerLargeIndex;
    mutable QVector<uchar> m_leftSmallIndex;
    mutable QVector<uchar> m_rightSmallIndex;
};

#endif
