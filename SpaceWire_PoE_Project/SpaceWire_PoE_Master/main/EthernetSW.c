#include "EthernetSW.h"

#define PHY_ADDR 1
#define PHY_RESET 5
#define PHY_MDIO 18
#define PHY_MDC 23
#define FRAME_BUFFER 1518

static const char *TAG = "ethernet";
esp_eth_handle_t eth_handle = NULL;
uint8_t* mac_addr_ptr;

// Event handler for Ethernet events
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    mac_addr_ptr = mac_addr;
    //we can get the ethernet driver handle from event data
    eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id)
    {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

void setupEthernet(void)
{
    //setup ethernet event handler callback
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));

    //Configure Ethernet Interface on ESP32
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG(); //MAC config object
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); //PHY config esp_eth_phy_new_ip101() can be used
    phy_config.phy_addr = PHY_ADDR;
    phy_config.reset_gpio_num = PHY_RESET;
    mac_config.smi_mdc_gpio_num = PHY_MDC;
    mac_config.smi_mdio_gpio_num = PHY_MDIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config); //create ethernet MAC instance against MAC config
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config); //create ethernet PHY instance for IP101 COnfig

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); //default eth config for mac and phy instances
    esp_eth_handle_t eth_handle = NULL;

    config.stack_input = (void *)receiveSWPackets;                 //pass ethernet frames received to rxSWpackets function to process
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle)); //after driver installed, obtain ethernet handler

    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
}
