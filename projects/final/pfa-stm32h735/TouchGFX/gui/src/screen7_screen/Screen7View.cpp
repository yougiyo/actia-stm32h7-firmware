#include <gui/screen7_screen/Screen7View.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Unicode.hpp>
#include "lwip.h"
#include <cstdio>

Screen7View::Screen7View() :
    okClickedCallback(this, &Screen7View::okClickedHandler),
    sendClickedCallback(this, &Screen7View::sendClickedHandler)
{
}

void Screen7View::setupScreen()
{
    Screen7ViewBase::setupScreen();

    // Réinitialisation des buffers texte
    memset(bufferData, 0, sizeof(bufferData));
    memset(bufferResponse, 0, sizeof(bufferResponse));

    textUserData.setWildcard(bufferData);
    textUserResponse.setWildcard(bufferResponse);

    // Positionnement du clavier virtuel sous l'écran (hors champ par défaut)
    keyboard.setPosition(80, 480, 320, 240);
    add(keyboard);

    flexButtonOK.setAction(okClickedCallback);
    flexButtonOK.setVisible(false);

    btnSend.setAction(sendClickedCallback);

    // Enregistrement du callback de réception UDP asynchrone depuis la couche LwIP
    supervision_udp_set_recv_callback([](const char* response, void* context) {
        Screen7View* self = static_cast<Screen7View*>(context);
        self->onUdpResponseReceived(response);
    }, this);
}

void Screen7View::tearDownScreen()
{
    Screen7ViewBase::tearDownScreen();
    // Désinscription du callback réseau lorsque l'écran est quitté
    supervision_udp_set_recv_callback(NULL, NULL);
}

void Screen7View::sendClicked()
{
    sendClickedHandler(btnSend);
}

/**
 * @brief Retour vers Screen5 lors de l'appui sur le bouton "Back"
 */
void Screen7View::backClicked()
{
    application().gotoScreen5ScreenCoverTransitionEast();
}

/**
 * @brief Clic sur le champ Donnée : ouverture du clavier virtuel
 */
void Screen7View::dataClicked()
{
    keyboard.clearBuffer();
    keyboard.moveTo(80, 32);
    flexButtonOK.setVisible(true);
    flexButtonOK.invalidate();
}

/**
 * @brief Mise à jour de la réponse affichée en Unicode
 */
void Screen7View::setResponse(const Unicode::UnicodeChar* response)
{
    Unicode::strncpy(bufferResponse, response, 31);
    bufferResponse[31] = 0;
    textUserResponse.resizeToCurrentText();
    textUserResponse.invalidate();
}

/**
 * @brief Callback appelé lors de la réception d'un paquet UDP de réponse
 */
void Screen7View::onUdpResponseReceived(const char* response)
{
    char responseUnicode[64];
    int len = snprintf(responseUnicode, sizeof(responseUnicode), "%s", response);
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(responseUnicode), bufferResponse, sizeof(bufferResponse) / sizeof(bufferResponse[0]));
    bufferResponse[len] = 0;
    textUserResponse.resizeToCurrentText();
    textUserResponse.invalidate();
}

/**
 * @brief Envoi de la donnée saisie sur le réseau Ethernet via UDP
 */
void Screen7View::sendClickedHandler(const touchgfx::AbstractButtonContainer& src)
{
    (void)src;
    char dataStr[32] = {0};
    Unicode::toUTF8(bufferData, reinterpret_cast<uint8_t*>(dataStr), sizeof(dataStr));
    if (dataStr[0] == '\0')
    {
        return;
    }
    supervision_udp_send_raw(dataStr);
}

/**
 * @brief Validation de la saisie clavier (bouton OK)
 */
void Screen7View::okClickedHandler(const touchgfx::AbstractButtonContainer& src)
{
    (void)src;
    Unicode::strncpy(bufferData, keyboard.getBuffer(), 31);
    bufferData[31] = 0;
    textUserData.resizeToCurrentText();
    textUserData.invalidate();
    keyboard.moveTo(80, 480);
    flexButtonOK.setVisible(false);
    flexButtonOK.invalidate();
}
