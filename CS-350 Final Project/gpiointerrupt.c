/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */
/* standard libraries */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

//setting varibles
const float         MIN_DESIRED_TEMP              = 4.0f;
const float         MAX_DESIRED_TEMP              = 33.0;
const float         BUTTON_PRESS_VALUE            = 0.5f;
const uint32_t      TIMER_HZ = 10;
const uint_least8_t DOWN_BUTTON_GPIO_INDEX        = CONFIG_GPIO_BUTTON_0;
const uint_least8_t UP_BUTTON_GPIO_INDEX          = CONFIG_GPIO_BUTTON_1;
const uint_least8_t HEATER_GPIO_INDEX             = CONFIG_GPIO_LED_0;
const unsigned int  HEATER_GPIO_ON_VALUE          = CONFIG_GPIO_LED_ON;
const unsigned int  HEATER_GPIO_OFF_VALUE         = CONFIG_GPIO_LED_OFF;
const uint_least8_t TEMP_SENSOR_I2C_CONFIG_INDEX  = CONFIG_I2C_0;
const uint8_t       TEMP_SENSOR_I2C_SLAVE_ADDRESS = 0x0041;
const uint8_t       TEMP_SENSOR_I2C_RESULT_REG    = 0x0001;
const I2C_BitRate   TEMP_SENSOR_I2C_BIT_RATE      = I2C_400kHz;
const float         TEMP_SENSOR_RESOLUTION_C      = 0.0078125f;
const unsigned int  TEMP_SENSOR_SENSITIVITY       = 10;
I2C_Handle          tempSensorHandle;
I2C_Transaction     tempSensorTransaction;
uint8_t             tempSensorTxBuffer[1];
uint8_t             tempSensorRxBuffer[2];

//creating hearter stated, will need to turn led off and on based on this state.
enum HeaterState {
    HEATER_OFF,
    HEATER_ON
};
enum HeaterState    heaterState                   = HEATER_OFF;
float               desiredTemperature            = 20.0f;
float               ambientTemperature            = 0.0f;
unsigned char       preventRemoteSet              = 0;
char                serverInput[64];
char                serverOutput[64];
Timer_Handle        timer;
UART_Handle         uart;
volatile bool       timerFlag                     = false;
volatile bool       downButtonFlag                = false;
volatile bool       upButtonFlag                  = false;
volatile bool       serverInputFlag               = false;
volatile bool       temperatureFlag               = false;
volatile int16_t    temperatureReading            = false;


void timerCallback(Timer_Handle myHandle, int_fast16_t status) {
    timerFlag = true;
}

void downButtonCallback(uint_least8_t index) {
    downButtonFlag = true;
}

void upButtonCallback(uint_least8_t index) {
    upButtonFlag = true;
}

void serverCallback(UART_Handle handle, void *buf, size_t count) {
    serverInput[count - 1] = 0;
    serverInputFlag = true;
}

void temperatureCallback(I2C_Handle handle, I2C_Transaction *transaction, bool transferStatus) {
    if (transferStatus) {
        int16_t reading = (tempSensorRxBuffer[0] << 8) | (tempSensorRxBuffer[1]);
        int16_t diff = abs(temperatureReading - reading);

        if (diff > TEMP_SENSOR_SENSITIVITY) {
            temperatureReading = reading;
            temperatureFlag = true;
        }
    }
}

void requestTemperature() {
    I2C_transfer(tempSensorHandle, &tempSensorTransaction);
}

void initializeTemperatureSensor() {
    I2C_Params i2cParams;

    I2C_init();
    I2C_Params_init(&i2cParams);

    i2cParams.bitRate                  = TEMP_SENSOR_I2C_BIT_RATE;
    i2cParams.transferMode             = I2C_MODE_CALLBACK;
    i2cParams.transferCallbackFxn      = temperatureCallback;

    tempSensorTransaction.slaveAddress = TEMP_SENSOR_I2C_SLAVE_ADDRESS;
    tempSensorTransaction.writeBuf     = tempSensorTxBuffer;
    tempSensorTransaction.writeCount   = 1;
    tempSensorTransaction.readBuf      = tempSensorRxBuffer;
    tempSensorTransaction.readCount    = 2;

    tempSensorHandle = I2C_open(TEMP_SENSOR_I2C_CONFIG_INDEX, &i2cParams);

    if (tempSensorHandle == NULL) {
        while (1);
    }
    tempSensorTxBuffer[0] = TEMP_SENSOR_I2C_RESULT_REG;
}

void readFromServer() {
    UART_read(uart, &serverInput, 63);
}

void initializeServerConnection() {
    UART_Params uartParams;

    UART_init();
    UART_Params_init(&uartParams);

    uartParams.readMode      = UART_MODE_CALLBACK;
    uartParams.readEcho      = UART_ECHO_OFF;
    uartParams.readCallback  = serverCallback;
    uartParams.writeDataMode = UART_DATA_BINARY;

    uart = UART_open(CONFIG_UART_0, &uartParams);

    if (uart == NULL) {
        while (1);
    }

    readFromServer();
}

