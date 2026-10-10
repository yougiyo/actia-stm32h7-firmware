#include <gui/supervision_screen/SupervisionView.hpp>
#include <touchgfx/Color.hpp>
#include "lwip.h"

SupervisionView::SupervisionView() :
    activeField(NONE),
    selectedProtocol(PROTOCOL_IPV4),
    okClickedCallback(this, &SupervisionView::okClickedHandler)
{

}

void SupervisionView::setupScreen()
{
    SupervisionViewBase::setupScreen();

    memset(bufferIP, 0, sizeof(bufferIP));
    memset(bufferLogical, 0, sizeof(bufferLogical));

    textUserIP.setWildcard(bufferIP);
    textUserLogical.setWildcard(bufferLogical);
    textUserIP.invalidate();
    textUserLogical.invalidate();

    keyboard.setPosition(80, 480, 320, 240);
    add(keyboard);

    flexButtonOK.setAction(okClickedCallback);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();

    updateProtocolButtons();
}

void SupervisionView::tearDownScreen()
{
    SupervisionViewBase::tearDownScreen();
}

void SupervisionView::ipClicked()
{
    showKeyboardForField(FIELD_IP);
}

void SupervisionView::logicalClicked()
{
    showKeyboardForField(FIELD_LOGICAL);
}

void SupervisionView::ipv4Clicked()
{
    selectedProtocol = PROTOCOL_IPV4;
    updateProtocolButtons();
    supervision_udp_send_protocol_notification(0);
}

void SupervisionView::ipv6Clicked()
{
    selectedProtocol = PROTOCOL_IPV6;
    updateProtocolButtons();
    supervision_udp_send_protocol_notification(1);
}

void SupervisionView::sendRequestsClicked()
{
    supervision_udp_send_raw("0001");
}

void SupervisionView::sendDataClicked()
{
    supervision_udp_send_raw("0002");
}

void SupervisionView::showKeyboardForField(ActiveField field)
{
    activeField = field;
    keyboard.clearBuffer();
    keyboard.moveTo(80, 32);

    flexButtonOK.setVisible(true);
    flexButtonOK.invalidate();
}

void SupervisionView::okClickedHandler(const touchgfx::AbstractButtonContainer& src)
{
    Unicode::UnicodeChar* typedText = keyboard.getBuffer();

    switch (activeField)
    {
    case FIELD_IP:
        Unicode::strncpy(bufferIP, typedText, 31);
        bufferIP[31] = 0;
        textUserIP.resizeToCurrentText();
        textUserIP.invalidate();

        {
            char ipStr[40] = {0};
            char logicalStr[5] = {0};
            Unicode::strncpy((Unicode::UnicodeChar*)ipStr, bufferIP, 39);
            Unicode::strncpy((Unicode::UnicodeChar*)logicalStr, bufferLogical, 4);
            supervision_udp_configure(ipStr, logicalStr, selectedProtocol == PROTOCOL_IPV6 ? 1 : 0);
        }
        break;
    case FIELD_LOGICAL:
        Unicode::strncpy(bufferLogical, typedText, 31);
        bufferLogical[31] = 0;
        textUserLogical.resizeToCurrentText();
        textUserLogical.invalidate();

        {
            char ipStr[40] = {0};
            char logicalStr[5] = {0};
            Unicode::strncpy((Unicode::UnicodeChar*)ipStr, bufferIP, 39);
            Unicode::strncpy((Unicode::UnicodeChar*)logicalStr, bufferLogical, 4);
            supervision_udp_configure(ipStr, logicalStr, selectedProtocol == PROTOCOL_IPV6 ? 1 : 0);
        }
        break;
    default:
        break;
    }

    keyboard.moveTo(80, 480);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();
    activeField = NONE;
}

void SupervisionView::updateProtocolButtons()
{
    radioButtonIPv4.setSelected(selectedProtocol == PROTOCOL_IPV4);
    radioButtonIPv6.setSelected(selectedProtocol == PROTOCOL_IPV6);
    radioButtonIPv4.invalidate();
    radioButtonIPv6.invalidate();
}
