# SigmaLib – Thư viện xử lý tiếng Việt cho Sigma

**SigmaLib** là thư viện động (`.dll`) phục vụ cho phần mềm gõ tiếng Việt **[Sigma](https://github.com/phamhoangnhat/Sigma)**, cung cấp toàn bộ các chức năng chính của ứng dụng.

---

## Hướng dẫn sử dụng và cài đặt

Xem hướng dẫn chi tiết cách sử dụng và cài đặt phần mềm tại liên kết sau:  
**[Tài liệu hướng dẫn sử dụng (HTML)](https://htmlpreview.github.io/?https://github.com/phamhoangnhat/Sigma/blob/main/Sigma%20User%20Guide.html)**

---

## Giới thiệu

## Giới thiệu

SigmaLib được thiết kế như một **thư viện động độc lập**, giúp tách phần xử lý logic khỏi file thực thi chính của phần mềm Sigma. Mục tiêu là để có thể **cập nhật các tính năng mà không cần build lại toàn bộ phần mềm Sigma**, chỉ cần thay thế `SigmaLib.dll`. Điều này giúp đơn giản hóa quá trình bảo trì, cập nhật và phát triển mở rộng.


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
# Mở project bằng Visual Studio để build SigmaLib.dll
