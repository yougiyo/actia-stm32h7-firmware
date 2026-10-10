#ifndef SEND_DATAVIEW_HPP
#define SEND_DATAVIEW_HPP

#include <gui_generated/send_data_screen/Send_DataViewBase.hpp>
#include <gui/send_data_screen/Send_DataPresenter.hpp>
#include <gui/common/CustomKeyboard.hpp>
#include <string.h>

class Send_DataView : public Send_DataViewBase
{
public:
    Send_DataView();
    virtual ~Send_DataView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void field1Clicked();
    virtual void field2Clicked();

protected:
    enum ActiveField { NONE, FIELD_1, FIELD_2 };
    ActiveField activeField;

    CustomKeyboard keyboard;

    Unicode::UnicodeChar buffer1[32];
    Unicode::UnicodeChar buffer2[32];

    Callback<Send_DataView, const AbstractButtonContainer&> okClickedCallback;

    void okClickedHandler(const AbstractButtonContainer& src);
    void showKeyboardForField(ActiveField field);
};

#endif // SEND_DATAVIEW_HPP
