# lorawan-esp32-s3-rfm95w
simple lorawan project for ttn and chirpstack



see here for flash size and clock data to be configured in sdkconfig / menuconfig
https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html

WROOM 1 flash is 8mb and WROOM 2 - 16mb or 32mb depending on module code

All S3 WROOM clocks are 240mHz


idf setup:

```
grep get_idf .bashrc 
alias get_idf='. $HOME/esp/esp-idf/export.sh'
```

```
get_idf;idf.py set-target esp32s3
```


idf.py set-target will clear the build directory and re-generate the sdkconfig file from scratch. The old sdkconfig file will be saved as sdkconfig.old.
