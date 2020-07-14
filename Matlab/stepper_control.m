serial = serialport("COM4",9600)



str = "M1" 
num = 07;


accel = 10;
duration = 10; 
angle = 30; 
sign = "+";

discriminant = sqrt((accel*duration)^2-4*accel * angle) * 0.5
if(angle <0 )
    sign = "-";
end;
if isreal(discriminant)
   str = str + num2str(duration,'%02.f') + num2str(accel, '%02.f') + ...
         sign + num2str(angle, '%03.f')
   %str = reverse(str)
        
else 
    
    disp("invalid");
    
end 
    
pause(2);
if serial.NumBytesAvailable > 0 
    read(serial,serial.NumBytesAvailable, 'CHAR')
end 
write(serial,str,'char');
clear serial;
