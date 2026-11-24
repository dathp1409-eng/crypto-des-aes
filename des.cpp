#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
using std::string;

typedef uint64_t u64;
typedef uint32_t u32;

// ---------- Cac bang hang so (FIPS 46-3) ----------
static const int IP[64] = {
    58,50,42,34,26,18,10,2, 60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6, 64,56,48,40,32,24,16,8,
    57,49,41,33,25,17, 9,1, 59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5, 63,55,47,39,31,23,15,7};
static const int FP[64] = {
    40,8,48,16,56,24,64,32, 39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30, 37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28, 35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26, 33,1,41, 9,49,17,57,25};
static const int E[48] = {
    32, 1, 2, 3, 4, 5,  4, 5, 6, 7, 8, 9,
     8, 9,10,11,12,13, 12,13,14,15,16,17,
    16,17,18,19,20,21, 20,21,22,23,24,25,
    24,25,26,27,28,29, 28,29,30,31,32, 1};
static const int P[32] = {
    16, 7,20,21,29,12,28,17,  1,15,23,26, 5,18,31,10,
     2, 8,24,14,32,27, 3, 9, 19,13,30, 6,22,11, 4,25};
static const int PC1[56] = {
    57,49,41,33,25,17, 9,  1,58,50,42,34,26,18,
    10, 2,59,51,43,35,27, 19,11, 3,60,52,44,36,
    63,55,47,39,31,23,15,  7,62,54,46,38,30,22,
    14, 6,61,53,45,37,29, 21,13, 5,28,20,12, 4};
static const int PC2[48] = {
    14,17,11,24, 1, 5,  3,28,15, 6,21,10,
    23,19,12, 4,26, 8, 16, 7,27,20,13, 2,
    41,52,31,37,47,55, 30,40,51,45,33,48,
    44,49,39,56,34,53, 46,42,50,36,29,32};
