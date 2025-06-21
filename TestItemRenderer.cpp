#include "Pch.h"

#include "TestItemRenderer.h"

static QShader loadShader_(const QString& filePath)
{
    QFile f{filePath};
    if (!f.open(QIODevice::ReadOnly))
        qFatal("Failed to load the shader");
    QShader s = QShader::fromSerialized(f.readAll());
    if (!s.isValid())
        qFatal("Failed to load the shader");
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

void TestItemRenderer::initialize(QRhiCommandBuffer* commandBuffer)
{
    if (rhi() == rhi_)
        return;

    std::println("Backend: {}", rhi()->backendName());
    std::println("Device: {}", rhi()->driverInfo().deviceName.data());
    std::println();

    const uint32_t indices[24] = {
        // 8 triangles that make up 4 quads
        // clang-format off
            0, 1, 2, 2, 1, 3,
            4, 5, 6, 6, 5, 7,
            8, 9, 10, 10, 9, 11,
            12, 13, 14, 14, 13, 15,
        // clang-format on
    };

    const float positions[32] = {
        // 16 (x, y)
        // clang-format off
            -0.75, -0.75,  -0.25, -0.75,  -0.75, -0.25,  -0.25, -0.25,
            -0.75,  0.25,  -0.25,  0.25,  -0.75,  0.75,  -0.25,  0.75,
             0.25,  0.25,   0.75,  0.25,   0.25,  0.75,   0.75,  0.75,
             0.25, -0.75,   0.75, -0.75,   0.25, -0.25,   0.75, -0.25,
        // clang-format on
    };

    const float colors[16] = {
        // 4 (r, g, b, a)
        // clang-format off
            1, 0, 0, 1,
            1, 1, 0, 1,
            0, 1, 0, 1,
            0, 0, 1, 1,
        // clang-format on
    };

    // create buffers

    indexBuf_.reset(rhi()->newBuffer(QRhiBuffer::Static, QRhiBuffer::IndexBuffer, sizeof(indices)));
    if (!indexBuf_->create())
        qFatal("Failed to create the index buffer");

    positionBuf_.reset(rhi()->newBuffer(QRhiBuffer::Static, QRhiBuffer::VertexBuffer, sizeof(positions)));
    if (!positionBuf_->create())
        qFatal("Failed to create the position buffer");

    uniformBuf_.reset(rhi()->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 16 * sizeof(float)));
    if (!uniformBuf_->create())
        qFatal("Failed to create the uniform buffer");

    colorBuf_.reset(rhi()->newBuffer(QRhiBuffer::Static, QRhiBuffer::StorageBuffer, sizeof(colors)));
    if (!colorBuf_->create())
        qFatal("Failed to create the color buffer");

    // create shader resource bindings

    shaderResourceBindings_.reset(rhi()->newShaderResourceBindings());
    shaderResourceBindings_->setBindings({
        QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage, uniformBuf_.get()),
        QRhiShaderResourceBinding::bufferLoad(1, QRhiShaderResourceBinding::VertexStage, colorBuf_.get()),
    });
    if (!shaderResourceBindings_->create())
        qFatal("Failed to create the shader resource bindings");

    // load shaders

    const QShader vertexShader = loadShader_(":/App/Qsb/Simple.vert.qsb");
    const QShader fragmentShader = loadShader_(":/App/Qsb/Simple.frag.qsb");

    // create pipeline

    QRhiVertexInputLayout layout;
    layout.setAttributes({{0, 0, QRhiVertexInputAttribute::Float2, 0}});
    layout.setBindings({{2 * sizeof(float), QRhiVertexInputBinding::PerVertex}});

    pipeline_.reset(rhi()->newGraphicsPipeline());
    pipeline_->setVertexInputLayout(layout);
    pipeline_->setShaderStages({{QRhiShaderStage::Vertex, vertexShader}, {QRhiShaderStage::Fragment, fragmentShader}});
    pipeline_->setShaderResourceBindings(shaderResourceBindings_.get());
    pipeline_->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());
    pipeline_->setTopology(QRhiGraphicsPipeline::Triangles);
    if (!pipeline_->create())
        qFatal("Failed to create the pipeline");

    // resource updates

    QRhiResourceUpdateBatch* updates = rhi()->nextResourceUpdateBatch();

    updates->uploadStaticBuffer(indexBuf_.get(), 0, sizeof(indices), indices);
    updates->uploadStaticBuffer(positionBuf_.get(), 0, sizeof(positions), positions);
    updates->uploadStaticBuffer(colorBuf_.get(), 0, sizeof(colors), colors);

    const QMatrix4x4 mvpMatrix = rhi()->clipSpaceCorrMatrix();
    updates->updateDynamicBuffer(uniformBuf_.get(), 0, 16 * sizeof(float), mvpMatrix.data());

    commandBuffer->resourceUpdate(updates);

    rhi_ = rhi();
}

//----------------------------------------------------------------------------------------------------------------------

void TestItemRenderer::render(QRhiCommandBuffer* commandBuffer)
{
    const QSize s = renderTarget()->pixelSize();
    const float w = static_cast<float>(s.width());
    const float h = static_cast<float>(s.height());

    QRhiCommandBuffer::VertexInput inputs[]{{positionBuf_.get(), 0}};

    commandBuffer->beginPass(renderTarget(), Qt::white, {});
    commandBuffer->setGraphicsPipeline(pipeline_.get());
    commandBuffer->setVertexInput(0, 1, inputs, indexBuf_.get(), 0, QRhiCommandBuffer::IndexUInt32);
    commandBuffer->setShaderResources();
    commandBuffer->setViewport({0, 0, w, h});
    commandBuffer->drawIndexed(24);
    commandBuffer->endPass();
}
