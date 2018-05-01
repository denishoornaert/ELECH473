## Results

The first table contains the results obtained by using the "delay computation" template suggested for the lab 7 (method working for both Windows and Linux).
On the other hand, the second table contains the results of the "delay computation" using the template suggested in the assignment statement of the lab 8 (method working only for Windows).

In addition, by comparing the tables, we found out that the second method seems to be be more precise than the first one.

# GCC
| MODE    | DIM | C        |          |          | ASM      |          |          |
|:-------:|-----|----------|----------|----------|----------|----------|----------|
|         |     | min      | max      | avg      | min      | max      | avg      |
| DEBUG   | 3x3 | 0.078000 | 0.110000 | 0.104570 | 0.000000 | 0.016000 | 0.000363 |
|         | 5x5 | 0.141000 | 0.172000 | 0.158446 | 0.000000 | 0.016000 | 0.002130 |
|         | 7x7 | 0.140000 | 0.281000 | 0.242227 | 0.000000 | 0.016000 | 0.002957 |
| RELEASE | 3x3 | 0.000000 | 0.031000 | 0.015497 | 0.000000 | 0.016000 | 0.000520 |
|         | 5x5 | 0.015000 | 0.032000 | 0.025640 | 0.015000 | 0.032000 | 0.025640 |
|         | 7x7 | 0.016000 | 0.047000 | 0.036810 | 0.000000 | 0.016000 | 0.002543 |

# Windows
| MODE    | DIM | C        |          |          | ASM      |          |          |
|:-------:|-----|----------|----------|----------|----------|----------|----------|
|         |     | min      | max      | avg      | min      | max      | avg      |
| DEBUG   | 3x3 | 0.101955 | 0.108967 | 0.104331 | 0.000600 | 0.000734 | 0.000652 |
|         | 5x5 | 0.155106 | 0.180117 | 0.159231 | 0.000987 | 0.001166 | 0.001032 |
|         | 7x7 | 0.225472 | 0.257760 | 0.232984 | 0.001733 | 0.001964 | 0.001788 |
| RELEASE | 3x3 | 0.014912 | 0.016357 | 0.015326 | 0.000598 | 0.000861 | 0.000652 |
|         | 5x5 | 0.025135 | 0.028877 | 0.025842 | 0.000989 | 0.001204 | 0.001050 |
|         | 7x7 | 0.035268 | 0.040642 | 0.036502 | 0.001733 | 0.001988 | 0.001795 |
