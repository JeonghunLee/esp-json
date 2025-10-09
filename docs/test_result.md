# TEST RESULT

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