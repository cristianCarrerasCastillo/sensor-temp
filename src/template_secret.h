#ifndef _SECRETS_H
#define _SECRETS_H
#include <pgmspace.h>

#define SECRET
#define THING_NAME "esp32"                                       //change this

const char AWS_IOT_ENDPOINT[] = "XXXXXXXXXXX.iot.XXXXXXXX.amazonaws.com";        //change this

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";

#endif