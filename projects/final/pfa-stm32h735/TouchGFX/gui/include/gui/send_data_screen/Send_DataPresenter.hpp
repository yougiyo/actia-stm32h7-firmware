#ifndef SEND_DATAPRESENTER_HPP
#define SEND_DATAPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Send_DataView;

class Send_DataPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Send_DataPresenter(Send_DataView& v);

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

    virtual ~Send_DataPresenter() {}

private:
    Send_DataPresenter();

    Send_DataView& view;
};

#endif // SEND_DATAPRESENTER_HPP
