
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

bool estePrim(int n) {
    if (n < 2) return false;

    for (int d = 2; d * d <= n; d++) {
        if (n % d == 0) return false;
    }

    return true;
}

int main() {
    const int N = 10;
    pid_t procese[N];
    int rezultate[N];

    for (int i = 0; i < N; i++) {
        int spreCopil[2];
        int spreParinte[2];

        if (pipe(spreCopil) == -1 ||
            pipe(spreParinte) == -1) {
            cerr << "Eroare la pipe!" << endl;
            return 1;
        }

        pid_t pid = fork();

        if (pid < 0) {
            cerr << "Eroare la fork!" << endl;
            return 1;
        }

        if (pid == 0) {
            // Proces copil
            close(spreCopil[1]);
            close(spreParinte[0]);

            int interval[2];

            if (read(spreCopil[0], interval,
                     sizeof(interval)) != sizeof(interval)) {
                _exit(1);
            }

            close(spreCopil[0]);

            for (int n = interval[0];
                 n <= interval[1]; n++) {

                if (estePrim(n)) {
                    if (write(spreParinte[1], &n,
                              sizeof(n)) != sizeof(n)) {
                        _exit(1);
                    }
                }
            }

            close(spreParinte[1]);
            _exit(0);
        }

        // Proces parinte
        procese[i] = pid;
        close(spreCopil[0]);
        close(spreParinte[1]);

        rezultate[i] = spreParinte[0];

        int interval[2] = {
            i * 1000 + 1,
            (i + 1) * 1000
        };

        write(spreCopil[1], interval, sizeof(interval));
        close(spreCopil[1]);
    }

    int total = 0;

    for (int i = 0; i < N; i++) {
        cout << "\nProcesul " << i + 1
             << " - intervalul "
             << i * 1000 + 1 << "-"
             << (i + 1) * 1000 << ":\n";

        int numar;
        int count = 0;
        ssize_t citit;

        while ((citit = read(rezultate[i],
                 &numar, sizeof(numar))) > 0) {

            if (citit != sizeof(numar)) {
                cerr << "Eroare la citirea pipe!" << endl;
                return 1;
            }

            cout << numar << " ";
            count++;
            total++;

            if (count % 20 == 0)
                cout << endl;
        }

        close(rezultate[i]);

        cout << "\nNumere prime gasite: "
             << count << endl;
    }

    for (int i = 0; i < N; i++) {
        waitpid(procese[i], nullptr, 0);
    }

    cout << "\nTotal numere prime: "
         << total << endl;

    cout << "Toate cele 10 procese au terminat!"
         << endl;

    return 0;
}
