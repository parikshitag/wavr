#include "application.h"

Application::Application(const QString &id, int &argc, char **argv)
    : QtSingleApplication(id, argc, argv) {
}

Application::~Application(void) {
}
