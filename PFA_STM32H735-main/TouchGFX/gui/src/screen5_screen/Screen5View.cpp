#include <gui/screen5_screen/Screen5View.hpp>
#include <touchgfx/Color.hpp>

extern "C" int supervision_udp_configure(const char* destination, const char* logicalAddress, unsigned char useIpv6);
extern "C" int supervision_udp_send_protocol_notification(unsigned char useIpv6);
extern "C" int supervision_udp_send_logical(void);

namespace
{
bool isDigit(touchgfx::Unicode::UnicodeChar value) { return value >= '0' && value <= '9'; }
}

Screen5View::Screen5View() :
    activeField(NONE),
    selectedProtocol(PROTOCOL_IPV4),
    okClickedCallback(this, &Screen5View::okClickedHandler)
{

}

void Screen5View::setupScreen()
{
    Screen5ViewBase::setupScreen();

    memset(bufferSendData, 0, sizeof(bufferSendData));
    memset(bufferSupervision, 0, sizeof(bufferSupervision));

    textUserIP.setWildcard(bufferSendData);
    textUserIP_1.setWildcard(bufferSupervision);
    textUserIP.invalidate();
    textUserIP_1.invalidate();

    keyboard.setPosition(80, 480, 320, 240);
    add(keyboard);

    flexButtonOK.setAction(okClickedCallback);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();

    updateProtocolButtons();
    updateInputHints();
}

void Screen5View::tearDownScreen()
{
    Screen5ViewBase::tearDownScreen();
}

void Screen5View::sendDataFieldClicked()
{
    showKeyboardForField(FIELD_SEND_DATA);
}

void Screen5View::supervisionFieldClicked()
{
    showKeyboardForField(FIELD_SUPERVISION);
}

void Screen5View::ipv4Clicked()
{
    selectedProtocol = PROTOCOL_IPV4;
    updateProtocolButtons();
    updateInputHints();
    notifyProtocolSelection();
}

void Screen5View::ipv6Clicked()
{
    selectedProtocol = PROTOCOL_IPV6;
    updateProtocolButtons();
    updateInputHints();
    notifyProtocolSelection();
}

void Screen5View::sendRequestsClicked()
{
    application().gotoScreen6ScreenSlideTransitionWest();
}

void Screen5View::sendDataClicked()
{
    char ipAddress[40] = { 0 };
    char logicalAddress[5] = { 0 };
    Unicode::toUTF8(bufferSendData, reinterpret_cast<uint8_t*>(ipAddress), sizeof(ipAddress));
    Unicode::toUTF8(bufferSupervision, reinterpret_cast<uint8_t*>(logicalAddress), sizeof(logicalAddress));
    if (supervision_udp_configure(ipAddress, logicalAddress, selectedProtocol == PROTOCOL_IPV6) == 0)
    {
        (void)supervision_udp_send_logical();
    }
    application().gotoScreen7ScreenSlideTransitionWest();
}

void Screen5View::showKeyboardForField(ActiveField field)
{
    activeField = field;
    keyboard.clearBuffer();
    if (field == FIELD_SEND_DATA)
        keyboard.setInputMode(selectedProtocol == PROTOCOL_IPV4 ? CustomKeyboard::INPUT_IPV4 : CustomKeyboard::INPUT_IPV6);
    else
        keyboard.setInputMode(CustomKeyboard::INPUT_NUMERIC);
    keyboard.moveTo(80, 32);

    flexButtonOK.setVisible(true);
    flexButtonOK.invalidate();
}

void Screen5View::okClickedHandler(const touchgfx::AbstractButtonContainer& src)
{
    switch (activeField)
    {
    case FIELD_SEND_DATA:
        commitIpAddress();
        break;
    case FIELD_SUPERVISION:
        commitLogicalAddress();
        break;
    default:
        break;
    }

    keyboard.moveTo(80, 480);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();
    activeField = NONE;
}

void Screen5View::updateInputHints()
{
    const char* hint = selectedProtocol == PROTOCOL_IPV4 ? "___.___.___.___" : "____:____:____:____:____:____:____:____";
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(hint), bufferSendData, sizeof(bufferSendData) / sizeof(bufferSendData[0]));
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>("____"), bufferSupervision, sizeof(bufferSupervision) / sizeof(bufferSupervision[0]));
    textUserIP.invalidate();
    textUserIP_1.invalidate();
}

