#include "esp_log.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "esp_heap_caps.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>


// JSON Components
#include "bjson_enc.h"
#include "bjson.h"


#define TEST_BIN_SIZE 4096

static const char* TAG = "APP";




static bool system_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(ret));
        return false;
    }

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "json",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS mount failed: %s", esp_err_to_name(ret));
        return false;
    }

    size_t total = 0, used = 0;
    esp_spiffs_info(conf.partition_label, &total, &used);
    ESP_LOGI(TAG, "SPIFFS total=%u, used=%u", (unsigned)total, (unsigned)used);
    return true;
}


static char *read_json_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) {
        ESP_LOGE(TAG, "file not found: %s", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, fp);
    fclose(fp);
    buf[sz] = '\0';

    ESP_LOGI(TAG, "Read %ld bytes from %s", sz, path);
    return buf;
}



static size_t bjson_encode_from_text(const char *json, uint8_t *out, size_t out_max)
{
    size_t out_len = 0;
    bjson_err_t rc = bjson_encode_from_json(json, out, out_max, &out_len);

    if (rc != BJSON_OK) {
        ESP_LOGE(TAG, "BJSON encode failed: %d", rc);
        return 0;
    }
    ESP_LOGI(TAG, "JSON To Bin encoded size = %u bytes", (unsigned)out_len);
    return out_len;
}



static inline uint32_t rd_u32le(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
}
static inline const uint8_t* align4p(const uint8_t* p) {
    uintptr_t x = (uintptr_t)p;
    return (const uint8_t*)((x + 3) & ~(uintptr_t)3);
}

static void bjson_dump_document(const uint8_t *data, size_t len)
{
    bjd_doc_t doc;
    if (bjd_open(data, len, &doc) != BJD_OK) {
        ESP_LOGE(TAG, "bjd_open failed");
        return;
    }

    const uint8_t* cur = doc.entries;
    const uint8_t* end = doc.base + doc.len;

    ESP_LOGI(TAG, "---- BJSON Document Dump (count=%" PRIu32 ") ----", doc.count);

    for (uint32_t i = 0; i < doc.count; i++) {
        // 최소 헤더 8바이트 체크: type(1) + nameLen(1) + reserved(2) + valueLen(4)
        if ((size_t)(end - cur) < 8) {
            ESP_LOGW(TAG, "truncated entry header at i=%" PRIu32, i);
            break;
        }

        uint8_t  type    = cur[0];
        uint8_t  nameLen = cur[1];
        uint32_t valueLen= rd_u32le(cur + 4);
        const uint8_t* name = cur + 8;
        const uint8_t* val  = name + nameLen;

        // 경계 체크
        if (name > end || val > end || (size_t)(end - val) < valueLen) {
            ESP_LOGW(TAG, "truncated entry payload at i=%" PRIu32, i);
            break;
        }

        // 키 안전 복사
        char key[65] = {0};
        size_t kcpy = nameLen < sizeof(key)-1 ? nameLen : sizeof(key)-1;
        memcpy(key, name, kcpy);
        key[kcpy] = '\0';

        // 값 출력
        switch (type) {
            case BJD_T_STR: {
                int vlen = (valueLen > 1024) ? 1024 : (int)valueLen; // 로그 폭주 방지
                ESP_LOGI(TAG, "%s = \"%.*s\"", key, vlen, (const char*)val);
                break;
            }
            case BJD_T_I16: {
                if (valueLen != 2) { ESP_LOGW(TAG, "%s: bad i16 len=%" PRIu32, key, valueLen); break; }
                int16_t v = (int16_t)(val[0] | (val[1]<<8));
                ESP_LOGI(TAG, "%s = %" PRId16, key, v);
                break;
            }
            case BJD_T_U16: {
                if (valueLen != 2) { ESP_LOGW(TAG, "%s: bad u16 len=%" PRIu32, key, valueLen); break; }
                uint16_t v = (uint16_t)(val[0] | (val[1]<<8));
                ESP_LOGI(TAG, "%s = %" PRIu16, key, v);
                break;
            }
            case BJD_T_I32: {
                if (valueLen != 4) { ESP_LOGW(TAG, "%s: bad i32 len=%" PRIu32, key, valueLen); break; }
                int32_t v = (int32_t)rd_u32le(val);
                ESP_LOGI(TAG, "%s = %" PRId32, key, v);
                break;
            }
            case BJD_T_U32: {
                if (valueLen != 4) { ESP_LOGW(TAG, "%s: bad u32 len=%" PRIu32, key, valueLen); break; }
                uint32_t v = rd_u32le(val);
                ESP_LOGI(TAG, "%s = %" PRIu32, key, v);
                break;
            }
            default:
                ESP_LOGW(TAG, "%s = (unknown type %u, len=%" PRIu32 ")", key, type, valueLen);
                break;
        }

        // 다음 엔트리로 (4바이트 정렬)
        const uint8_t* next = align4p(val + valueLen);
        if (next <= cur) { ESP_LOGW(TAG, "pointer stall; abort"); break; }
        cur = next;
        if (cur > end) { ESP_LOGW(TAG, "cursor beyond end"); break; }
    }

    ESP_LOGI(TAG, "---- Dump End ----");
}




static void bjson_process(const char *json_text)
{

    uint8_t *bin = heap_caps_malloc(TEST_BIN_SIZE, MALLOC_CAP_8BIT );
    if (!bin) {
        ESP_LOGE(TAG, "PSRAM alloc failed");
        return;
    }

    size_t bin_len = bjson_encode_from_text(json_text, bin, TEST_BIN_SIZE);
    if (!bin_len) 
      return;

    // 바이너리를 파일로 저장하거나 네트워크 전송 가능
    // 예: fwrite(bin, 1, bin_len, fp);

    bjson_dump_document(bin, bin_len);

    free(bin);
}




void app_main(void)
{
    ESP_LOGI(TAG, "=== ESP-JSON Demo Start ===");

    if (!system_init()) {
        ESP_LOGE(TAG, "System init failed");
        return;
    }

    char *json = read_json_file("/spiffs/test.json");
    if (!json) {
        ESP_LOGE(TAG, "JSON read failed");
        return;
    }

    ESP_LOGI(TAG, "JSON content:\n%s", json);

    bjson_process(json);
    free(json);

    ESP_LOGI(TAG, "=== Demo Completed ===");
}