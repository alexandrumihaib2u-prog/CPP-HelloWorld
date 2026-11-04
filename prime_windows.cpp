
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

bool estePrim(int n) {
    if (n < 2) return false;

    for (int d = 2; d * d <= n; d++) {
        if (n % d == 0) return false;
    }

    return true;
}

int main(int argc, char* argv[]) {

    // Codul executat de fiecare proces copil
    if (argc > 1 && string(argv[1]) == "copil") {
        int inceput, sfarsit;

        if (!(cin >> inceput >> sfarsit))
            return 1;

        for (int n = inceput; n <= sfarsit; n++) {
            if (estePrim(n)) {
                cout << n << " ";
            }
        }

        cout << endl;
        return 0;
    }

    // Codul procesului principal
    const int N = 10;
    HANDLE procese[N];
    HANDLE rezultate[N];

    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    char cale[MAX_PATH];
    GetModuleFileNameA(NULL, cale, MAX_PATH);

    for (int i = 0; i < N; i++) {
        HANDLE intrareCitire, intrareScriere;
        HANDLE iesireCitire, iesireScriere;

        if (!CreatePipe(&intrareCitire, &intrareScriere,
                        &sa, 0) ||
            !CreatePipe(&iesireCitire, &iesireScriere,
                        &sa, 0)) {
            cerr << "Eroare la crearea pipe-urilor!" << endl;
            return 1;
        }

        SetHandleInformation(intrareScriere,
                             HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(iesireCitire,
                             HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si = {};
        PROCESS_INFORMATION pi = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = intrareCitire;
        si.hStdOutput = iesireScriere;
        si.hStdError = iesireScriere;

        string comanda = "\"" + string(cale) + "\" copil";

        BOOL creat = CreateProcessA(
            NULL, &comanda[0],
            NULL, NULL, TRUE,
            CREATE_NO_WINDOW,
            NULL, NULL, &si, &pi
        );

        CloseHandle(intrareCitire);
        CloseHandle(iesireScriere);

        if (!creat) {
            cerr << "Eroare la crearea procesului!" << endl;
            CloseHandle(intrareScriere);
            CloseHandle(iesireCitire);
            return 1;
        }

        procese[i] = pi.hProcess;
        rezultate[i] = iesireCitire;
        CloseHandle(pi.hThread);

        int start = i * 1000 + 1;
        int stop = (i + 1) * 1000;

        string mesaj = to_string(start) + " " +
                       to_string(stop) + "\n";

        DWORD scris;
        WriteFile(intrareScriere,
                  mesaj.c_str(),
                  (DWORD)mesaj.size(),
                  &scris, NULL);

        CloseHandle(intrareScriere);
    }

    int total = 0;

    // Procesul principal primeste numerele prime
    for (int i = 0; i < N; i++) {
        string raspuns;
        char buffer[512];
        DWORD citit;

        while (ReadFile(rezultate[i], buffer,
                        sizeof(buffer), &citit, NULL)
               && citit > 0) {
            raspuns.append(buffer, citit);
        }

        CloseHandle(rezultate[i]);

        istringstream date(raspuns);
        int numar;
        int count = 0;

        cout << "\nProcesul " << i + 1
             << " - intervalul "
             << i * 1000 + 1 << "-"
             << (i + 1) * 1000 << ":\n";

        while (date >> numar) {
            cout << numar << " ";
            count++;
            total++;

            if (count % 20 == 0)
                cout << endl;
        }

        cout << "\nNumere prime gasite: "
             << count << endl;
    }

    WaitForMultipleObjects(N, procese, TRUE, INFINITE);

    for (int i = 0; i < N; i++) {
        CloseHandle(procese[i]);
    }

    cout << "\nTotal numere prime: " << total << endl;
    cout << "Toate cele 10 procese au terminat!" << endl;

    return 0;
}
