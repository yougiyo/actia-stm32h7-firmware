#include <gui/screen6_screen/Screen6View.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Unicode.hpp>
#include "lwip.h"
#include <cstring>
#include <cstdio>
#include <algorithm>

// --- Définition des Services UDS disponibles ---
const Screen6View::ServiceOption Screen6View::serviceList[NUM_SERVICES] = {
    {"0x10 - Session Control", 0x10, 0x01},
    {"0x22 - Read Data By ID", 0x22, 0x00},
    {"0x19 - Read DTC Info", 0x19, 0x02},
    {"0x31 - Routine Control", 0x31, 0x01},
    {"0x27 - Security Access", 0x27, 0x01},
    {"0x11 - ECU Reset", 0x11, 0x01},
    {"0x2E - Write Data By ID", 0x2E, 0x00}
};

// --- Définition des Données (DIDs / PIDs) disponibles ---
const Screen6View::DataOption Screen6View::dataList[NUM_DATA] = {
    {"0xF190 - VIN", 0xF1, 0x90},
    {"0xF187 - SW Version", 0xF1, 0x87},
    {"0xF18C - Serial Number", 0xF1, 0x8C},
    {"0x0100 - Supported PIDs", 0x01, 0x00},
    {"0x010C - Engine RPM", 0x01, 0x0C},
    {"0x010D - Vehicle Speed", 0x01, 0x0D},
    {"0x0105 - Coolant Temp", 0x01, 0x05},
    {"0x010F - Intake Air Temp", 0x01, 0x0F},
    {"0x0111 - Throttle Position", 0x01, 0x11},
    {"0x0200 - Freeze Frame", 0x02, 0x00}
};

/**
 * @brief Constructeur de la ligne de menu déroulant (DropdownItemWidget)
 */
Screen6View::DropdownItemWidget::DropdownItemWidget() :
    itemClickCallback(this, &DropdownItemWidget::buttonClickHandler),
    callbackAction(0),
    itemIndex(0)
{
    // Bouton de fond interactif avec retour visuel (blanc au repos, vert clair à l'appui)
    itemBtn.setBoxWithBorderPosition(0, 0, DROPDOWN_WIDTH - 2, ITEM_HEIGHT);
    itemBtn.setBorderSize(1);
    itemBtn.setBoxWithBorderColors(
        touchgfx::Color::getColorFromRGB(255, 255, 255),
        touchgfx::Color::getColorFromRGB(220, 245, 225),
        touchgfx::Color::getColorFromRGB(220, 225, 230),
        touchgfx::Color::getColorFromRGB(0, 137, 70)
    );
    itemBtn.setPosition(0, 0, DROPDOWN_WIDTH - 2, ITEM_HEIGHT);
    itemBtn.setAction(itemClickCallback);
    add(itemBtn);

    // Texte affiché dans la ligne
    itemText.setPosition(6, 4, DROPDOWN_WIDTH - 14, ITEM_HEIGHT - 6);
    itemText.setColor(touchgfx::Color::getColorFromRGB(30, 30, 30));
    itemText.setLinespacing(0);
    itemText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_VAL_SERVICE));
    itemText.setWildcard(textBuffer);
    add(itemText);

    setWidth(DROPDOWN_WIDTH - 2);
    setHeight(ITEM_HEIGHT);
}

/**
 * @brief Configuration du texte et de l'index pour chaque item de la liste
 */
void Screen6View::DropdownItemWidget::setup(const char* label, uint8_t index, touchgfx::GenericCallback<uint8_t>& action)
{
    itemIndex = index;
    callbackAction = &action;
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(label), textBuffer, sizeof(textBuffer) / sizeof(textBuffer[0]));
    itemText.invalidate();
}

/**
 * @brief Handler de clic sur un item de la liste : notifie la vue avec l'index sélectionné
 */
void Screen6View::DropdownItemWidget::buttonClickHandler(const touchgfx::AbstractButtonContainer& src)
{
    (void)src;
    if (callbackAction && callbackAction->isValid())
    {
        callbackAction->execute(itemIndex);
    }
}

