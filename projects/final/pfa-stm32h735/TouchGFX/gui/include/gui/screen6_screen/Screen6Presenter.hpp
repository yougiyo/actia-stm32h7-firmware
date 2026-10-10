#ifndef SCREEN6PRESENTER_HPP
#define SCREEN6PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>
#include <vector>
#include <cstdint>

using namespace touchgfx;

class Screen6View;

class Screen6Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen6Presenter(Screen6View& v);

    virtual void activate();
    virtual void deactivate();
    void sendOBD2Request(const std::vector<uint8_t>& payload);

    virtual ~Screen6Presenter() {}

private:
    Screen6Presenter();

    Screen6View& view;
};

#endif // SCREEN6PRESENTER_HPP
