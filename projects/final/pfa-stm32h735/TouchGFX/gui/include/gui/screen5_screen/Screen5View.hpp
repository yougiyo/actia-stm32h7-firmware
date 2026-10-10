#ifndef SCREEN5VIEW_HPP
#define SCREEN5VIEW_HPP

#include <gui_generated/screen5_screen/Screen5ViewBase.hpp>
#include <gui/screen5_screen/Screen5Presenter.hpp>
#include <gui/common/CustomKeyboard.hpp>
#include <string.h>

class Screen5View : public Screen5ViewBase
{
public:
    Screen5View();
    virtual ~Screen5View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void sendDataFieldClicked();
    virtual void supervisionFieldClicked();
    virtual void ipv4Clicked();
    virtual void ipv6Clicked();
    virtual void sendRequestsClicked();
    virtual void sendDataClicked();

protected:
    enum ActiveField { NONE, FIELD_SEND_DATA, FIELD_SUPERVISION };
    enum ProtocolType { PROTOCOL_IPV4, PROTOCOL_IPV6 };

    ActiveField activeField;
    ProtocolType selectedProtocol;

    CustomKeyboard keyboard;

    Unicode::UnicodeChar bufferSendData[40];
    Unicode::UnicodeChar bufferSupervision[5];

    touchgfx::Callback<Screen5View, const touchgfx::AbstractButtonContainer&> okClickedCallback;
    void okClickedHandler(const touchgfx::AbstractButtonContainer& src);

    void showKeyboardForField(ActiveField field);
    void updateProtocolButtons();
    void updateInputHints();
    bool commitIpAddress();
    bool commitLogicalAddress();
    void notifyProtocolSelection();
};

#endif // SCREEN5VIEW_HPP
