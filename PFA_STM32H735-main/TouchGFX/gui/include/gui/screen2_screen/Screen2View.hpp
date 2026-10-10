#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <gui/common/CustomKeyboard.hpp>
#include <string.h>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

protected:
    enum ActiveField { NONE, CIN, NOM, PRENOM };
    ActiveField activeField;

    CustomKeyboard keyboard;

    Unicode::UnicodeChar bufferCIN[20];
    Unicode::UnicodeChar bufferNom[20];
    Unicode::UnicodeChar bufferPrenom[20];

    Callback<Screen2View, const AbstractButtonContainer&> cinClickedCallback;
    Callback<Screen2View, const AbstractButtonContainer&> nomClickedCallback;
    Callback<Screen2View, const AbstractButtonContainer&> prenomClickedCallback;
    Callback<Screen2View, const AbstractButton&> validerClickedCallback;
    Callback<Screen2View, const AbstractButtonContainer&> okClickedCallback;

    void cinClickedHandler(const AbstractButtonContainer& src);
    void nomClickedHandler(const AbstractButtonContainer& src);
    void prenomClickedHandler(const AbstractButtonContainer& src);
    void validerClickedHandler(const AbstractButton& src);
    void okClickedHandler(const AbstractButtonContainer& src);

    void showKeyboardForField(ActiveField field);
};

#endif // SCREEN2VIEW_HPP
