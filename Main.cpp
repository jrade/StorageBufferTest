#include "Pch.h"

#include "TestItem.h"

int main(int argc, char** argv)
{
    std::println("Qt {}", ::qVersion());

    // QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
    // QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D11);
    // QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D12);

    QGuiApplication app{argc, argv};
    ::qmlRegisterType<TestItem>("App", 1, 0, "TestItem");

    QQmlApplicationEngine eng{QUrl{"qrc:/App/Main.qml"}};
    if (eng.rootObjects().isEmpty())
        qFatal("Failed to load the QML script");

    return app.exec();
}
