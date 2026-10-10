#include <gui/send_requests_screen/Send_RequestsView.hpp>

Send_RequestsView::Send_RequestsView() :
    activeField(NONE),
    okClickedCallback(this, &Send_RequestsView::okClickedHandler)
{

}

void Send_RequestsView::setupScreen()
{
    Send_RequestsViewBase::setupScreen();

    memset(bufferService, 0, sizeof(bufferService));
    memset(bufferData, 0, sizeof(bufferData));

    textUserService.setWildcard(bufferService);
    textUserData.setWildcard(bufferData);
    textUserService.invalidate();
    textUserData.invalidate();

    keyboard.setPosition(80, 480, 320, 240);
    add(keyboard);

    flexButtonOK.setAction(okClickedCallback);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();
}

void Send_RequestsView::tearDownScreen()
{
    Send_RequestsViewBase::tearDownScreen();
}

void Send_RequestsView::serviceClicked()
{
    showKeyboardForField(FIELD_SERVICE);
}

void Send_RequestsView::dataClicked()
{
    showKeyboardForField(FIELD_DATA);
}

void Send_RequestsView::showKeyboardForField(ActiveField field)
{
    activeField = field;
    keyboard.clearBuffer();
    keyboard.moveTo(80, 32);

    flexButtonOK.setVisible(true);
    flexButtonOK.invalidate();
}

void Send_RequestsView::okClickedHandler(const AbstractButtonContainer& src)
{
    Unicode::UnicodeChar* typedText = keyboard.getBuffer();

    switch (activeField)
    {
    case FIELD_SERVICE:
        Unicode::strncpy(bufferService, typedText, 31);
        bufferService[31] = 0;
        textUserService.resizeToCurrentText();
        textUserService.invalidate();
        break;
    case FIELD_DATA:
        Unicode::strncpy(bufferData, typedText, 31);
        bufferData[31] = 0;
        textUserData.resizeToCurrentText();
        textUserData.invalidate();
        break;
    default:
        break;
    }

    keyboard.moveTo(80, 480);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();
    activeField = NONE;
}
