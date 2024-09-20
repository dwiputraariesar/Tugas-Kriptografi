#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#define IDC_PROCESS 101
#define IDC_TEXTBOX 102
#define IDC_KEYBOX 103
#define IDC_RESULT_VIGENERE 104
#define IDC_VIGENERE_RADIO 105
#define IDC_PLAYFAIR_RADIO 106
#define IDC_HILL_RADIO 107
#define IDC_ENCRYPT 108
#define IDC_DECRYPT 109

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Existing Vigenere Cipher functions
std::wstring VigenereCipher(const std::wstring& text, const std::wstring& key) {
    std::wstring result;
    int keyLength = key.length();
    for (size_t i = 0; i < text.length(); i++) {
        wchar_t c = toupper(text[i]);
        if (iswalpha(c)) {
            wchar_t encryptedChar = (c + toupper(key[i % keyLength]) - 2 * 'A') % 26 + 'A';
            result += encryptedChar;
        }
        else {
            result += c; // Keep non-alphabet characters unchanged
        }
    }
    return result;
}

std::wstring VigenereDecipher(const std::wstring& text, const std::wstring& key) {
    std::wstring result;
    int keyLength = key.length();
    for (size_t i = 0; i < text.length(); i++) {
        wchar_t c = toupper(text[i]);
        if (iswalpha(c)) {
            wchar_t decryptedChar = (c - toupper(key[i % keyLength]) + 26) % 26 + 'A';
            result += decryptedChar;
        }
        else {
            result += c; // Keep non-alphabet characters unchanged
        }
    }
    return result;
}

// Existing Playfair Cipher functions
std::wstring FormatTextForPlayfair(const std::wstring& text) {
    std::wstring result;
    for (size_t i = 0; i < text.length(); i++) {
        if (iswalpha(text[i])) {
            result += toupper(text[i]);
        }
    }
    if (result.length() % 2 != 0) {
        result += L'X'; // Pad with 'X' if length is odd
    }
    return result;
}

std::vector<std::vector<wchar_t>> CreatePlayfairMatrix(const std::wstring& key) {
    std::wstring formattedKey;
    for (size_t i = 0; i < key.length(); i++) {
        wchar_t c = toupper(key[i]);
        if (formattedKey.find(c) == std::wstring::npos && c != L'J') {
            formattedKey += c;
        }
    }

    std::vector<std::vector<wchar_t>> matrix(5, std::vector<wchar_t>(5));
    std::wstring alphabet = L"ABCDEFGHIKLMNOPQRSTUVWXYZ";
    size_t keyIndex = 0, alphaIndex = 0;

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            if (keyIndex < formattedKey.length()) {
                matrix[row][col] = formattedKey[keyIndex++];
            }
            else {
                while (formattedKey.find(alphabet[alphaIndex]) != std::wstring::npos) {
                    alphaIndex++;
                }
                matrix[row][col] = alphabet[alphaIndex++];
            }
        }
    }
    return matrix;
}

std::wstring EncryptPlayfair(const std::wstring& formattedText, const std::vector<std::vector<wchar_t>>& matrix) {
    std::wstring result;

    for (size_t i = 0; i < formattedText.length(); i += 2) {
        wchar_t a = formattedText[i];
        wchar_t b = formattedText[i + 1];

        int rowA, colA, rowB, colB;

        // Find positions of the letters in the matrix
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 5; c++) {
                if (matrix[r][c] == a) {
                    rowA = r; colA = c;
                }
                if (matrix[r][c] == b) {
                    rowB = r; colB = c;
                }
            }
        }

        // Encryption rules
        if (rowA == rowB) {
            result += matrix[rowA][(colA + 1) % 5];
            result += matrix[rowB][(colB + 1) % 5];
        }
        else if (colA == colB) {
            result += matrix[(rowA + 1) % 5][colA];
            result += matrix[(rowB + 1) % 5][colB];
        }
        else {
            result += matrix[rowA][colB];
            result += matrix[rowB][colA];
        }
    }

    return result;
}

