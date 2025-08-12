#pragma once
#define APP_NAME           "Sigma"
#define APP_FILENAME       "Sigma.exe"
#define APP_DESCRIPTION    "Sigma - Ứng dụng gõ tiếng Việt"
#define APP_COMPANY        "Phạm Hoàng Nhật"

#define APP_VERSION_MAJOR  1
#define APP_VERSION_MINOR  9
#define APP_VERSION_PATCH  5

// Tạo chuỗi version: "1.0.0"
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define APP_VERSION_STRING  STRINGIFY(APP_VERSION_MAJOR) "." STRINGIFY(APP_VERSION_MINOR) "." STRINGIFY(APP_VERSION_PATCH)

// Wide string (Unicode) dùng cho .rc
#define WIDE2(x) L##x
#define WIDE(x) WIDE2(x)

#define LAPP_NAME           WIDE(APP_NAME)
#define LAPP_FILENAME       WIDE(APP_FILENAME)
#define LAPP_DESCRIPTION    WIDE(APP_DESCRIPTION)
#define LAPP_COMPANY        WIDE(APP_COMPANY)
#define LAPP_VERSION_STRING WIDE(APP_VERSION_STRING)
