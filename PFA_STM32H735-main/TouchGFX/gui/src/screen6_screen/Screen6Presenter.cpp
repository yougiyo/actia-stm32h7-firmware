#include <gui/screen6_screen/Screen6View.hpp>
#include <gui/screen6_screen/Screen6Presenter.hpp>
#include <vector>

Screen6Presenter::Screen6Presenter(Screen6View& v)
    : view(v)
{
}

void Screen6Presenter::activate()
{
}

void Screen6Presenter::deactivate()
{
}

void Screen6Presenter::sendOBD2Request(const std::vector<uint8_t>& payload)
{
    model->sendOBD2Request(payload);
}