std::wstring PlayfairEncrypt(const std::wstring& text, const std::wstring& key) {
    std::wstring formattedText = FormatTextForPlayfair(text);
    std::vector<std::vector<wchar_t>> matrix = CreatePlayfairMatrix(key);
    return EncryptPlayfair(formattedText, matrix);
}

std::wstring DecryptPlayfair(const std::wstring& formattedText, const std::vector<std::vector<wchar_t>>& matrix) {
    std::wstring result;

    for (size_t i = 0; i < formattedText.length(); i += 2) {
        wchar_t a = formattedText[i];
        wchar_t b = formattedText[i + 1];

        int rowA, colA, rowB, colB;

        // Find positions of the letters in the matrix
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 5; c++) {
                if (matrix[r][c] == a) {
                    rowA = r; colA = c;
                }
                if (matrix[r][c] == b) {
                    rowB = r; colB = c;
                }
            }
        }

        // Decryption rules
        if (rowA == rowB) {
            result += matrix[rowA][(colA + 4) % 5];
            result += matrix[rowB][(colB + 4) % 5];
        }
        else if (colA == colB) {
            result += matrix[(rowA + 4) % 5][colA];
            result += matrix[(rowB + 4) % 5][colB];
        }
        else {
            result += matrix[rowA][colB];
            result += matrix[rowB][colA];
        }
    }

    return result;
}

std::wstring PlayfairDecipher(const std::wstring& text, const std::wstring& key) {
    std::wstring formattedText = FormatTextForPlayfair(text);
    std::vector<std::vector<wchar_t>> matrix = CreatePlayfairMatrix(key);
    return DecryptPlayfair(formattedText, matrix);
}

// New Hill Cipher functions
std::vector<std::vector<int>> GetKeyMatrix(const std::wstring& key) {
    int keySize = std::sqrt(key.length());
    std::vector<std::vector<int>> keyMatrix(keySize, std::vector<int>(keySize));
    int k = 0;
    for (int i = 0; i < keySize; i++) {
        for (int j = 0; j < keySize; j++) {
            keyMatrix[i][j] = (toupper(key[k]) - 'A') % 26;
            k++;
        }
    }
    return keyMatrix;
}

int ModInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++)
        if ((a * x) % m == 1)
            return x;
    return 1;
}

int Determinant(const std::vector<std::vector<int>>& matrix) {
    if (matrix.size() == 2) {
        return (matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0] + 26) % 26;
    }
    // For larger matrices, you'd need to implement a more general determinant calculation
    return 0;
}

std::vector<std::vector<int>> InverseMatrix(const std::vector<std::vector<int>>& matrix) {
    int det = Determinant(matrix);
    int detInv = ModInverse(det, 26);
    std::vector<std::vector<int>> inverse(2, std::vector<int>(2));
    inverse[0][0] = (matrix[1][1] * detInv) % 26;
    inverse[0][1] = ((26 - matrix[0][1]) * detInv) % 26;
    inverse[1][0] = ((26 - matrix[1][0]) * detInv) % 26;
    inverse[1][1] = (matrix[0][0] * detInv) % 26;
    return inverse;
}

std::wstring HillCipher(const std::wstring& text, const std::wstring& key, bool encrypt) {
    std::vector<std::vector<int>> keyMatrix = GetKeyMatrix(key);
    int keySize = keyMatrix.size();
    std::wstring result;
    std::vector<std::vector<int>> inverseKeyMatrix;

    if (!encrypt) {
        inverseKeyMatrix = InverseMatrix(keyMatrix);
    }

    for (size_t i = 0; i < text.length(); i += keySize) {
        std::vector<int> block(keySize);
        for (int j = 0; j < keySize; j++) {
            if (i + j < text.length()) {
                block[j] = toupper(text[i + j]) - 'A';
            }
            else {
                block[j] = 0; // Pad with 'A' if necessary
            }
        }

        for (int j = 0; j < keySize; j++) {
            int sum = 0;
            for (int k = 0; k < keySize; k++) {
                if (encrypt) {
                    sum += block[k] * keyMatrix[k][j];
                }
                else {
                    sum += block[k] * inverseKeyMatrix[k][j];
                }
            }
            result += (sum % 26 + 'A');
        }
    }

    return result;
}

