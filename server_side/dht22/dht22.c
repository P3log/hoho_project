#include <stdio.h>
#include <pigpio.h>
#include <string.h>

#define DHT_PIN 4
#define MAX_EDGES 200

typedef struct {
    uint32_t tick[MAX_EDGES];
    uint8_t level[MAX_EDGES];
    int count;
} capture_t;


/* ---------------- RAW CALLBACK ---------------- */
void callback(int gpio, int level, uint32_t tick, void *userdata) {
    (void)gpio; // to avoid a warning during compilation because unused
    capture_t *c = (capture_t *)userdata;

    if (c->count < MAX_EDGES) {
        c->tick[c->count] = tick;
        c->level[c->count] = level;
        c->count++;

    }
}


/* ---------------- START DHT22 ---------------- */
void dht_start() {
    gpioSetMode(DHT_PIN, PI_OUTPUT);
    gpioSetPullUpDown(DHT_PIN, PI_PUD_UP);

    gpioWrite(DHT_PIN, 0);
    gpioDelay(20000);   // >= 18ms
    gpioWrite(DHT_PIN, 1);
    gpioDelay(40);

    gpioSetMode(DHT_PIN, PI_INPUT);
}


/* ---------------- DECODE ---------------- */
int decode_bits(capture_t *c, int *bits) {
    int b = 0;
    int started = 0;

    for (int i = 1; i < c->count && b < 40; i++) {
        uint32_t dt = c->tick[i] - c->tick[i - 1];

        /* detect the beginning of DATA (end of ACK) */
        if (!started) {
            // end of ACK = HIGH ~80µs follwoed by LOW ~50µs
            if (c->level[i - 1] == 1 &&
                dt > 60 && dt < 120) {
                started = 1;
            }
            continue;
        }

        /* ---------------- DATA ONLY ---------------- */
        if (c->level[i - 1] == 1) {
            if (dt > 10 && dt < 200) {
                // DHT22 threshold
                bits[b] = (dt > 60);
                b++;
            }
        }
    }
    return b;
}


/* ---------------- MAIN ---------------- */
int getMeasures(float* humidity, float* temperature) {
    if (gpioInitialise() < 0) {
        printf("pigpio init failed\n");
        return 1;
    }

    capture_t cap = {0};
    int bits[40];

    /* arm callback BEFORE start */
    gpioSetAlertFuncEx(DHT_PIN, callback, &cap);

    dht_start();

    /* capture window (~10ms enough for DHT22) */
    gpioDelay(10000);

    gpioSetAlertFunc(DHT_PIN, NULL);

    /* ---------------- DEBUG ---------------- */
    // printf("edges captured: %d\n", cap.count);

    int nb = decode_bits(&cap, bits);

    if (nb != 40) {
        printf("Not enough bits: %d\n", nb);
        gpioTerminate();
        return 1;
    }

    /* ---------------- BYTES ---------------- */
    unsigned char bytes[5] = {0};

    for (int i = 0; i < 40; i++) {
        bytes[i / 8] <<= 1;
        bytes[i / 8] |= bits[i];
    }

    /* ---------------- CHECKSUM ---------------- */
    if (((bytes[0] + bytes[1] + bytes[2] + bytes[3]) & 0xFF) != bytes[4]) {
        printf("Checksum error\n");
        gpioTerminate();
        return 1;
    }

    /* ---------------- RESULT ---------------- */
    *humidity = (bytes[0] << 8 | bytes[1]) / 10.0;

    uint16_t value = (bytes[2] << 8 | bytes[3]);
    if (value & 0x8000){ // negative temperature
        *temperature = -((value & 0x7FFF) / 10.0);
    } else {
        *temperature = value / 10.0;
    }
    
    // printf("Humidity: %.1f %%\n", humidity);
    // printf("Temperature: %.1f °C\n", temperature);

    gpioTerminate();

    return 0;
}
