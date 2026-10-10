#ifndef SEND_REQUESTSVIEW_HPP
#define SEND_REQUESTSVIEW_HPP

#include <gui_generated/send_requests_screen/Send_RequestsViewBase.hpp>
#include <gui/send_requests_screen/Send_RequestsPresenter.hpp>
#include <gui/common/CustomKeyboard.hpp>
#include <string.h>

class Send_RequestsView : public Send_RequestsViewBase
{
public:
    Send_RequestsView();
    virtual ~Send_RequestsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void serviceClicked();
    virtual void dataClicked();

protected:
    enum ActiveField { NONE, FIELD_SERVICE, FIELD_DATA };
    ActiveField activeField;

    CustomKeyboard keyboard;

    Unicode::UnicodeChar bufferService[32];
    Unicode::UnicodeChar bufferData[32];

    Callback<Send_RequestsView, const AbstractButtonContainer&> okClickedCallback;

    void okClickedHandler(const AbstractButtonContainer& src);
    void showKeyboardForField(ActiveField field);
};

#endif // SEND_REQUESTSVIEW_HPP