std::wstring HillDecipher(const std::wstring& text, const std::wstring& key) {
    return HillCipher(text, key, false);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    hInst = hInstance;
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc,
        0, 0, GetModuleHandle(NULL), NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, L"MainWindow", NULL };

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, L"Encryption/Decryption Tool",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, wc.hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hTextBox, hKeyBox, hResultVigenere, hVigenereRadio, hPlayfairRadio, hHillRadio, hEncrypt, hDecrypt;

    switch (msg) {
    case WM_CREATE:
        CreateWindow(L"STATIC", L"Enter Text:", WS_VISIBLE | WS_CHILD,
            20, 20, 100, 20, hwnd, NULL, hInst, NULL);

        hTextBox = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            20, 50, 200, 20, hwnd, (HMENU)IDC_TEXTBOX, hInst, NULL);

        CreateWindow(L"STATIC", L"Enter Key:", WS_VISIBLE | WS_CHILD,
            20, 90, 100, 20, hwnd, NULL, hInst, NULL);

        hKeyBox = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            20, 120, 200, 20, hwnd, (HMENU)IDC_KEYBOX, hInst, NULL);

        CreateWindow(L"STATIC", L"Choose Cipher:", WS_VISIBLE | WS_CHILD,
            20, 160, 100, 20, hwnd, NULL, hInst, NULL);

        hVigenereRadio = CreateWindow(L"BUTTON", L"Vigenere", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            20, 190, 100, 25, hwnd, (HMENU)IDC_VIGENERE_RADIO, hInst, NULL);
        SendMessage(hVigenereRadio, BM_SETCHECK, BST_CHECKED, 0);

        hPlayfairRadio = CreateWindow(L"BUTTON", L"Playfair", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            120, 190, 100, 25, hwnd, (HMENU)IDC_PLAYFAIR_RADIO, hInst, NULL);

        hHillRadio = CreateWindow(L"BUTTON", L"Hill", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            220, 190, 100, 25, hwnd, (HMENU)IDC_HILL_RADIO, hInst, NULL);

        hEncrypt = CreateWindow(L"BUTTON", L"Encrypt", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            20, 230, 120, 25, hwnd, (HMENU)IDC_ENCRYPT, hInst, NULL);
        SendMessage(hEncrypt, BM_SETCHECK, BST_CHECKED, 0);

        hDecrypt = CreateWindow(L"BUTTON", L"Decrypt", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            150, 230, 120, 25, hwnd, (HMENU)IDC_DECRYPT, hInst, NULL);

        CreateWindow(L"BUTTON", L"Process", WS_VISIBLE | WS_CHILD,
            20, 270, 100, 25, hwnd, (HMENU)IDC_PROCESS, hInst, NULL);

        hResultVigenere = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY,
            20, 310, 300, 60, hwnd, (HMENU)IDC_RESULT_VIGENERE, hInst, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_PROCESS) {
            wchar_t text[260], key[260];
            GetWindowText(hTextBox, text, 260);
            GetWindowText(hKeyBox, key, 260);

            std::wstring result;
            bool isEncrypt = SendMessage(hEncrypt, BM_GETCHECK, 0, 0) == BST_CHECKED;

            if (SendMessage(hVigenereRadio, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                if (isEncrypt) {
                    result = VigenereCipher(text, key);
                }
                else {
                    result = VigenereDecipher(text, key);
                }
            }
            else if (SendMessage(hPlayfairRadio, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                if (isEncrypt) {
                    result = PlayfairEncrypt(text, key);
                }
                else {
                    result = PlayfairDecipher(text, key);
                }
            }
            else if (SendMessage(hHillRadio, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                if (isEncrypt) {
                    result = HillCipher(text, key, true);
                }
                else {
                    result = HillDecipher(text, key);
                }
            }

            SetWindowText(hResultVigenere, result.c_str());
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}