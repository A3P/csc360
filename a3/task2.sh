!/bin/bash
# The double for loop is responsible for simulating the cpu proccessing 
# for various dispatch and quantum values. At each instance the output is sent to
# a python script for calculation of it's average wait and turnaround time.
# The python script then stores the data into a text file for later use by the R
# programming language.
for quantum in 500 250 100 50 
do
    dispatch=0
    for i in {1..6}
    do
        ./simgen 1200 4456 \
        | ./rrsim --quantum $quantum --dispatch $dispatch \
        | python3 simreader.py $dispatch
        
        dispatch=$((dispatch+5))
    done
done

# R programming language is used to read the data recorded by the python script as 
# matrices and the matplot command graphs every row of it.
# A color palette is used in R for consistent coloring between the two graphs.
# The commands placed between the png() and dev.off() draw the graph before
# the png file is saved and closed by dev.off().
R --no-save <<EOF

dispatch <- c(0,5,10,15,20,25)
quantum <- c(500,250,100,50)

input <- read.table("./stat/w.txt")
palette = rainbow(nrow(input))

png(filename="./stat/w.png")

matplot(dispatch, t(input),
type='l',lty=1,
xlab="Dispatch overhead",ylab="Average waiting time",
main="Round Robin scheduler -- # tasks: 1200; seed value:4456", col=palette)

legend(legend = quantum, x = "top", y = "top", lty = 1, lwd = 2, col = palette)
matpoints(dispatch, t(input), pch = "*", col = palette)
dev.off()

input <- read.table("./stat/ta.txt")
png(filename="./stat/ta.png")

matplot(dispatch, t(input),
type='l',lty=1,
xlab="Dispatch overhead",ylab="Average turnaround time",
main="Round Robin scheduler -- # tasks: 1200; seed value:4456", col=palette)

legend(legend = quantum, x = "top", y = "top", lty = 1, lwd = 2, col = palette)
matpoints(dispatch, t(input), pch = "*", col = palette)
dev.off()
EOF
