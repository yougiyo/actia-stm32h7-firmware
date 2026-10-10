#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <cstdint>

class ModelListener;

/**
 * @brief Classe Model : Cœur de l'architecture MVP TouchGFX
 * 
 * Cette classe fait l'interface entre l'interface graphique (Presenters/Views)
 * et la couche matérielle/réseau (Pile TCP/IP LwIP, FreeRTOS, Ethernet).
 */
class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    // Méthodes d'émission de requêtes OBD2 / UDS vers Ethernet
    void sendOBD2Request(const std::vector<uint8_t>& payload);
    void sendOBD2Request(const char* payloadStr);

protected:
    ModelListener* modelListener;
};

#endif // MODEL_HPP
