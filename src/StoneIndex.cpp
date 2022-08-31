/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "StoneIndex.h"

#include <QRandomGenerator>

#include <cassert>

StoneIndex::StoneIndex() = default;

void StoneIndex::setStoneCount(int largeStoneCount, int smallStoneCount)
{
    if ((m_largeStoneCount == largeStoneCount) &&
        (m_smallStoneCount == smallStoneCount)) {
        return;
    }

    m_largeStoneCount = largeStoneCount;
    m_smallStoneCount = smallStoneCount;

    // reset data
    m_indexSize = 0;
    m_upperLargeIndex.clear();
    m_lowerLargeIndex.clear();
    m_leftSmallIndex.clear();
    m_rightSmallIndex.clear();
}

void StoneIndex::expandIndex(int size) const
{
    size++;
    assert(size < 2500);

    m_upperLargeIndex.resize(size);
    m_lowerLargeIndex.resize(size);
    m_leftSmallIndex.resize(size);
    m_leftSmallIndex.resize(size);

    QRandomGenerator *random = QRandomGenerator::global();

    for (int i = m_indexSize; i < size; i++) {
        m_upperLargeIndex[i] = random->bounded(m_largeStoneCount);
        m_lowerLargeIndex[i] = random->bounded(m_largeStoneCount);
        m_leftSmallIndex[i] = random->bounded(m_smallStoneCount);
        m_leftSmallIndex[i] = random->bounded(m_smallStoneCount);
    }

    m_indexSize = size;
}

int StoneIndex::upperLarge(int index) const
{
    assert(index >= 0);
    if (m_indexSize <= index)
        expandIndex(index);
    return m_upperLargeIndex[index];
}

int StoneIndex::lowerLarge(int index) const
{
    assert(index >= 0);
    if (m_indexSize <= index)
        expandIndex(index);
    return m_lowerLargeIndex[index];
}

int StoneIndex::leftSmall(int index) const
{
    assert(index >= 0);
    if (m_indexSize <= index)
        expandIndex(index);
    return m_leftSmallIndex[index];
}

int StoneIndex::rightSmall(int index) const
{
    assert(index >= 0);
    if (m_indexSize <= index)
        expandIndex(index);
    return m_leftSmallIndex[index];
}