/**
 * @brief Constructeur de la vue Screen6
 */
Screen6View::Screen6View() :
    selectedServiceIdx(0),
    selectedDataIdx(0),
    backdropDismissCallback(this, &Screen6View::backdropDismissHandler),
    successTimer(0),
    serviceSelectCallback(this, &Screen6View::onServiceSelected),
    dataSelectCallback(this, &Screen6View::onDataSelected),
    sendBtnCallback(this, &Screen6View::sendBtnHandler)
{
}

/**
 * @brief Initialisation de l'écran Screen6 et de tous ses composants
 */
void Screen6View::setupScreen()
{
    Screen6ViewBase::setupScreen();

    // Réinitialisation des buffers texte
    memset(bufferService, 0, sizeof(bufferService));
    memset(bufferData, 0, sizeof(bufferData));
    memset(bufferSentService, 0, sizeof(bufferSentService));
    memset(bufferSentData, 0, sizeof(bufferSentData));
    memset(bufferSentTo, 0, sizeof(bufferSentTo));
    memset(bufferSentFrame, 0, sizeof(bufferSentFrame));

    textUserService.setWildcard(bufferService);
    textUserData.setWildcard(bufferData);

    // Initialise les champs avec les premières valeurs par défaut (0x10 et 0xF190)
    updateServiceFieldDisplay();
    updateDataFieldDisplay();

    // Associe l'action du bouton "Send Data"
    btnSend.setAction(sendBtnCallback);

    // 1. Bouton d'arrière-plan transparent : capte les clics en dehors pour fermer les listes
    backdropDismissBtn.setBoxWithBorderPosition(0, 0, 480, 272);
    backdropDismissBtn.setBorderSize(0);
    backdropDismissBtn.setAlpha(0);
    backdropDismissBtn.setAction(backdropDismissCallback);
    backdropDismissBtn.setPosition(0, 0, 480, 272);
    backdropDismissBtn.setVisible(false);
    add(backdropDismissBtn);

    // 2. Configuration du menu déroulant Service (7 items)
    serviceDropdownContainer.setPosition(164, 110, DROPDOWN_WIDTH, 145);
    serviceDropdownContainer.setVisible(false);

    serviceDropdownBorder.setPosition(0, 0, DROPDOWN_WIDTH, 145);
    serviceDropdownBorder.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    serviceDropdownBorder.setBorderColor(touchgfx::Color::getColorFromRGB(0, 137, 70));
    serviceDropdownBorder.setBorderSize(2);
    serviceDropdownContainer.add(serviceDropdownBorder);

    serviceScrollable.setPosition(1, 1, DROPDOWN_WIDTH - 2, 143);
    serviceScrollable.setScrollbarsColor(touchgfx::Color::getColorFromRGB(0, 137, 70));
    for (uint8_t i = 0; i < NUM_SERVICES; i++)
    {
        serviceItemWidgets[i].setPosition(0, i * ITEM_HEIGHT, DROPDOWN_WIDTH - 2, ITEM_HEIGHT);
        serviceItemWidgets[i].setup(serviceList[i].label, i, serviceSelectCallback);
        serviceScrollable.add(serviceItemWidgets[i]);
    }
    serviceDropdownContainer.add(serviceScrollable);
    add(serviceDropdownContainer);

    // 3. Configuration du menu déroulant Data (10 items défilables)
    dataDropdownContainer.setPosition(164, 90, DROPDOWN_WIDTH, 160);
    dataDropdownContainer.setVisible(false);

    dataDropdownBorder.setPosition(0, 0, DROPDOWN_WIDTH, 160);
    dataDropdownBorder.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    dataDropdownBorder.setBorderColor(touchgfx::Color::getColorFromRGB(0, 137, 70));
    dataDropdownBorder.setBorderSize(2);
    dataDropdownContainer.add(dataDropdownBorder);

    dataScrollable.setPosition(1, 1, DROPDOWN_WIDTH - 2, 158);
    dataScrollable.setScrollbarsColor(touchgfx::Color::getColorFromRGB(0, 137, 70));
    for (uint8_t i = 0; i < NUM_DATA; i++)
    {
        dataItemWidgets[i].setPosition(0, i * ITEM_HEIGHT, DROPDOWN_WIDTH - 2, ITEM_HEIGHT);
        dataItemWidgets[i].setup(dataList[i].label, i, dataSelectCallback);
        dataScrollable.add(dataItemWidgets[i]);
    }
    dataDropdownContainer.add(dataScrollable);
    add(dataDropdownContainer);

    // 4. Configuration de la boîte de dialogue de confirmation (Popup de succès)
    successPopupContainer.setPosition(50, 40, 380, 175);
    successPopupContainer.setVisible(false);

    successPopupBox.setPosition(0, 0, 380, 175);
    successPopupBox.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    successPopupBox.setBorderColor(touchgfx::Color::getColorFromRGB(0, 137, 70));
    successPopupBox.setBorderSize(3);
    successPopupContainer.add(successPopupBox);

    // Titre vert
    successTitleText.setPosition(10, 10, 360, 24);
    successTitleText.setColor(touchgfx::Color::getColorFromRGB(0, 137, 70));
    successTitleText.setLinespacing(0);
    successTitleText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_DATA_SENT));
    successPopupContainer.add(successTitleText);

    // Ligne 1 : Service envoyé
    successServiceText.setPosition(15, 40, 350, 22);
    successServiceText.setColor(touchgfx::Color::getColorFromRGB(30, 30, 30));
    successServiceText.setLinespacing(0);
    successServiceText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_VAL_SERVICE));
    successServiceText.setWildcard(bufferSentService);
    successPopupContainer.add(successServiceText);

    // Ligne 2 : Data envoyée
    successDataText.setPosition(15, 66, 350, 22);
    successDataText.setColor(touchgfx::Color::getColorFromRGB(30, 30, 30));
    successDataText.setLinespacing(0);
    successDataText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_VAL_SERVICE));
    successDataText.setWildcard(bufferSentData);
    successPopupContainer.add(successDataText);

    // Ligne 3 : Adresse IP et port de destination
    successToText.setPosition(15, 92, 350, 22);
    successToText.setColor(touchgfx::Color::getColorFromRGB(30, 30, 30));
    successToText.setLinespacing(0);
    successToText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_VAL_SERVICE));
    successToText.setWildcard(bufferSentTo);
    successPopupContainer.add(successToText);

    // Ligne 4 : Trame hexadécimale brute envoyée
    successFrameText.setPosition(15, 118, 350, 22);
    successFrameText.setColor(touchgfx::Color::getColorFromRGB(0, 110, 56));
    successFrameText.setLinespacing(0);
    successFrameText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_VAL_SERVICE));
    successFrameText.setWildcard(bufferSentFrame);
    successPopupContainer.add(successFrameText);

    add(successPopupContainer);
}

