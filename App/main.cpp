#include "Framework/Application.hpp"

int main(int argc, char* argv[])
{
    cf::framework::Application app(argc, argv);
    return app.run();
}