#ifndef SCREEN7VIEW_HPP
#define SCREEN7VIEW_HPP

#include <gui_generated/screen7_screen/Screen7ViewBase.hpp>
#include <gui/screen7_screen/Screen7Presenter.hpp>
#include <gui/common/CustomKeyboard.hpp>
#include <string.h>

/**
 * @brief Vue Screen7 : Écran d'envoi de données brutes et réception de réponse (Supervision)
 * 
 * L'utilisateur peut :
 *  1. Saisir une chaîne de données avec le clavier virtuel.
 *  2. Appuyer sur "Send Data" pour envoyer le paquet UDP sur le réseau Ethernet.
 *  3. Recevoir et visualiser la réponse asynchrone de l'ECU/serveur dans le champ "Response".
 *  4. Revenir manuellement vers Screen5 en appuyant sur le bouton "Back".
 */
class Screen7View : public Screen7ViewBase
{
public:
    Screen7View();
    virtual ~Screen7View() {}

    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void sendClicked();
    virtual void backClicked();
    virtual void dataClicked();

    void setResponse(const Unicode::UnicodeChar* response);

protected:
    CustomKeyboard keyboard;

    // Buffers texte pour la donnée émise et la réponse reçue
    Unicode::UnicodeChar bufferData[32];
    Unicode::UnicodeChar bufferResponse[32];

    // Callbacks TouchGFX pour les boutons
    touchgfx::Callback<Screen7View, const touchgfx::AbstractButtonContainer&> okClickedCallback;
    touchgfx::Callback<Screen7View, const touchgfx::AbstractButtonContainer&> sendClickedCallback;

    void okClickedHandler(const touchgfx::AbstractButtonContainer& src);
    void sendClickedHandler(const touchgfx::AbstractButtonContainer& src);
    void onUdpResponseReceived(const char* response);
};

#endif // SCREEN7VIEW_HPP