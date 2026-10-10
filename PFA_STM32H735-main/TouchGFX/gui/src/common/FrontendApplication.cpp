#include <gui/common/FrontendApplication.hpp>
#include <gui/common/FrontendHeap.hpp>
#include <gui/screen6_screen/Screen6View.hpp>
#include <gui/screen6_screen/Screen6Presenter.hpp>
#include <gui/screen7_screen/Screen7View.hpp>
#include <gui/screen7_screen/Screen7Presenter.hpp>
#include <touchgfx/transitions/SlideTransition.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap), screen6TransitionCallback(), screen7TransitionCallback()
{

}

void FrontendApplication::gotoScreen6ScreenSlideTransitionWest()
{
    screen6TransitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoScreen6ScreenSlideTransitionWestImpl);
    pendingScreenTransitionCallback = &screen6TransitionCallback;
}

void FrontendApplication::gotoScreen6ScreenSlideTransitionWestImpl()
{
    touchgfx::makeTransition<Screen6View, Screen6Presenter, touchgfx::SlideTransition<touchgfx::WEST>, Model>(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

void FrontendApplication::gotoScreen7ScreenSlideTransitionWest()
{
    screen7TransitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoScreen7ScreenSlideTransitionWestImpl);
    pendingScreenTransitionCallback = &screen7TransitionCallback;
}

void FrontendApplication::gotoScreen7ScreenSlideTransitionWestImpl()
{
    touchgfx::makeTransition<Screen7View, Screen7Presenter, touchgfx::SlideTransition<touchgfx::WEST>, Model>(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