static const int SHIFTS[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
static const int SBOX[8][4][16] = {
  {{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
   {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
  {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
   {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
  {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
   {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
  {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
   {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
  {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
   {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
  {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
   {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
  {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
   {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
  {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
   {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};

// ---------- Tien ich ----------
// Hoan vi bit: out[i] = in[table[i]] (bit danh so tu 1, tinh tu MSB cua nIn bit)
u64 permute(u64 in, const int* table, int nOut, int nIn) {
    u64 out = 0;
    for (int i = 0; i < nOut; i++) {
        out <<= 1;
        out |= (in >> (nIn - table[i])) & 1;
    }
    return out;
}

// Quay trai 28 bit (dung cho nua C, D cua khoa)
u32 rotl28(u32 x, int n) { return ((x << n) | (x >> (28 - n))) & 0x0FFFFFFF; }

// In so nguyen duoi dang chuoi bit, nhom 'group' bit cho de doc
void printBits(const char* label, u64 v, int nbits, int group) {
    printf("%-16s", label);
    for (int i = nbits - 1; i >= 0; i--) {
        printf("%d", (int)((v >> i) & 1));
        if (i % group == 0 && i != 0) printf(" ");
    }
    printf("\n");
}
void printHex(const char* label, u64 v, int hexDigits) { printf("%-16s%0*llX\n", label, hexDigits, (unsigned long long)v); }

// ---------- Sinh 16 khoa con ----------
void keySchedule(u64 key, u64 K[16], bool verbose) {
    u64 cd = permute(key, PC1, 56, 64);              // PC-1: 64 -> 56 bit (bo bit parity)
    u32 C = (u32)(cd >> 28) & 0x0FFFFFFF;
    u32 D = (u32)cd & 0x0FFFFFFF;
    if (verbose) { printBits("C0", C, 28, 4); printBits("D0", D, 28, 4); printf("\n"); }
    for (int i = 0; i < 16; i++) {
        C = rotl28(C, SHIFTS[i]);
        D = rotl28(D, SHIFTS[i]);
        K[i] = permute(((u64)C << 28) | D, PC2, 48, 56);  // PC-2: 56 -> 48 bit
        if (verbose) {
            char l[16];
            snprintf(l, sizeof l, "C%d", i + 1); printBits(l, C, 28, 4);
            snprintf(l, sizeof l, "D%d", i + 1); printBits(l, D, 28, 4);
            snprintf(l, sizeof l, "K%d", i + 1); printBits(l, K[i], 48, 6);
            snprintf(l, sizeof l, "K%d (hex)", i + 1); printHex(l, K[i], 12);
            printf("\n");
        }
    }
}

// ---------- Ham Feistel f(R, K) ----------
u32 feistel(u32 R, u64 K, bool verbose) {
    u64 er = permute(R, E, 48, 32);                  // mo rong 32 -> 48 bit
    u64 x = er ^ K;                                  // XOR voi khoa con
    u32 sOut = 0;
    for (int i = 0; i < 8; i++) {
        int six = (int)((x >> (42 - 6 * i)) & 0x3F); // 6 bit cho S-box thu i
        int row = ((six >> 4) & 2) | (six & 1);      // hang = bit 1 va bit 6
        int col = (six >> 1) & 0xF;                  // cot = 4 bit giua
        sOut = (sOut << 4) | (u32)SBOX[i][row][col];
    }
    u32 res = (u32)permute(sOut, P, 32, 32);         // hoan vi P
    if (verbose) {
        printBits("  E(R)", er, 48, 6);
        printBits("  E(R) xor K", x, 48, 6);
        printBits("  S-box out", sOut, 32, 4);
        printBits("  P(S-box)=f", res, 32, 4);
    }
    return res;
}

// ---------- Ma hoa / giai ma 1 khoi 64 bit ----------
// decrypt = true: dung 16 khoa con theo thu tu nguoc lai
u64 desBlock(u64 block, u64 key, bool decrypt, bool verbose) {
    u64 K[16];
    keySchedule(key, K, verbose);
    if (verbose) printf("--- %s ---\n", decrypt ? "Giai ma" : "Ma hoa");

    u64 ip = permute(block, IP, 64, 64);
    u32 L = (u32)(ip >> 32), R = (u32)ip;
    if (verbose) {
        printHex("Sau IP", ip, 16);
        printHex("L0", L, 8);
        printHex("R0", R, 8);
        printf("\n");
    }
    for (int i = 0; i < 16; i++) {
        u64 k = decrypt ? K[15 - i] : K[i];
        if (verbose) printf("Vong %d:\n", i + 1);
        u32 f = feistel(R, k, verbose);
        u32 newR = L ^ f;                            // R_i = L_{i-1} xor f(R_{i-1}, K_i)
        L = R;                                       // L_i = R_{i-1}
        R = newR;
        if (verbose) {
            char l[16];
            snprintf(l, sizeof l, "  L%d", i + 1); printHex(l, L, 8);
            snprintf(l, sizeof l, "  R%d", i + 1); printHex(l, R, 8);
            printf("\n");
        }
    }
    u64 pre = ((u64)R << 32) | L;                    // dao 2 nua truoc khi FP (R16 L16)
    u64 out = permute(pre, FP, 64, 64);
    if (verbose) { printHex("R16L16", pre, 16); printHex("Sau FP", out, 16); }
    return out;
}

bool parseHex64(const string& h, u64& out) {
    if (h.size() != 16) return false;
    char* end = nullptr;
    out = strtoull(h.c_str(), &end, 16);
    return *end == '\0';
}

bool runTest(const char* name, const string& keyHex, const string& ptHex, const char* expectHex) {
    u64 key, pt;
    if (!parseHex64(keyHex, key) || !parseHex64(ptHex, pt)) { printf("Hex khong hop le\n"); return false; }
    printf("==================== %s ====================\n", name);
    printHex("Key:", key, 16);
    printHex("Plaintext:", pt, 16);
    printf("\n");
    u64 ct = desBlock(pt, key, false, true);
    printf("\n");
    printHex("Ciphertext:", ct, 16);
    u64 back = desBlock(ct, key, true, false);
    printHex("Decrypt lai:", back, 16);
    bool ok = (back == pt);
    printf("Giai ma khop plaintext: %s\n", ok ? "OK" : "SAI");
    if (expectHex) {
        u64 ex;
        parseHex64(expectHex, ex);
        bool m = (ct == ex);
        printf("Khop ciphertext mong doi (%s): %s\n", expectHex, m ? "OK" : "SAI");
        ok = ok && m;
    }
    printf("\n");
    return ok;
}

int main(int argc, char** argv) {
    if (argc == 3) return runTest("Vector tu chon", argv[1], argv[2], nullptr) ? 0 : 1;
    bool ok = true;
    ok &= runTest("Vi du kinh dien (Grabbe)", "133457799BBCDFF1", "0123456789ABCDEF", "85E813540F0AB405");
    ok &= runTest("Vector NBS", "0E329232EA6D0D73", "8787878787878787", "0000000000000000");
    printf("TONG KET: %s\n", ok ? "TAT CA TEST DAT" : "CO TEST SAI");
    return ok ? 0 : 1;
}
