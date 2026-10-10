#include <gui/screen2_screen/Screen2View.hpp>
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include <string.h>
#include <stdio.h>

/* Adresse et port d'ecoute du PC - a adapter si ta configuration change */
#define PC_IP_ADDRESS   "192.168.1.5"
#define PC_LISTEN_PORT  5000

Screen2View::Screen2View() :
    activeField(NONE),
    cinClickedCallback(this, &Screen2View::cinClickedHandler),
    nomClickedCallback(this, &Screen2View::nomClickedHandler),
    prenomClickedCallback(this, &Screen2View::prenomClickedHandler),
    validerClickedCallback(this, &Screen2View::validerClickedHandler),
    okClickedCallback(this, &Screen2View::okClickedHandler)
{
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    memset(bufferCIN, 0, sizeof(bufferCIN));
    memset(bufferNom, 0, sizeof(bufferNom));
    memset(bufferPrenom, 0, sizeof(bufferPrenom));

    textUser1.setWildcard(bufferCIN);
    textUser2.setWildcard(bufferNom);
    textUser3.setWildcard(bufferPrenom);
    textUser1.invalidate();
    textUser2.invalidate();
    textUser3.invalidate();

    keyboard.setPosition(80, 480, 320, 240); // Y=480 = hors écran au démarrage
    add(keyboard);

    flexButton1.setAction(cinClickedCallback);
    flexButton1_1.setAction(nomClickedCallback);
    flexButton1_2.setAction(prenomClickedCallback);
    buttonWithLabel1.setAction(validerClickedCallback);

    flexButton2.setAction(okClickedCallback);
    flexButton2.setVisible(false); // bouton OK caché au démarrage
    flexButton2.invalidate();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::cinClickedHandler(const AbstractButtonContainer& src)
{
    showKeyboardForField(CIN);
}

void Screen2View::nomClickedHandler(const AbstractButtonContainer& src)
{
    showKeyboardForField(NOM);
}

void Screen2View::prenomClickedHandler(const AbstractButtonContainer& src)
{
    showKeyboardForField(PRENOM);
}

void Screen2View::showKeyboardForField(ActiveField field)
{
    activeField = field;
    keyboard.clearBuffer();
    keyboard.moveTo(80, 32);

    flexButton2.setVisible(true);
    flexButton2.invalidate();
}

void Screen2View::validerClickedHandler(const AbstractButton& src)
{
    /* Si un champ etait encore en cours de saisie au clavier, enregistre-le avant l'envoi */
    if (activeField != NONE)
    {
        Unicode::UnicodeChar* typedText = keyboard.getBuffer();
        switch (activeField)
        {
        case CIN:
            Unicode::strncpy(bufferCIN, typedText, 19);
            bufferCIN[19] = 0;
            textUser1.resizeToCurrentText();
            textUser1.invalidate();
            break;
        case NOM:
            Unicode::strncpy(bufferNom, typedText, 19);
            bufferNom[19] = 0;
            textUser2.resizeToCurrentText();
            textUser2.invalidate();
            break;
        case PRENOM:
            Unicode::strncpy(bufferPrenom, typedText, 19);
            bufferPrenom[19] = 0;
            textUser3.resizeToCurrentText();
            textUser3.invalidate();
            break;
        default:
            break;
        }
        keyboard.moveTo(80, 480);
        flexButton2.setVisible(false);
        flexButton2.invalidate();
        activeField = NONE;
    }

    /* Convertit les 3 buffers Unicode (UTF-16) en chaines ASCII simples. */
    char cinStr[20]    = {0};
    char nomStr[20]    = {0};
    char prenomStr[20] = {0};

    for (int i = 0; i < 19 && bufferCIN[i] != 0; i++)    { cinStr[i]    = (char)bufferCIN[i]; }
    for (int i = 0; i < 19 && bufferNom[i] != 0; i++)    { nomStr[i]    = (char)bufferNom[i]; }
    for (int i = 0; i < 19 && bufferPrenom[i] != 0; i++) { prenomStr[i] = (char)bufferPrenom[i]; }

    char payload[128];
    int len = snprintf(payload, sizeof(payload), "CIN:%s;NOM:%s;PRENOM:%s", cinStr, nomStr, prenomStr);

    int sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (sock >= 0)
    {
        struct sockaddr_in destAddr;
        memset(&destAddr, 0, sizeof(destAddr));
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(PC_LISTEN_PORT);
        destAddr.sin_addr.s_addr = inet_addr(PC_IP_ADDRESS);

        lwip_sendto(sock, payload, len, 0, (struct sockaddr*)&destAddr, sizeof(destAddr));
        lwip_close(sock);
    }

    /* Transition vers Screen3 */
    application().gotoScreen3ScreenSlideTransitionWest();
}

void Screen2View::okClickedHandler(const AbstractButtonContainer& src)
{
    Unicode::UnicodeChar* typedText = keyboard.getBuffer(); // <-- vraie saisie du clavier

    switch (activeField)
    {
    case CIN:
        Unicode::strncpy(bufferCIN, typedText, 19);
        bufferCIN[19] = 0;
        textUser1.resizeToCurrentText();
        textUser1.invalidate();
        break;
    case NOM:
        Unicode::strncpy(bufferNom, typedText, 19);
        bufferNom[19] = 0;
        textUser2.resizeToCurrentText();
        textUser2.invalidate();
        break;
    case PRENOM:
        Unicode::strncpy(bufferPrenom, typedText, 19);
        bufferPrenom[19] = 0;
        textUser3.resizeToCurrentText();
        textUser3.invalidate();
        break;
    default:
        break;
    }

    keyboard.moveTo(80, 480);
    flexButton2.setVisible(false);
    flexButton2.invalidate();
    activeField = NONE;
}