CSC 360 A3, Task 2 Description
=====================

The steps below provide a quick summary of how
the graphs were generated for task 2 of Assignment 3. However, there are further clarifications on some of the techincial aspects of the code provided within the comments of the scripts.


1. A shell script named task2.sh is used to iterate through the combinations of quantums and dispatch values.

2. In each iteration simgen is ran to provide input for rrsim.

3. After each simulation, the output is provided to simreader.py. simreader uses regexp to find and sum all the wait and turnaround times.

4. When all the output of a simulations is parsed, then simreader.py appends the averages to a text file for later use.

5. After enough iterations of the steps above, statistical programming language R is invoked by task2.sh to read and graph the data written by simreader.py.

6. R plots each row of the text file and saves the graph to a png file which is later converted to a pdf.

Files submitted
------------------

* rrsim.c
* graph_turnaround.pdf
* graph_waiting.pdf
* task2.md
* task2.sh
* simreader.py
* w.txt and ta.txt, created by simreader.py and used by R for plotting the graphs.
    