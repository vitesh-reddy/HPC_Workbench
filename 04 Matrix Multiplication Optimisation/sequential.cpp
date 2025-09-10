#include <bits/stdc++.h>
using namespace std;

void matrix_mul(long **A, long *X, long *Y, int n)
{
    for (int i = 0; i < n; ++i)
    {
        Y[i] = 0;
        for (int k = 0; k < n; ++k)
            Y[i] += A[i][k] * X[k];
    }
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    vector<int> sizes = {1000, 5000, 10000, 20000, 30000, 40000};
    ofstream fout("Sequential.txt");
    fout << "N\tTime(s)\n";

    for (int N : sizes)
    {
        int numA = 1, numX = 1;
        long **A = new long *[N];
        long *X = new long[N], *Y = new long[N];
        for (int i = 0; i < N; ++i)
        {
            A[i] = new long[N];
            for (int j = 0; j < N; ++j)
                A[i][j] = numA++;
            X[i] = numX++;
            Y[i] = 0;        
        }

        auto start = chrono::high_resolution_clock::now();
        matrix_mul(A, X, Y, N);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        fout << N << "\t" << fixed << setprecision(8) << elapsed.count() << "\n";

        for (int i = 0; i < N; ++i)
            delete[] A[i];
        delete[] A;
        delete[] X;
        delete[] Y;
        cout << N << " completed" << endl;
    }

    fout.close();
    return 0;
}
