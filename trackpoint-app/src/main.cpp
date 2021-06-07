// Include dependencies
#include <QApplication>
#include <QSurfaceFormat>
#include <QMessageBox>

#ifdef INCLUDE_PLUGINS
#include <QtPlugin>
Q_IMPORT_PLUGIN (QXcbIntegrationPlugin);
#endif

// Include modules
#include "MainWindow.hpp"
#include "OpenScadRenderer.hpp"

int main(int argc, char** argv) {
    QApplication application(argc, argv);

    if (!OpenScadRenderer::openScadAvailable()) {
      QMessageBox openScadErrorBox;
      openScadErrorBox.setText("Unable to find OpenSCAD! Either it is not installed or not in its default location. Exporting will be disabled.");
      openScadErrorBox.exec();
    }

    QSurfaceFormat format;
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);

    QSurfaceFormat::setDefaultFormat(format);

    MainWindow mainWindow;
    mainWindow.show();

    return(application.exec());
}
