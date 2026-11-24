# Cài đặt DES và AES-128 bằng C++

Bài tập về nhà môn Lý thuyết mật mã: cài đặt thuật toán **DES** và **AES-128**, in giá trị trung gian ở từng bước để kiểm định với test vector.

## Cấu trúc

```
.
├── aes.cpp                 # AES-128: key expansion, SubBytes, ShiftRows, MixColumns, AddRoundKey (+ giải mã)
├── des.cpp                 # DES: sinh 16 khóa con, hàm Feistel, 16 vòng (+ giải mã)
├── output/
│   ├── aes_output.txt      # kết quả chạy AES (giá trị trung gian từng vòng)
│   └── des_output.txt      # kết quả chạy DES (khóa con, L/R từng vòng)
└── README.md
```

## Build và chạy

```bash
g++ -std=c++17 -o aes aes.cpp
g++ -std=c++17 -o des des.cpp

./aes > output/aes_output.txt     # chạy các test vector có sẵn
./des > output/des_output.txt

# chạy vector tùy chọn: ./aes <key 32 hex> <plaintext 32 hex>
./aes 2b7e151628aed2a6abf7158809cf4f3c 3243f6a8885a308d313198a2e0370734
# chạy vector tùy chọn: ./des <key 16 hex> <plaintext 16 hex>
./des 133457799BBCDFF1 0123456789ABCDEF
```

## Giá trị được in ra

**AES-128** (state in theo cột, cùng định dạng FIPS-197):
- 11 round key
- Mỗi vòng: `start`, `s_box` (SubBytes), `s_row` (ShiftRows), `m_col` (MixColumns, vòng 10 không có), `k_sch` (AddRoundKey)

**DES**:
- C0/D0, C_i/D_i, 16 khóa con K1..K16 (dạng bit và hex)
- Sau hoán vị đầu IP, L0, R0
- Mỗi vòng: E(R), E(R) xor K, đầu ra S-box, f(R,K), L_i, R_i
- 

## Test vector đã kiểm chứng

| Thuật toán | Key | Plaintext | Ciphertext |
|---|---|---|---|
| AES-128 (FIPS-197 App. B) | `2b7e151628aed2a6abf7158809cf4f3c` | `3243f6a8885a308d313198a2e0370734` | `3925841d02dc09fbdc118597196a0b32` |
| AES-128 (FIPS-197 App. C.1) | `000102030405060708090a0b0c0d0e0f` | `00112233445566778899aabbccddeeff` | `69c4e0d86a7b0430d8cdb78070b4c55a` |
| DES (ví dụ kinh điển) | `133457799BBCDFF1` | `0123456789ABCDEF` | `85E813540F0AB405` |
| DES (NBS) | `0E329232EA6D0D73` | `8787878787878787` | `0000000000000000` |

Chương trình cũng giải mã ngược ciphertext và kiểm tra ra đúng plaintext ban đầu.

## Ghi chú cài đặt

- **AES**: S-box tra bảng; phép nhân trong GF(2^8) cài bằng `xtime` (nhân 2, XOR `0x1b` khi tràn) và `gmul`.
- **DES**: các bảng hoán vị đánh số bit từ 1 (bit 1 là MSB); hàm `permute` dùng chung cho IP, FP, E, P, PC1, PC2.
