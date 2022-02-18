
#include "Context.hpp"
#include "Rasterizer.hpp"

int main(void)
{
    Context context;
    Rasterizer rasterizer(context);

    bool running = true;
    while (running)
    {
        running = rasterizer.update();
    }

    return 0;
}