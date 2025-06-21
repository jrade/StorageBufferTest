#pragma once

#include "TestItemRenderer.h"

class TestItem : public QQuickRhiItem {
public:
    TestItem() = default;
    ~TestItem() override = default;

protected:
    QQuickRhiItemRenderer* createRenderer() override { return new TestItemRenderer; }
};
