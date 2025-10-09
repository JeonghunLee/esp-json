# TEST RESULT



간단한 TEST 이며, Flex 와 Bison 이 필요 없을 듯 함    


```
I (27) boot: ESP-IDF v5.4 2nd stage bootloader
I (27) boot: compile time Oct  9 2025 12:23:09
I (27) boot: Multicore bootloader
I (27) boot: chip revision: v0.1
I (27) boot: efuse block revision: v1.2
I (27) qio_mode: Enabling default flash chip QIO
I (27) boot.esp32s3: Boot SPI Speed : 80MHz
I (28) boot.esp32s3: SPI Mode       : QIO
I (28) boot.esp32s3: SPI Flash Size : 4MB
I (28) boot: Enabling RNG early entropy source...
I (28) boot: Partition Table:
I (28) boot: ## Label            Usage          Type ST Offset   Length
I (29) boot:  0 nvs              WiFi data        01 02 0000d000 00008000
I (29) boot:  1 fctry            WiFi data        01 02 00015000 00004000
I (29) boot:  2 log_status       WiFi data        01 02 00019000 00004000
I (30) boot:  3 otadata          OTA data         01 00 0001d000 00002000
I (30) boot:  4 phy_init         RF data          01 01 0001f000 00001000
I (31) boot:  5 ota_0            OTA app          00 10 00020000 001ca000
I (31) boot:  6 json             Unknown data     01 82 001ea000 00010000
I (31) boot: End of partition table
I (32) esp_image: segment 0: paddr=00020020 vaddr=3c020020 size=0ae0ch ( 44556) map
I (39) esp_image: segment 1: paddr=0002ae34 vaddr=3fc91c00 size=02a00h ( 10752) load
I (41) esp_image: segment 2: paddr=0002d83c vaddr=40374000 size=027dch ( 10204) load
I (43) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=1f3fch (127996) map
I (62) esp_image: segment 4: paddr=0004f424 vaddr=403767dc size=0b35ch ( 45916) load
I (71) esp_image: segment 5: paddr=0005a788 vaddr=600fe100 size=0001ch (    28) load
I (77) boot: Loaded app from partition at offset 0x20000
I (77) boot: Disabling RNG early entropy source...
I (78) cpu_start: Multicore app
I (87) cpu_start: Pro cpu start user code
I (87) cpu_start: cpu freq: 240000000 Hz
I (87) app_init: Application information:
I (87) app_init: Project name:     esp-json
I (87) app_init: App version:      61b8ecf
I (87) app_init: Compile time:     Oct  9 2025 12:22:17
I (88) app_init: ELF file SHA256:  af33d2252...
I (88) app_init: ESP-IDF:          v5.4
I (88) efuse_init: Min chip rev:     v0.0
I (88) efuse_init: Max chip rev:     v0.99
I (88) efuse_init: Chip rev:         v0.1
I (89) heap_init: Initializing. RAM available for dynamic allocation:
I (89) heap_init: At 3FC94EE0 len 00054830 (338 KiB): RAM
I (89) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (90) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (90) heap_init: At 600FE11C len 00001ECC (7 KiB): RTCRAM
I (91) spi_flash: detected chip: generic
I (91) spi_flash: flash io: qio
W (91) spi_flash: Detected size(8192k) larger than the size in the binary image header(4096k). Using the size in the binary image header. 
I (91) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (92) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (92) main_task: Started on CPU0
I (140) main_task: Calling app_main()
I (140) APP: === ESP-JSON Demo Start ===
I (146) APP: SPIFFS total=52961, used=502
I (147) APP: Read 117 bytes from /spiffs/test.json
I (147) APP: JSON content:
{
  STR_32_DEVICE_NAME: "esp32s3",
  STR_64_OWNER: "Jeonghun",
  INT16_RATE: 480,
  UINT32_PACKET_MAX: 1048576
}
I (148) APP: JSON To Bin encoded size = 128 bytes
I (148) APP: ---- BJSON Document Dump (count=4) ----
I (148) APP: STR_32_DEVICE_NAME = "esp32s3"
I (149) APP: STR_64_OWNER = "Jeonghun"
I (149) APP: INT16_RATE = 480
I (149) APP: UINT32_PACKET_MAX = 1048576
I (149) APP: ---- Dump End ----
I (149) APP: === Demo Completed ===
I (149) main_task: Returned from app_main()
```