void Screen6View::tearDownScreen()
{
    Screen6ViewBase::tearDownScreen();
}

/**
 * @brief Gestionnaire de tick exécuté à chaque trame graphique (60 Hz)
 * Compte à rebours de 5 secondes (300 ticks) après l'envoi avant de retourner à Screen5
 */
void Screen6View::handleTickEvent()
{
    if (successTimer > 0)
    {
        successTimer--;
        if (successTimer == 0)
        {
            successPopupContainer.setVisible(false);
            successPopupContainer.invalidate();
            // Retour automatique vers l'écran Screen5
            application().gotoScreen5ScreenCoverTransitionEast();
        }
    }
}

/**
 * @brief Clic sur le champ Service : ouvre/ferme le menu déroulant des Services
 */
void Screen6View::serviceClicked()
{
    if (serviceDropdownContainer.isVisible())
    {
        closeDropdowns();
    }
    else
    {
        openServiceDropdown();
    }
}

/**
 * @brief Clic sur le champ Data : ouvre/ferme le menu déroulant des Données
 */
void Screen6View::dataClicked()
{
    if (dataDropdownContainer.isVisible())
    {
        closeDropdowns();
    }
    else
    {
        openDataDropdown();
    }
}

/**
 * @brief Ouvre le menu déroulant des services et affiche le fond de fermeture
 */
