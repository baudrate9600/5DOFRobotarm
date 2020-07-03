#Communication 
The host sends a data frame which the microcontroller will parse and 
do what it is commanded 

##Data frame 
M | #motor | sign | n0 | n1 | n2 | 

start with sending a M, then the motor number, then the sign of the angle.
The angle is between [0,999]. so for instance 
M 1 - 1 3 0 
