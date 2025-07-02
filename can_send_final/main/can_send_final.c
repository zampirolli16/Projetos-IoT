#include <stdio.h>
#include "driver/twai.h"  // Include the TWAI (CAN) driver library
#include "esp_log.h"      // Include ESP logging library
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "as5600.h"
#include "esp_timer.h"
#include "math.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "dht11.h"

#define TAG "CAN_DRIVER"  // Define a tag for logging messages

void app_main() {
    // Configure the CAN driver parameters (TX = 4; RX = 5)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_4, GPIO_NUM_5, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    int tempo = 0;

    // Initialize the CAN driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG, "TWAI driver installed successfully");
    } else {
        ESP_LOGE(TAG, "Failed to install TWAI driver");
        return; // Exit the function if driver installation fails
    }

    // Start the CAN driver
    if (twai_start() == ESP_OK) {
        ESP_LOGI(TAG, "TWAI driver started successfully");
    } else {
        ESP_LOGE(TAG, "Failed to start TWAI driver");
        return; // Exit the function if driver start fails
    }

    // Setup Anemometro
    as5600_init();
    DHT11_init(GPIO_NUM_26);
    float angulo_anterior = as5600_get_angle_degrees();
    int64_t tempo_anterior = esp_timer_get_time();
    int flag = 0;
    int T_media = 75;
    int count_media = 0;
    float vet_media[T_media];
    float rpm_media = 0;

    while (1){
        //lógica anemometro
        flag = 0;
        float angulo_atual = as5600_get_angle_degrees();
        int64_t tempo_atual = esp_timer_get_time();

        
        float delta_angulo = angulo_atual - angulo_anterior;
        if (delta_angulo < -0.1f) {
            delta_angulo += 360.0; // Correção de overflow (ex: 359 → 5 graus)
            flag=1;
        }

        float delta_tempo = (tempo_atual - tempo_anterior) / 1000000.0; // micros → segundos
        float rpm = (delta_angulo / 360.0f) * (60.0f / delta_tempo);
        if (fabs(delta_angulo) < 1.0f) {
        rpm = 0.0f;
        }

        //Filtro média móvel
        if (count_media < T_media){
            vet_media[count_media] = rpm;
            int i = 0;
            rpm_media = 0;
            while (i < T_media){
                rpm_media = rpm_media + vet_media[i];
                i++;
            }
            rpm_media = rpm_media/T_media;
            count_media++;
        }
        else{
            count_media = 0;
        }
        //printf("RPM: %.2f\n", rpm);
        //printf("angulo anterior: %.2f\n",angulo_anterior);
        //printf("angulo atual: %.2f\n",angulo_atual);

        
        
        //if (flag == 1){
       //     printf("negativo\n");
       // }

        angulo_anterior = angulo_atual;
        tempo_anterior = tempo_atual;

        //dht11 (temeratura e humidade)

        vTaskDelay (pdMS_TO_TICKS(1));
        tempo++;

        // Prepare a message to send
        if (tempo == 200){
            int rpm_send = (int)round(rpm_media);
            twai_message_t anemometro;          
            anemometro.identifier = 0x001;     
            anemometro.data_length_code = 1;       
            anemometro.data[0] = rpm_send;  

            twai_message_t temperatura;
            temperatura.identifier = 0x002;
            temperatura.data_length_code = 2;
            temperatura.data[0] = DHT11_read().temperature;

            twai_message_t humidade;
            humidade.identifier = 0x003;
            humidade.data_length_code = 1;
            humidade.data[0] = DHT11_read().humidity;
            
            printf("Anemometro \n");
            printf ("RPM Raw: %.2f \n", rpm);
            printf ("RPM (média móvel): %.2f\n", rpm_media);
            printf ("rpm mensagem: %d\n", anemometro.data[0]);
            printf("angulo anterior: %.2f\n",angulo_anterior);
            printf("angulo atual: %.2f\n \n",angulo_atual);

            printf ("Temperatura: %d \n \n", DHT11_read().temperature);

            printf ("humidade: %d \n \n", DHT11_read().humidity);


            // Transmit the message
            if (twai_transmit(&anemometro, pdMS_TO_TICKS(1)) == ESP_OK) {
                ESP_LOGI(TAG, "Anemometro transmitted");
            } else {
                ESP_LOGE(TAG, "Failed to transmit Anemometro");
            }

            if (twai_transmit(&temperatura, pdMS_TO_TICKS(1)) == ESP_OK) {
                ESP_LOGI(TAG, "temperatura transmitted");
            } else {
                ESP_LOGE(TAG, "Failed to transmit temperatura");
            }

            if (twai_transmit(&humidade, pdMS_TO_TICKS(1)) == ESP_OK) {
                ESP_LOGI(TAG, "humidade transmitted");
            } else {
                ESP_LOGE(TAG, "Failed to transmit humidade");
            }


            tempo = 0;
        }
        
    }

    // Stop and uninstall the CAN driver
    twai_stop();                         
    twai_driver_uninstall();             
    ESP_LOGI(TAG, "TWAI driver uninstalled");
}

