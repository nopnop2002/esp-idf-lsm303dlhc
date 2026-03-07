# I2Cdev library collection   
I used [this](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino) I2Cdev library collection by Jeff Rowberg.   
However, I2Cdev has been rewritten for ESP-IDF.   

# websocket   
I used [this](https://github.com/Molorius/esp32-websocket) ESP-IDF WebSocket Component.   
The original uses mbedtls_sha1(), but I changed to mbedtls_md().   
This is because mbedtls_sha1() was deprecated in ESP-IDF V6.   
