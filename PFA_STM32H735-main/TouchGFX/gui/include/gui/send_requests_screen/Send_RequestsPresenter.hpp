#ifndef SEND_REQUESTSPRESENTER_HPP
#define SEND_REQUESTSPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Send_RequestsView;

class Send_RequestsPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Send_RequestsPresenter(Send_RequestsView& v);

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

    virtual ~Send_RequestsPresenter() {}

private:
    Send_RequestsPresenter();

    Send_RequestsView& view;
};

#endif // SEND_REQUESTSPRESENTER_HPP
