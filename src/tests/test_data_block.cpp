// © SigViewer developers
//
// License: GPL-3.0

#include <QtTest>
#include <cmath>

#include "base/fixed_data_block.h"
#include "base/sigviewer_user_types.h"

using namespace sigviewer;

class TestDataBlock : public QObject {
    Q_OBJECT

   private slots:
    void basicProperties() {
        QSharedPointer<QVector<float32>> data(new QVector<float32>);
        for (unsigned i = 1; i <= 10; i++) data->push_back(i);

        FixedDataBlock block(data, 10);
        QCOMPARE(block.size(), static_cast<unsigned>(data->size()));
        QCOMPARE(block.getMax(), 10.0f);
        QCOMPARE(block.getMin(), 1.0f);
        for (unsigned i = 0; i < 10; i++)
            QCOMPARE(block[i], static_cast<float32>(i + 1));
    }

    void mean() {
        QSharedPointer<QVector<float32>> data(new QVector<float32>);
        QSharedPointer<QVector<float32>> reverseData(new QVector<float32>);
        for (unsigned i = 1; i <= 10; i++) {
            data->push_back(i);
            reverseData->push_back(11 - i);
        }

        QSharedPointer<DataBlock const> block1(new FixedDataBlock(data, 10));
        QSharedPointer<DataBlock const> block2(new FixedDataBlock(data, 10));
        QSharedPointer<DataBlock const> blockRev(new FixedDataBlock(reverseData, 10));

        // Identical blocks: mean equals the block itself
        std::list<QSharedPointer<DataBlock const>> sameBlocks = {block1, block2};
        auto mean = FixedDataBlock::calculateMean(sameBlocks);
        QCOMPARE(mean->size(), 10u);
        for (unsigned x = 0; x < 10; x++)
            QCOMPARE((*mean)[x], static_cast<float32>(x + 1));

        // Mixed blocks: mean is 5.5 everywhere
        std::list<QSharedPointer<DataBlock const>> mixedBlocks = {block1, blockRev};
        auto meanMixed = FixedDataBlock::calculateMean(mixedBlocks);
        QCOMPARE(meanMixed->size(), 10u);
        for (unsigned x = 0; x < 10; x++) QCOMPARE((*meanMixed)[x], 5.5f);
    }

    void standardDeviation() {
        QSharedPointer<QVector<float32>> data(new QVector<float32>);
        QSharedPointer<QVector<float32>> reverseData(new QVector<float32>);
        for (unsigned i = 1; i <= 10; i++) {
            data->push_back(i);
            reverseData->push_back(11 - i);
        }

        QSharedPointer<DataBlock const> block1(new FixedDataBlock(data, 10));
        QSharedPointer<DataBlock const> block2(new FixedDataBlock(data, 10));
        QSharedPointer<DataBlock const> blockRev(new FixedDataBlock(reverseData, 10));

        // Identical blocks → zero standard deviation
        std::list<QSharedPointer<DataBlock const>> sameBlocks = {block1, block2};
        auto stdDev = FixedDataBlock::calculateStandardDeviation(sameBlocks);
        QCOMPARE(stdDev->size(), 10u);
        for (unsigned x = 0; x < 10; x++) QCOMPARE((*stdDev)[x], 0.0f);

        // Mixed blocks → standard deviation of {x+1, 10-x} around mean 5.5
        std::list<QSharedPointer<DataBlock const>> mixedBlocks = {block1, blockRev};
        auto stdDevMixed = FixedDataBlock::calculateStandardDeviation(mixedBlocks);
        QCOMPARE(stdDevMixed->size(), 10u);
        for (unsigned x = 0; x < 10; x++) {
            float32 expected = std::sqrt(
                (std::pow((*block1)[x] - 5.5f, 2) + std::pow((*blockRev)[x] - 5.5f, 2)) / 2.0f);
            QCOMPARE((*stdDevMixed)[x], expected);
        }
    }
};

QTEST_GUILESS_MAIN(TestDataBlock)
#include "test_data_block.moc"
