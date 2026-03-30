#ifndef DHT22_H
#define DHT22_H

#ifdef __cplusplus
extern "C" {
    #endif
    int getMeasures(float* humidity, float* temperature);
    #ifdef __cplusplus
}
#endif

#endif