bool Screen5View::commitIpAddress()
{
    Unicode::UnicodeChar* raw = keyboard.getBuffer();
    if (selectedProtocol == PROTOCOL_IPV4)
    {
        uint16_t digits = 0;
        Unicode::UnicodeChar digitOnly[12];
        for (uint16_t i = 0; raw[i] != 0; i++)
        {
            if (isDigit(raw[i]))
            {
                if (digits == 12) return false;
                digitOnly[digits++] = raw[i];
            }
            else if (raw[i] != '.') return false;
        }
        if (digits != 12) return false;
        uint16_t source = 0, target = 0;
        for (uint16_t group = 0; group < 4; group++)
        {
            uint16_t value = 0;
            for (uint16_t digit = 0; digit < 3; digit++) { value = value * 10 + (digitOnly[source++] - '0'); }
            if (value > 255) return false;
            bufferSendData[target++] = digitOnly[source - 3]; bufferSendData[target++] = digitOnly[source - 2]; bufferSendData[target++] = digitOnly[source - 1];
            if (group != 3) bufferSendData[target++] = '.';
        }
        bufferSendData[target] = 0;
    }
    else
    {
        Unicode::strncpy(bufferSendData, raw, 39);
        bufferSendData[39] = 0;
    }
    textUserIP.resizeToCurrentText();
    textUserIP.invalidate();
    return true;
}

bool Screen5View::commitLogicalAddress()
{
    Unicode::UnicodeChar* raw = keyboard.getBuffer();
    uint16_t length = Unicode::strlen(raw);
    if (length == 0 || length > 4) return false;
    for (uint16_t i = 0; i < length; i++) if (!isDigit(raw[i])) return false;
    uint16_t padding = 4 - length;
    for (uint16_t i = 0; i < padding; i++) bufferSupervision[i] = '0';
    Unicode::strncpy(&bufferSupervision[padding], raw, length);
    bufferSupervision[4] = 0;
    textUserIP_1.resizeToCurrentText();
    textUserIP_1.invalidate();
    return true;
}

void Screen5View::notifyProtocolSelection()
{
    char ipAddress[40] = { 0 };
    char logicalAddress[5] = { 0 };
    Unicode::toUTF8(bufferSendData, reinterpret_cast<uint8_t*>(ipAddress), sizeof(ipAddress));
    Unicode::toUTF8(bufferSupervision, reinterpret_cast<uint8_t*>(logicalAddress), sizeof(logicalAddress));
    if (supervision_udp_configure(ipAddress, logicalAddress, selectedProtocol == PROTOCOL_IPV6) == 0)
        supervision_udp_send_protocol_notification(selectedProtocol == PROTOCOL_IPV6);
}

void Screen5View::updateProtocolButtons()
{
    const touchgfx::colortype greenBg = touchgfx::Color::getColorFromRGB(46, 139, 68);
    const touchgfx::colortype greenBgPressed = touchgfx::Color::getColorFromRGB(35, 110, 55);
    const touchgfx::colortype whiteBg = touchgfx::Color::getColorFromRGB(255, 255, 255);
    const touchgfx::colortype whiteBgPressed = touchgfx::Color::getColorFromRGB(240, 240, 240);
    const touchgfx::colortype greenBorder = touchgfx::Color::getColorFromRGB(46, 139, 68);
    const touchgfx::colortype grayBorder = touchgfx::Color::getColorFromRGB(180, 180, 180);
    const touchgfx::colortype whiteText = touchgfx::Color::getColorFromRGB(255, 255, 255);
    const touchgfx::colortype grayText = touchgfx::Color::getColorFromRGB(150, 150, 150);

    if (selectedProtocol == PROTOCOL_IPV4)
    {
        flexButtonIPv4.setBoxWithBorderColors(greenBg, greenBgPressed, greenBorder, greenBgPressed);
        flexButtonIPv4.setTextColors(whiteText, whiteText);
        flexButtonIPv6.setBoxWithBorderColors(whiteBg, whiteBgPressed, grayBorder, grayBorder);
        flexButtonIPv6.setTextColors(grayText, grayText);
    }
    else
    {
        flexButtonIPv6.setBoxWithBorderColors(greenBg, greenBgPressed, greenBorder, greenBgPressed);
        flexButtonIPv6.setTextColors(whiteText, whiteText);
        flexButtonIPv4.setBoxWithBorderColors(whiteBg, whiteBgPressed, grayBorder, grayBorder);
        flexButtonIPv4.setTextColors(grayText, grayText);
    }

    flexButtonIPv4.invalidate();
    flexButtonIPv6.invalidate();
}
