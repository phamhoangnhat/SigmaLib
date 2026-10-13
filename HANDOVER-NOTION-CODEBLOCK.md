# Handover — Fix lỗi gõ tiếng Việt trong Notion Code Block (SigmaLib 2.3.5)

> Tài liệu bàn giao cho tác giả gốc. Mọi phân tích dưới đây đều có bằng chứng thực测 trên máy
> Windows 11 + Chrome + Notion web, mô tả cách tái hiện kèm theo để kiểm chứng độc lập.

## 1. Tóm tắt

Gõ tiếng Việt bằng Telex trong **code block của Notion web** cho kết quả sai:

| Cấu hình | Kết quả khi gõ "đang gõ sẽ trở thành" |
|---|---|
| "Tương thích với ứng dụng có gợi ý từ" TẮT | `dđang goõ seẽ troơở thanhành` |
| "Tương thích với ứng dụng có gợi ý từ" BẬT | `d·đang go·õ se·ẽ tro·ơ·ở thanh·ành` |

Mọi app khác và mọi vị trí gõ khác của Notion đều bình thường.

## 2. Gốc rễ (đã chứng minh bằng thực nghiệm)

`TypeWord::fixStringDisplay` gửi **một lô `SendInput` duy nhất** chứa toàn bộ chuỗi
`[mồi ·] [VK_BACK]×N [ký tự UNICODE]`. Trong editor xử lý bất đồng bộ (code block Notion,
một số editor web async khác), **các phím xóa VK_BACK đến dính chùm trong cùng một chu kỳ
xử lý bị âm thầm bỏ rơi**, trong khi các ký tự chèn `KEYEVENTF_UNICODE` vẫn vào bình thường.
Kết quả: ký tự cũ không được xóa, hậu tố mới nối tiếp → sai số cộng dồn trong cả từ.
Dấu `·` (ký tự mồi của chế độ tương thích gợi ý từ) cũng vì vậy mà ở lại văn bản.

Bằng chứng quyết định: từ `goõ` là từ duy nhất gõ đúng trong câu test — vì lần viết lại của nó
có `numBackspace = 0` (chỉ thêm dấu, không cần xóa) → xác nhận **chỉ riêng phép XÓA là hỏng**.

## 3. Quá trình loại trừ (để không ai phải thử lại)

Đã test và **loại trừ** các giả thuyết sau:

1. **Điền scancode cho VK_BACK** (`wScan` từ `MapVirtualKeyW`): không giúp gì.
2. **VK_BACK thuần KEYEVENTF_SCANCODE**: không giúp gì.
3. **Tách lô xóa/chèn với sleep 15ms giữa hai lô** (xóa vẫn dính chùm trong lô của nó): không giúp gì.
4. **Backspace Unicode U+0008** (KEYEVENTF_UNICODE): **vô tác dụng hoàn toàn ở mọi app**, kể cả Notepad.

Phương pháp chẩn đoán: gửi tuần tự 5 phương pháp xóa, mỗi phương pháp sau một ký tự đánh dấu
`A B C D E`, mỗi bước cách 30ms. Kết quả ở cả Notion code block lẫn Notepad đều là `aCEa`
(chữ `a` cuối là phím vật lý rò qua do chuỗi ~320ms vượt LowLevelHooksTimeout — chỉ hiện tượng
test): `A`, `B`, `D` bị xóa (VK_BACK cách 30ms **đều hoạt động**), `C`, `E` sống sót (U+0008 chết).

**Kết luận: VK_BACK tổng hợp hoạt động bình thường trong code block Notion — với điều kiện
mỗi phím được gửi bằng một lô SendInput riêng và cách nhau đủ thời gian để editor xử lý xong
chu kỳ trước đó.**

## 4. Fix

Thêm setting per-app mới **`modeFixModernEditor`** — checkbox
**"Tương thích với trình soạn thảo hiện đại"** trong cấu hình từng ứng dụng
(mặc định BẬT cho các trình duyệt qua `listAppFixModernEditor`), theo đúng pattern của
`modeFixAutoSuggest` hiện có. Khi bật, `TypeWord::fixStringDisplay`:

1. Gửi mồi `·` (nếu `modeFixAutoSuggest` bật), nghỉ **10ms**.
2. Mỗi phím VK_BACK một lô `SendInput` riêng, nghỉ **20ms** giữa các phím
   (10ms nếu chuỗi ≥ 12 bước để tránh lâu).
3. Chèn ký tự mới ngay sau phím xóa cuối.

Kèm hai cải tiến an toàn:

- **Chữ ký `dwExtraInfo` (`SIGMA_INPUT_SIGNATURE`)** cho mọi phím `addInput` tự gửi; hook
  `WH_KEYBOARD_LL` bỏ qua hẳn phím có chữ ký — triệt tiêu lớp lỗi "phím của chính mình bị
  xử lý lại như phím người dùng" khi callback đến sau khi `flagSendingKey` đã tắt
  (lộ rõ khi gửi rải rác + gõ nhanh).
- Mọi đường khác (`fixStringDisplayCliboard`, `removeWordBack`, `moveLeft`, `moveRight`)
  và hành vi khi **tắt** setting: giữ nguyên 100% như bản cũ.

## 5. Bằng chứng test cuối

- Notion code block (Chrome): `đang gõ sẽ trở thành` — đúng, kể cả gõ nhanh.
- ZCode (Electron): đúng.
- Regression: Notepad, Notion block thường — không đổi (setting mặc định tắt cho app ngoài trình duyệt).

## 6. Files thay đổi

| File | Thay đổi |
|---|---|
| `TypeWord.cpp/.h` | `fixStringDisplay`: nhánh gửi rải rác khi `modeFixModernEditor`; `addInput` gắn `dwExtraInfo` chữ ký |
| `Listener.cpp/.h` | hook bỏ qua phím có `SIGMA_INPUT_SIGNATURE`; định nghĩa chữ ký |
| `Variable.cpp/.h` | `modeFixModernEditor`, `listAppFixModernEditor` (mặc định: các trình duyệt), nạp trong `loadSettingsWindow` |
| `ConfigUi.cpp/.h` | checkbox "Tương thích với trình soạn thảo hiện đại" (đầy đủ wiring như các checkbox per-app khác) |
| `version.h` | 2.3.4 → 2.3.5 |
| `CMakeLists.txt`, `build-sigma.ps1` | (tùy chọn cho maintainer) đường build CMake không cần Qt VS Tools |

## 7. Hạn chế còn lại (đề xuất theo hướng phát triển)

- Không phân biệt được site/webview trong cùng một trình duyệt (nhận diện theo tên exe) —
  setting áp dụng cho cả trình duyệt; chi phí là ~20ms/phím xóa khi từ đang được sửa.
- Nếu cần tối ưu thêm: khoảng nghỉ là các hằng số trong `fixStringDisplay` (20/10/10ms),
  có thể tinh chỉnh theo từng editor nếu sau này có kênh phát hiện chính xác hơn.
- Hướng dài hạn: phát diện editor async theo UI Automation / accessibility thay vì opt-in thủ công.
