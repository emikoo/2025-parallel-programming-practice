# Problem 2: Sort an Array

This task is worth 40 points.

In this problem, you need to implement a program that will sort an array and will output some of the elements of the sorted array. The array to sort is given on the first line in an implicit form: you're given the number of elements in the arrray and then numbers A, B, and P. The array is defined in the following way:

```
array[0] = A mod P
array[i] = (B + array[i - 1] * A) mod P
```

On the second line there's a number K, and the program should output each Kth number from the sorted array.

You're already given an example solution that doesn't yet pass tests. Your task is to speed up this solution using parallelism.

Note, that you could also implement a more optimal serial solution, but it's not the goal of this task, so a more optimal serial solution won't be accepted even if it passes the tests.
