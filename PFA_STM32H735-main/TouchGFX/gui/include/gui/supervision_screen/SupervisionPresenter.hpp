#ifndef SUPERVISIONPRESENTER_HPP
#define SUPERVISIONPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SupervisionView;

class SupervisionPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SupervisionPresenter(SupervisionView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~SupervisionPresenter() {}

private:
    SupervisionPresenter();

    SupervisionView& view;
};

#endif // SUPERVISIONPRESENTER_HPP
