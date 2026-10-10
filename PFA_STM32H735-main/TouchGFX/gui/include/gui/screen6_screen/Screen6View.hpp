#ifndef SCREEN6VIEW_HPP
#define SCREEN6VIEW_HPP

#include <gui_generated/screen6_screen/Screen6ViewBase.hpp>
#include <gui/screen6_screen/Screen6Presenter.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/containers/ScrollableContainer.hpp>
#include <touchgfx/containers/buttons/Buttons.hpp>
#include <touchgfx/widgets/BoxWithBorder.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/Callback.hpp>
#include <vector>
#include <cstdint>

/**
 * @brief Vue Screen6 : Écran d'envoi des requêtes UDS/OBD2 via Ethernet (Send_Data)
 * 
 * Cet écran propose à l'utilisateur deux listes déroulantes (ComboBox personnalisées) :
 *  1. Sélection du Service UDS (7 options disponibles)
 *  2. Sélection de la Donnée / DID / PID (10 options disponibles)
 *
 * Lorsque l'utilisateur appuie sur "Send Data" :
 *  - La trame UDS est construite et envoyée en UDP sur le réseau Ethernet (port 5000).
 *  - Une boîte de dialogue de confirmation s'affiche pendant 5 secondes (avec les détails du service, data, IP et trame).
 *  - L'application revient ensuite automatiquement à Screen5.
 */
class Screen6View : public Screen6ViewBase
{
public:
    Screen6View();
    virtual ~Screen6View() {}

    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    // Handlers d'actions hérités de Screen6ViewBase
    virtual void serviceClicked();
    virtual void dataClicked();
    virtual void sendClicked();
    virtual void backClicked();

protected:
    static const uint8_t NUM_SERVICES = 7;
    static const uint8_t NUM_DATA = 10;
    static const uint16_t DROPDOWN_WIDTH = 255;
    static const uint16_t ITEM_HEIGHT = 26;

    // Structure représentant une option de service UDS
    struct ServiceOption
    {
        const char* label;      // Libellé affiché à l'écran (ex: "0x10 - Session Control")
        uint8_t sid;            // Identifiant de service (ex: 0x10)
        uint8_t subFunction;    // Sous-fonction par défaut (ex: 0x01)
    };

    // Structure représentant une option de donnée (DID / PID)
    struct DataOption
    {
        const char* label;      // Libellé affiché à l'écran (ex: "0xF190 - VIN")
        uint8_t msb;            // Octet de poids fort (ex: 0xF1)
        uint8_t lsb;            // Octet de poids faible (ex: 0x90)
    };

    /**
     * @brief Widget personnalisé représentant une ligne cliquable dans la liste déroulante
     */
    class DropdownItemWidget : public touchgfx::Container
    {
    public:
        DropdownItemWidget();
        void setup(const char* label, uint8_t index, touchgfx::GenericCallback<uint8_t>& action);

    private:
        touchgfx::BoxWithBorderButtonStyle<touchgfx::ClickButtonTrigger> itemBtn;
        touchgfx::TextAreaWithOneWildcard itemText;
        touchgfx::Unicode::UnicodeChar textBuffer[40];
        touchgfx::Callback<DropdownItemWidget, const touchgfx::AbstractButtonContainer&> itemClickCallback;
        touchgfx::GenericCallback<uint8_t>* callbackAction;
        uint8_t itemIndex;

        void buttonClickHandler(const touchgfx::AbstractButtonContainer& src);
    };

    // Index des éléments actuellement sélectionnés
    int8_t selectedServiceIdx;
    int8_t selectedDataIdx;

    // Buffers texte pour les champs de saisie et la popup
    Unicode::UnicodeChar bufferService[40];
    Unicode::UnicodeChar bufferData[40];
    Unicode::UnicodeChar bufferSentService[50];
    Unicode::UnicodeChar bufferSentData[50];
    Unicode::UnicodeChar bufferSentTo[50];
    Unicode::UnicodeChar bufferSentFrame[50];

    // Listes statiques des options prédéfinies
    static const ServiceOption serviceList[NUM_SERVICES];
    static const DataOption dataList[NUM_DATA];

    // Bouton transparent plein écran pour fermer les listes déroulantes au clic en dehors
    touchgfx::BoxWithBorderButtonStyle<touchgfx::ClickButtonTrigger> backdropDismissBtn;
    touchgfx::Callback<Screen6View, const touchgfx::AbstractButtonContainer&> backdropDismissCallback;

    // Menu déroulant pour les Services
    touchgfx::Container serviceDropdownContainer;
    touchgfx::BoxWithBorder serviceDropdownBorder;
    touchgfx::ScrollableContainer serviceScrollable;
    DropdownItemWidget serviceItemWidgets[NUM_SERVICES];

    // Menu déroulant pour les Données
    touchgfx::Container dataDropdownContainer;
    touchgfx::BoxWithBorder dataDropdownBorder;
    touchgfx::ScrollableContainer dataScrollable;
    DropdownItemWidget dataItemWidgets[NUM_DATA];

    // Boîte de dialogue de succès (5 secondes)
    touchgfx::Container successPopupContainer;
    touchgfx::BoxWithBorder successPopupBox;
    touchgfx::TextArea successTitleText;
    touchgfx::TextAreaWithOneWildcard successServiceText;
    touchgfx::TextAreaWithOneWildcard successDataText;
    touchgfx::TextAreaWithOneWildcard successToText;
    touchgfx::TextAreaWithOneWildcard successFrameText;
    uint16_t successTimer;

    // Callbacks TouchGFX
    touchgfx::Callback<Screen6View, uint8_t> serviceSelectCallback;
    touchgfx::Callback<Screen6View, uint8_t> dataSelectCallback;
    touchgfx::Callback<Screen6View, const touchgfx::AbstractButtonContainer&> sendBtnCallback;

    // Fonctions internes de gestion
    void onServiceSelected(uint8_t index);
    void onDataSelected(uint8_t index);
    void backdropDismissHandler(const touchgfx::AbstractButtonContainer& src);
    void sendBtnHandler(const touchgfx::AbstractButtonContainer& src);

    void openServiceDropdown();
    void openDataDropdown();
    void closeDropdowns();
    void showSuccessPopup(const char* frameHex);

    void updateServiceFieldDisplay();
    void updateDataFieldDisplay();

    std::vector<uint8_t> buildRequestFromSelection();
};

#endif // SCREEN6VIEW_HPP