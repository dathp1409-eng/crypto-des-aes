#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
using std::string;  

typedef uint8_t byte;

// S-box (FIPS-197 Fig.7)
static const byte SBOX[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};
static byte INV_SBOX[256];                 // tinh tu SBOX trong initInvSbox()
static const byte RCON[10] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};

// ---------- tien ich ----------
void initInvSbox() { for (int i = 0; i < 256; i++) INV_SBOX[SBOX[i]] = (byte)i; }

// hex string -> 16 byte
bool parseHex16(const string& h, byte out[16]) {
    if (h.size() != 32) return false;
    for (int i = 0; i < 16; i++) out[i] = (byte)strtoul(h.substr(2 * i, 2).c_str(), nullptr, 16);
    return true;
}

// In 16 byte theo thu tu cot (giong cach FIPS-197 in state)

}

// state[r][c] <-> mang 16 byte: byte thu i nam o hang i%4, cot i/4
void toState(const byte in[16], byte s[4][4]) { for (int i = 0; i < 16; i++) s[i % 4][i / 4] = in[i]; }
void fromState(byte s[4][4], byte out[16])   { for (int i = 0; i < 16; i++) out[i] = s[i % 4][i / 4]; }
void printState(const char* label, byte s[4][4]) { byte t[16]; fromState(s, t); printBytes(label, t); }

// ---------- GF(2^8) voi da thuc toi thieu x^8+x^4+x^3+x+1 (0x11b) ----------
// Nhan 2: dich trai 1 bit; neu bit cao truoc khi dich = 1 thi XOR 0x1b (rut gon modulo)
byte xtime(byte x) { return (byte)((x << 1) ^ ((x & 0x80) ? 0x1b : 0x00)); }

// Nhan tong quat: cong don a*2^k tuong ung cac bit cua b
byte gmul(byte a, byte b) {
    byte p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        a = xtime(a);
        b >>= 1;
    }
    return p;
}

// ---------- Key expansion: 16 byte key -> 11 round key (moi cai 16 byte, theo cot) ----------
void keyExpansion(const byte key[16], byte rk[11][16]) {
    byte w[44][4];
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) w[i][j] = key[4 * i + j];
    for (int i = 4; i < 44; i++) {
        byte t[4];
        memcpy(t, w[i - 1], 4);
        if (i % 4 == 0) {
            byte tmp = t[0];                         // RotWord: [a0 a1 a2 a3] -> [a1 a2 a3 a0]
            t[0] = t[1]; t[1] = t[2]; t[2] = t[3]; t[3] = tmp;
            for (int j = 0; j < 4; j++) t[j] = SBOX[t[j]];   // SubWord
            t[0] ^= RCON[i / 4 - 1];                         // XOR Rcon vao byte dau
        }
        for (int j = 0; j < 4; j++) w[i][j] = w[i - 4][j] ^ t[j];
    }
    for (int r = 0; r < 11; r++)
        for (int c = 0; c < 4; c++)
            for (int j = 0; j < 4; j++) rk[r][4 * c + j] = w[4 * r + c][j];
}

// ---------- 4 phep bien doi ----------
void subBytes(byte s[4][4])    { for (int r = 0; r < 4; r++) for (int c = 0; c < 4; c++) s[r][c] = SBOX[s[r][c]]; }
void invSubBytes(byte s[4][4]) { for (int r = 0; r < 4; r++) for (int c = 0; c < 4; c++) s[r][c] = INV_SBOX[s[r][c]]; }

// Hang r dich trai r vi tri
void shiftRows(byte s[4][4]) {
    for (int r = 1; r < 4; r++) {
        byte t[4];
        for (int c = 0; c < 4; c++) t[c] = s[r][(c + r) % 4];
        for (int c = 0; c < 4; c++) s[r][c] = t[c];
    }
}
// Hang r dich phai r vi tri
void invShiftRows(byte s[4][4]) {
    for (int r = 1; r < 4; r++) {
        byte t[4];
        for (int c = 0; c < 4; c++) t[(c + r) % 4] = s[r][c];
        for (int c = 0; c < 4; c++) s[r][c] = t[c];
    }
}

// Nhan moi cot voi ma tran circ(2,3,1,1)
void mixColumns(byte s[4][4]) {
    for (int c = 0; c < 4; c++) {
        byte a0 = s[0][c], a1 = s[1][c], a2 = s[2][c], a3 = s[3][c];   // luu gia tri cu
        s[0][c] = gmul(a0, 2) ^ gmul(a1, 3) ^ a2 ^ a3;
        s[1][c] = a0 ^ gmul(a1, 2) ^ gmul(a2, 3) ^ a3;
        s[2][c] = a0 ^ a1 ^ gmul(a2, 2) ^ gmul(a3, 3);
        s[3][c] = gmul(a0, 3) ^ a1 ^ a2 ^ gmul(a3, 2);
    }
}
// Nghich dao: ma tran circ(14,11,13,9)
void invMixColumns(byte s[4][4]) {
    for (int c = 0; c < 4; c++) {
        byte a0 = s[0][c], a1 = s[1][c], a2 = s[2][c], a3 = s[3][c];
        s[0][c] = gmul(a0, 14) ^ gmul(a1, 11) ^ gmul(a2, 13) ^ gmul(a3, 9);
        s[1][c] = gmul(a0, 9)  ^ gmul(a1, 14) ^ gmul(a2, 11) ^ gmul(a3, 13);
        s[2][c] = gmul(a0, 13) ^ gmul(a1, 9)  ^ gmul(a2, 14) ^ gmul(a3, 11);
        s[3][c] = gmul(a0, 11) ^ gmul(a1, 13) ^ gmul(a2, 9)  ^ gmul(a3, 14);
    }
}