void Screen6View::openServiceDropdown()
{
    dataDropdownContainer.setVisible(false);
    dataDropdownContainer.invalidate();

    backdropDismissBtn.setVisible(true);
    backdropDismissBtn.invalidate();

    serviceDropdownContainer.setVisible(true);
    serviceDropdownContainer.invalidate();
}

/**
 * @brief Ouvre le menu déroulant des données et affiche le fond de fermeture
 */
void Screen6View::openDataDropdown()
{
    serviceDropdownContainer.setVisible(false);
    serviceDropdownContainer.invalidate();

    backdropDismissBtn.setVisible(true);
    backdropDismissBtn.invalidate();

    dataDropdownContainer.setVisible(true);
    dataDropdownContainer.invalidate();
}

/**
 * @brief Ferme tous les menus déroulants et masque le fond de capture
 */
void Screen6View::closeDropdowns()
{
    bool needsInvalidate = false;
    if (backdropDismissBtn.isVisible())
    {
        backdropDismissBtn.setVisible(false);
        backdropDismissBtn.invalidate();
        needsInvalidate = true;
    }
    if (serviceDropdownContainer.isVisible())
    {
        serviceDropdownContainer.setVisible(false);
        serviceDropdownContainer.invalidate();
        needsInvalidate = true;
    }
    if (dataDropdownContainer.isVisible())
    {
        dataDropdownContainer.setVisible(false);
        dataDropdownContainer.invalidate();
        needsInvalidate = true;
    }
    if (needsInvalidate)
    {
        invalidate();
    }
}

/**
 * @brief Clic en dehors des menus : fermeture automatique
 */
void Screen6View::backdropDismissHandler(const touchgfx::AbstractButtonContainer& src)
{
    (void)src;
    closeDropdowns();
}

/**
 * @brief Sélection d'un service depuis le menu déroulant
 */
void Screen6View::onServiceSelected(uint8_t index)
{
    if (index < NUM_SERVICES)
    {
        selectedServiceIdx = index;
        updateServiceFieldDisplay();
    }
    closeDropdowns();
}

/**
 * @brief Sélection d'une donnée depuis le menu déroulant
 */
void Screen6View::onDataSelected(uint8_t index)
{
    if (index < NUM_DATA)
    {
        selectedDataIdx = index;
        updateDataFieldDisplay();
    }
    closeDropdowns();
}

/**
 * @brief Met à jour le texte affiché dans le rectangle du Service
 */
void Screen6View::updateServiceFieldDisplay()
{
    if (selectedServiceIdx >= 0 && selectedServiceIdx < NUM_SERVICES)
    {
        Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(serviceList[selectedServiceIdx].label),
                          bufferService,
                          sizeof(bufferService) / sizeof(bufferService[0]));
    }
    textUserService.resizeToCurrentText();
    textUserService.invalidate();
}

/**
 * @brief Met à jour le texte affiché dans le rectangle de la Donnée
 */
void Screen6View::updateDataFieldDisplay()
{
    if (selectedDataIdx >= 0 && selectedDataIdx < NUM_DATA)
    {
        Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(dataList[selectedDataIdx].label),
                          bufferData,
                          sizeof(bufferData) / sizeof(bufferData[0]));
    }
    textUserData.resizeToCurrentText();
    textUserData.invalidate();
}

void Screen6View::sendBtnHandler(const touchgfx::AbstractButtonContainer& src)
{
    (void)src;
    sendClicked();
}

/**
 * @brief Envoi de la trame via Ethernet et affichage de la boîte de dialogue de confirmation
 */
