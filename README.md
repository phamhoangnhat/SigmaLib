# SigmaLib – Thư viện xử lý tiếng Việt cho Sigma

**SigmaLib** là thư viện động (`.dll`) phục vụ cho phần mềm gõ tiếng Việt **[Sigma](https://github.com/phamhoangnhat/Sigma)**, cung cấp các chức năng xử lý văn bản, kiểm tra ngữ âm, cấu hình bộ gõ, và hỗ trợ AI gợi ý từ.

---

## Hướng dẫn sử dụng và cài đặt

Xem hướng dẫn chi tiết cách sử dụng và cài đặt phần mềm tại liên kết sau:  
**[Tài liệu hướng dẫn sử dụng (HTML)](https://htmlpreview.github.io/?https://github.com/phamhoangnhat/Sigma/blob/main/Sigma%20User%20Guide.html)**

---

## Giới thiệu

SigmaLib được thiết kế như một **thư viện độc lập**, giúp tách phần xử lý logic khỏi giao diện người dùng của phần mềm Sigma. Mục tiêu là giúp dễ dàng tái sử dụng, bảo trì và mở rộng các thuật toán xử lý tiếng Việt trong môi trường Qt.

---

## Hướng dẫn sử dụng

Để sử dụng SigmaLib trong phần mềm của bạn hoặc khi build Sigma:

- **Đảm bảo `SigmaLib.dll` nằm cùng thư mục với file thực thi (`Sigma.exe`)**
- Repo Sigma sẽ tự động tải và liên kết với `SigmaLib.dll` khi khởi động

---

## Build thư viện

### Yêu cầu

- **Qt 6.9.0**
- **Visual Studio 2022** hoặc compiler tương đương
- Hệ điều hành: **Windows 10 trở lên**

### Các bước build

```bash
git clone https://github.com/phamhoangnhat/SigmaLib.git
cd SigmaLib
# Mở project bằng Qt Creator hoặc Visual Studio để build SigmaLib.dll
