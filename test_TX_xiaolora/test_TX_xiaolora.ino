#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
SoftwareSerial e5(9, 10); 
int data1=0;
int data2=0;
int data3=0; 
int data4=0; 

static char recv_buf[512];
static bool is_exist = false;

static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    e5.printf(p_cmd, args);
    Serial.printf(p_cmd, args);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
    {
        return 0;
    }
 
    do
    {
        while (e5.available() > 0)
        {
            ch = e5.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }
 
        if (strstr(recv_buf, p_ack) != NULL)
        {
            return 1;
        }
    } while (millis() - startMillis < timeout_ms);
    return 0;
}

static int node_send(uint32_t timeout)
{
    static uint16_t count = 0;
    int ret = 0;
    char data[32];
    char cmd[128];
    
    memset(data, 0, sizeof(data));
    sprintf(data, "%04X,%04X,%04X,%04X", data1, data2, data3,data4);
    sprintf(cmd, "AT+TEST=TXLRPKT,\"5345454544%s\"\r\n", data);
    ret = at_send_check_response("TX DONE", 2000, cmd);
    if (ret == 1)
    {
        Serial.print("Sent successfully!\r\n");
    }
    else
    {
        Serial.print("Send failed!\r\n");
    }
    data1++;
    data2=data2+5;
    data3=data3+10;
    data4=data4+20;
    return ret;
}
 
void setup(void)
{
    Serial.begin(115200);
    // while (!Serial);
    e5.begin(9600);
    uint16_t error;
    char errorMessage[256];
    Serial.print("ping pong communication!\r\n");
    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);
    }
    else
    {
        is_exist = false;
        Serial.print("No E5 module found.\r\n");
    }
}
 
void loop(void)
{
    if (is_exist)
    {
        node_send(2000);
        delay(3000);    
    }
}