void initializeTimer() {
    Timer_Params params;

    Timer_init();
    Timer_Params_init(&params);

    params.periodUnits   = Timer_PERIOD_HZ;
    params.period        = TIMER_HZ;
    params.timerMode     = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer = Timer_open(CONFIG_TIMER_0, &params);

    if (timer == NULL || Timer_start(timer) == Timer_STATUS_ERROR) {
        while (1);
    }
}
//setting up buttom and led
void initializeHeaterAndButtons() {
    // initialize GPIO driver
    GPIO_init();

    // configure down button GPIO pin
    GPIO_setConfig(DOWN_BUTTON_GPIO_INDEX, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(DOWN_BUTTON_GPIO_INDEX, downButtonCallback);
    GPIO_enableInt(DOWN_BUTTON_GPIO_INDEX);

    // configure up button GPIO pin
    GPIO_setConfig(UP_BUTTON_GPIO_INDEX, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(UP_BUTTON_GPIO_INDEX, upButtonCallback);
    GPIO_enableInt(UP_BUTTON_GPIO_INDEX);

    // configure heater GPIO pin
    GPIO_setConfig(HEATER_GPIO_INDEX, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
}
//turing led on adn off based on temp read and temp set
bool setDesiredTemperature(float temperature) {
    if (temperature < MIN_DESIRED_TEMP) {
        temperature = MIN_DESIRED_TEMP;
    } else if (temperature > MAX_DESIRED_TEMP) {
        temperature = MAX_DESIRED_TEMP;
    } else {
        float reciprical = pow(BUTTON_PRESS_VALUE, -1);
        temperature = round(temperature * reciprical) / reciprical;
    }

    if (fabs(desiredTemperature - temperature) > TEMP_SENSOR_RESOLUTION_C * TEMP_SENSOR_SENSITIVITY) {
        desiredTemperature = temperature;
        return true;
    }

    return false;
}
//Uart and server set up
void sendStateToServer() {
    int len = snprintf(serverOutput, 64, "D:%f,A:%f,H:%d,L:%d\n", desiredTemperature, ambientTemperature, heaterState, preventRemoteSet);
    UART_write(uart, &serverOutput, len);
}

bool handleServerInput() {
    size_t len = strlen(serverInput);

    if (len > 0) {
        switch (serverInput[0]) {
            case 'U':
                return true;
            case 'D':
                if (len > 2 && serverInput[1] == ':' && !preventRemoteSet) {
                    float temperature;
                    if (sscanf(&serverInput[2], "%f", &temperature)) {
                        return setDesiredTemperature(temperature);
                    }
                }
        }
    }

    return false;
}

//state machine for heater state, its it is higher or lower than set temp
bool setHeaterState(enum HeaterState state) {
    if (heaterState != state) {
        switch (state) {
            case HEATER_ON:
                GPIO_write(HEATER_GPIO_INDEX, HEATER_GPIO_ON_VALUE);
                break;
            case HEATER_OFF:
                GPIO_write(HEATER_GPIO_INDEX, HEATER_GPIO_OFF_VALUE);
                break;
        }
        heaterState = state;
        return true;
    }

    return false;
}

bool decreaseDesiredTemperature() {
    return setDesiredTemperature(desiredTemperature - BUTTON_PRESS_VALUE);
}

bool increaseDesiredTemperature() {
    return setDesiredTemperature(desiredTemperature + BUTTON_PRESS_VALUE);
}

void *mainThread(void *arg0) {
    bool firstTempReceived = false;

    initializeServerConnection();
    initializeHeaterAndButtons();
    initializeTemperatureSensor();
    initializeTimer();
//while loop setup for buttons flags, pressing buttons and having heater turn off and on
    while(1) {
        while (!timerFlag);
        bool stateChanged = false;
        if (serverInputFlag) {
            stateChanged |= handleServerInput();
            serverInputFlag = false;
            readFromServer();
        }
        if (downButtonFlag && upButtonFlag) {
            preventRemoteSet = !preventRemoteSet;
            downButtonFlag = upButtonFlag = false;
            stateChanged = true;
        } else if (downButtonFlag) {
            stateChanged |= decreaseDesiredTemperature();
            downButtonFlag = false;

        } else if (upButtonFlag) {
            stateChanged |= increaseDesiredTemperature();
            upButtonFlag = false;
        }
        if (temperatureFlag) {
            ambientTemperature = temperatureReading * TEMP_SENSOR_RESOLUTION_C;
            temperatureFlag = false;
            firstTempReceived = true;
            stateChanged = true;
        }
        if (ambientTemperature < desiredTemperature) {
            stateChanged |= setHeaterState(HEATER_ON);
        } else if (ambientTemperature > desiredTemperature) {
            stateChanged |= setHeaterState(HEATER_OFF);
        }
        if (firstTempReceived && stateChanged) {
            sendStateToServer();
        }
        requestTemperature();
        timerFlag = false;
    }
}
