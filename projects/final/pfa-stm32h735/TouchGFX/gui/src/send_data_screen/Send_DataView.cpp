#include <gui/send_data_screen/Send_DataView.hpp>

Send_DataView::Send_DataView() :
    activeField(NONE),
    okClickedCallback(this, &Send_DataView::okClickedHandler)
{

}

void Send_DataView::setupScreen()
{
    Send_DataViewBase::setupScreen();

    memset(buffer1, 0, sizeof(buffer1));
    memset(buffer2, 0, sizeof(buffer2));

    textUser1.setWildcard(buffer1);
    textUser2.setWildcard(buffer2);
    textUser1.invalidate();
    textUser2.invalidate();

    keyboard.setPosition(80, 480, 320, 240);
    add(keyboard);

    flexButtonOK.setAction(okClickedCallback);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();
}

void Send_DataView::tearDownScreen()
{
    Send_DataViewBase::tearDownScreen();
}

void Send_DataView::field1Clicked()
{
    showKeyboardForField(FIELD_1);
}

void Send_DataView::field2Clicked()
{
    showKeyboardForField(FIELD_2);
}

void Send_DataView::showKeyboardForField(ActiveField field)
{
    activeField = field;
    keyboard.clearBuffer();
    keyboard.moveTo(80, 32);

    flexButtonOK.setVisible(true);
    flexButtonOK.invalidate();
}

void Send_DataView::okClickedHandler(const AbstractButtonContainer& src)
{
    Unicode::UnicodeChar* typedText = keyboard.getBuffer();

    switch (activeField)
    {
    case FIELD_1:
        Unicode::strncpy(buffer1, typedText, 31);
        buffer1[31] = 0;
        textUser1.resizeToCurrentText();
        textUser1.invalidate();
        break;
    case FIELD_2:
        Unicode::strncpy(buffer2, typedText, 31);
        buffer2[31] = 0;
        textUser2.resizeToCurrentText();
        textUser2.invalidate();
        break;
    default:
        break;
    }

    keyboard.moveTo(80, 480);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();
    activeField = NONE;
}
