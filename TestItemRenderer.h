#pragma once

class TestItemRenderer : public QQuickRhiItemRenderer {
public:
    TestItemRenderer() = default;
    ~TestItemRenderer() override = default;

protected:
    void initialize(QRhiCommandBuffer* commandBuffer) override;
    void render(QRhiCommandBuffer* commandBuffer) override;
    void synchronize(QQuickRhiItem* item) override {}

private:
    std::unique_ptr<QRhiBuffer> indexBuf_;
    std::unique_ptr<QRhiBuffer> positionBuf_;
    std::unique_ptr<QRhiBuffer> uniformBuf_;
    std::unique_ptr<QRhiBuffer> colorBuf_;
    std::unique_ptr<QRhiShaderResourceBindings> shaderResourceBindings_;
    std::unique_ptr<QRhiGraphicsPipeline> pipeline_;

    QRhi* rhi_ = nullptr;
};
