#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

void matrix_mul_1d(long **A, long *X, long *Y, int n);
void matrix_mul_2d(long **A, long *X, long *Y, int n);

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    
    int T = 16;
    omp_set_num_threads(T);

    vector<int> sizes = {1000, 5000, 10000, 20000, 30000, 40000};
    string filename = "1D_2D_Open_MP_" + to_string(T) + "threads.txt";
    ofstream fout(filename);

    fout << "N\t1D_Time(s)\t2D_Time(s)\n";

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

        double start_time, end_time;

        fill(Y, Y + N, 0);
        start_time = omp_get_wtime();
        matrix_mul_1d(A, X, Y, N);
        end_time = omp_get_wtime();
        double t1 = end_time - start_time;

        fill(Y, Y + N, 0);
        start_time = omp_get_wtime();
        matrix_mul_2d(A, X, Y, N);
        end_time = omp_get_wtime();
        double t2 = end_time - start_time;

        fout << N << "\t" << fixed << setprecision(8) << t1 << "\t" << t2 << "\n";

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

void matrix_mul_1d(long **A, long *X, long *Y, int n)
{
#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        int numThreads = omp_get_num_threads();

        int offset = n / numThreads;
        if (threadID < n % numThreads)
            ++offset;

        int start = threadID * (n / numThreads);
        if (threadID < n % numThreads)
            start += threadID;
        else
            start += n % numThreads;

        int end = start + offset;
        for (int i = start; i < end; ++i)
        {
            Y[i] = 0;
            for (int k = 0; k < n; ++k)
                Y[i] += A[i][k] * X[k];
        }
    }
}

void matrix_mul_2d(long **A, long *X, long *Y, int n)
{
    int numThreads = omp_get_max_threads();
    int gridSize = sqrt(numThreads);
    int blockSize = n / gridSize + (n % gridSize != 0);

#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        int rowBlock = threadID / gridSize;
        int colBlock = threadID % gridSize;

        int rowStart = rowBlock * blockSize;
        int rowEnd = min((rowBlock + 1) * blockSize, n);
        int colStart = colBlock * blockSize;
        int colEnd = min((colBlock + 1) * blockSize, n);

        for (int i = rowStart; i < rowEnd; ++i)
        {
            long temp = 0;
            for (int k = colStart; k < colEnd; ++k)
                temp += A[i][k] * X[k];
#pragma omp atomic
            Y[i] += temp;
        }
    }
}
