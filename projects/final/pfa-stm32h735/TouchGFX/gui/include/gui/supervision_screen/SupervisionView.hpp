#ifndef SUPERVISIONVIEW_HPP
#define SUPERVISIONVIEW_HPP

#include <gui_generated/supervision_screen/SupervisionViewBase.hpp>
#include <gui/supervision_screen/SupervisionPresenter.hpp>
#include <gui/common/CustomKeyboard.hpp>
#include <string.h>

class SupervisionView : public SupervisionViewBase
{
public:
    SupervisionView();
    virtual ~SupervisionView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void ipClicked();
    virtual void logicalClicked();
    virtual void ipv4Clicked();
    virtual void ipv6Clicked();
    virtual void sendRequestsClicked();
    virtual void sendDataClicked();

protected:
    enum ActiveField { NONE, FIELD_IP, FIELD_LOGICAL };
    ActiveField activeField;

    enum Protocol { PROTOCOL_IPV4, PROTOCOL_IPV6 };
    Protocol selectedProtocol;

    CustomKeyboard keyboard;

    Unicode::UnicodeChar bufferIP[32];
    Unicode::UnicodeChar bufferLogical[32];

    touchgfx::Callback<SupervisionView, const touchgfx::AbstractButtonContainer&> okClickedCallback;
    void okClickedHandler(const touchgfx::AbstractButtonContainer& src);

    void showKeyboardForField(ActiveField field);
    void updateProtocolButtons();
};

#endif // SUPERVISIONVIEW_HPP