void addRoundKey(byte s[4][4], const byte rk[16]) {
    for (int i = 0; i < 16; i++) s[i % 4][i / 4] ^= rk[i];
}

// ---------- Ma hoa (verbose = in trung gian) ----------
void encrypt(const byte in[16], const byte key[16], byte out[16], bool verbose) {
    byte rk[11][16], s[4][4];
    keyExpansion(key, rk);
    if (verbose) {
        printf("--- Round keys ---\n");
        for (int r = 0; r < 11; r++) { char l[32]; snprintf(l, sizeof l, "RoundKey[%d]", r); printBytes(l, rk[r]); }
        printf("\n--- Ma hoa ---\n");
    }
    toState(in, s);
    if (verbose) printState("input", s);
    addRoundKey(s, rk[0]);
    if (verbose) printState("round[ 0].k_sch", s);

    for (int round = 1; round <= 10; round++) {
        char l[40];
        if (verbose) { snprintf(l, sizeof l, "round[%2d].start", round); printState(l, s); }
        subBytes(s);
        if (verbose) { snprintf(l, sizeof l, "round[%2d].s_box", round); printState(l, s); }
        shiftRows(s);
        if (verbose) { snprintf(l, sizeof l, "round[%2d].s_row", round); printState(l, s); }
        if (round != 10) {                       // vong cuoi KHONG co MixColumns
            mixColumns(s);
            if (verbose) { snprintf(l, sizeof l, "round[%2d].m_col", round); printState(l, s); }
        }
        addRoundKey(s, rk[round]);
        if (verbose) { snprintf(l, sizeof l, "round[%2d].k_sch", round); printState(l, s); }
    }
    fromState(s, out);
}

// ---------- Giai ma ----------
void decrypt(const byte in[16], const byte key[16], byte out[16]) {
    byte rk[11][16], s[4][4];
    keyExpansion(key, rk);
    toState(in, s);
    addRoundKey(s, rk[10]);
    for (int round = 9; round >= 0; round--) {
        invShiftRows(s);
        invSubBytes(s);
        addRoundKey(s, rk[round]);
        if (round != 0) invMixColumns(s);
    }
    fromState(s, out);
}

// Chay 1 test: in trung gian, so voi ket qua mong doi (neu co)
bool runTest(const char* name, const string& keyHex, const string& ptHex, const char* expectHex) {
    byte key[16], pt[16], ct[16], back[16];
    if (!parseHex16(keyHex, key) || !parseHex16(ptHex, pt)) { printf("Hex khong hop le\n"); return false; }
    printf("==================== %s ====================\n", name);
    printBytes("Key:", key);
    printBytes("Plaintext:", pt);
    printf("\n");
    encrypt(pt, key, ct, true);
    printf("\n");
    printBytes("Ciphertext:", ct);
    decrypt(ct, key, back);
    printBytes("Decrypt lai:", back);
    bool ok = memcmp(back, pt, 16) == 0;
    printf("Giai ma khop plaintext: %s\n", ok ? "OK" : "SAI");
    if (expectHex) {
        byte ex[16];
        parseHex16(expectHex, ex);
        bool m = memcmp(ct, ex, 16) == 0;
        printf("Khop ciphertext mong doi (%s): %s\n", expectHex, m ? "OK" : "SAI");
        ok = ok && m;
    }
    printf("\n");
    return ok;
}

int main(int argc, char** argv) {
    initInvSbox();
    if (argc == 3) return runTest("Vector tu chon", argv[1], argv[2], nullptr) ? 0 : 1;
    bool ok = true;
    ok &= runTest("FIPS-197 Appendix B",
                  "2b7e151628aed2a6abf7158809cf4f3c", "3243f6a8885a308d313198a2e0370734",
                  "3925841d02dc09fbdc118597196a0b32");
    ok &= runTest("FIPS-197 Appendix C.1",
                  "000102030405060708090a0b0c0d0e0f", "00112233445566778899aabbccddeeff",
                  "69c4e0d86a7b0430d8cdb78070b4c55a");
    printf("TONG KET: %s\n", ok ? "TAT CA TEST DAT" : "CO TEST SAI");
    return ok ? 0 : 1;
}