void Screen6View::sendClicked()
{
    std::vector<uint8_t> request = buildRequestFromSelection();
    if (!request.empty())
    {
        // 1. Transmission de la requête au Model (vers la couche réseau LwIP UDP)
        presenter->sendOBD2Request(request);

        // 2. Formatage de la trame brute en texte hexadécimal pour l'affichage (ex: "Trame: 10 01 11")
        char frameStr[40] = { 0 };
        int offset = snprintf(frameStr, sizeof(frameStr), "Trame: ");
        for (size_t i = 0; i < request.size() && offset < static_cast<int>(sizeof(frameStr)) - 4; ++i)
        {
            offset += snprintf(frameStr + offset, sizeof(frameStr) - offset, (i == 0 ? "%02X" : " %02X"), request[i]);
        }

        // 3. Déclenchement du popup de succès (5 secondes)
        showSuccessPopup(frameStr);
    }
}

/**
 * @brief Remplissage et affichage de la boîte de dialogue de confirmation d'envoi
 */
void Screen6View::showSuccessPopup(const char* frameHex)
{
    closeDropdowns();

    // 1. Ligne du Service
    char serviceStr[50] = { 0 };
    if (selectedServiceIdx >= 0 && selectedServiceIdx < NUM_SERVICES)
    {
        snprintf(serviceStr, sizeof(serviceStr), "Service: %s", serviceList[selectedServiceIdx].label);
    }
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(serviceStr), bufferSentService, sizeof(bufferSentService) / sizeof(bufferSentService[0]));
    successServiceText.resizeToCurrentText();

    // 2. Ligne de la Donnée
    char dataStr[50] = { 0 };
    if (selectedDataIdx >= 0 && selectedDataIdx < NUM_DATA)
    {
        snprintf(dataStr, sizeof(dataStr), "Data: %s", dataList[selectedDataIdx].label);
    }
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(dataStr), bufferSentData, sizeof(bufferSentData) / sizeof(bufferSentData[0]));
    successDataText.resizeToCurrentText();

    // 3. Ligne de l'adresse IP de destination
    char toStr[50] = { 0 };
    const char* destIp = supervision_udp_get_destination_ip();
    snprintf(toStr, sizeof(toStr), "Dest IP: %s (UDP 5000)", destIp);
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(toStr), bufferSentTo, sizeof(bufferSentTo) / sizeof(bufferSentTo[0]));
    successToText.resizeToCurrentText();

    // 4. Ligne de la trame hexadécimale
    Unicode::fromUTF8(reinterpret_cast<const uint8_t*>(frameHex), bufferSentFrame, sizeof(bufferSentFrame) / sizeof(bufferSentFrame[0]));
    successFrameText.resizeToCurrentText();

    // Rendre la popup visible et démarrer le timer de 5 secondes (300 frames à 60 fps)
    successPopupContainer.setVisible(true);
    successPopupContainer.invalidate();
    successTimer = 300;
}

/**
 * @brief Retour vers Screen5 lors du clic sur le bouton "Back"
 */
void Screen6View::backClicked()
{
    application().gotoScreen5ScreenCoverTransitionEast();
}

/**
 * @brief Construction de la trame d'octets UDS à partir du service et de la donnée choisis
 */
std::vector<uint8_t> Screen6View::buildRequestFromSelection()
{
    std::vector<uint8_t> request;

    if (selectedServiceIdx < 0 || selectedServiceIdx >= NUM_SERVICES)
    {
        return request;
    }

    // Octet de service (SID)
    uint8_t sid = serviceList[selectedServiceIdx].sid;
    request.push_back(sid);

    // Octets de la donnée sélectionnée (MSB et LSB)
    if (selectedDataIdx >= 0 && selectedDataIdx < NUM_DATA)
    {
        uint8_t msb = dataList[selectedDataIdx].msb;
        uint8_t lsb = dataList[selectedDataIdx].lsb;

        request.push_back(msb);
        request.push_back(lsb);
    }

    return request;
}