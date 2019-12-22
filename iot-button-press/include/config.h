// This file contains your configuration used to connect to Cloud IoT Core

// WIFI
const char* ssid = "****";
const char* password = "****";

// Cloud iot details.
const char* project_id = "****";
const char* location = "us-central1";
const char* registry_id = "****";
const char* device_id = "****";

// Configuration for NTP
const char* ntp_primary = "pool.ntp.org";
const char* ntp_secondary = "time.nist.gov";

// To get the private key run (where private-key.pem is the ec private key
// used to create the certificate uploaded to google cloud iot):
// openssl ec -in <private-key.pem> -noout -text
// and copy priv: part.
// The key length should be exactly the same as the key length bellow (32 pairs
// of hex digits). If it's bigger and it starts with "00:" delete the "00:". If
// it's smaller add "00:" to the start. If it's too big or too small something
// is probably wrong with your key.
const char* private_key_str =
    "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
    "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
    "00:00";

// Time (seconds) to expire token += 20 minutes for drift
const int jwt_exp_secs = 3600; // Maximum 24H (3600*24)

// In case we ever need extra topics
const int ex_num_topics = 0;
const char* ex_topics[ex_num_topics];
