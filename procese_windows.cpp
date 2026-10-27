
#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc == 2) {
        int secunde = atoi(argv[1]);

        cout << "Procesul " << secunde
             << " asteapta " << secunde
             << " secunde." << endl;

        Sleep(secunde * 1000);

        cout << "Procesul " << secunde
             << " a terminat." << endl;
        return 0;
    }

    HANDLE procese[3];

    char cale[MAX_PATH];
    GetModuleFileNameA(NULL, cale, MAX_PATH);

    for (int i = 0; i < 3; i++) {
        string comanda = "\"" + string(cale) +
                         "\" " + to_string(i + 1);

        STARTUPINFOA si = {};
        PROCESS_INFORMATION pi = {};
        si.cb = sizeof(si);

        if (!CreateProcessA(NULL, &comanda[0],
                            NULL, NULL, FALSE, 0,
                            NULL, NULL, &si, &pi)) {
            cout << "Eroare la creare proces!" << endl;
            for (int j = 0; j < i; j++) {
                WaitForSingleObject(procese[j], INFINITE);
                CloseHandle(procese[j]);
            }
            return 1;
        }

        procese[i] = pi.hProcess;
        CloseHandle(pi.hThread);
    }

    WaitForMultipleObjects(3, procese, TRUE, INFINITE);

    for (int i = 0; i < 3; i++) {
        CloseHandle(procese[i]);
    }

    cout << "Toate procesele au terminat!" << endl;
    return 0;
}
