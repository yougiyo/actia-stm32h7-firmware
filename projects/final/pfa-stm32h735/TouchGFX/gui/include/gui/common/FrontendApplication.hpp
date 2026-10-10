#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent()
    {
        model.tick();
        FrontendApplicationBase::handleTickEvent();
    }
    void gotoScreen6ScreenSlideTransitionWest();
    void gotoScreen7ScreenSlideTransitionWest();
private:
    touchgfx::Callback<FrontendApplication> screen6TransitionCallback;
    touchgfx::Callback<FrontendApplication> screen7TransitionCallback;
    void gotoScreen6ScreenSlideTransitionWestImpl();
    void gotoScreen7ScreenSlideTransitionWestImpl();
};

#endif // FRONTENDAPPLICATION_HPP
