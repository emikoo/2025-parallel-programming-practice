# Problem 1: Multiply Two Matrices

This task is worth 35 points.

In this problem, you need to implement a program that will accept two matrices and will compute a product of those two matrices. Input matrices are given to the program in an indirect form: on the first line, the number of rows and columns are given. On the second line, numbers A, B, X, Y, Z, and P are given. The matrix is then defined in the following way:

```
matrix'[i][j] = A mod P
matrix'[i][j] = (B + matrix'[i - 1][j] * X + matrix'[i][j - 1] * Y + matrix'[i - 1][j - 1] * Z) mod P
matrix[i][j] = matrix'[i][j] / max(matrix')
```

You're already given an example solution that doesn't yet pass tests. Your task is to speed up this solution using parallelism.

Note, that you could also implement a more optimal serial solution, but it's not the goal of this task, so a more optimal serial solution won't be accepted even if it passes the tests.
