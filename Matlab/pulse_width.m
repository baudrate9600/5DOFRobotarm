angle = 90;
step_to_angle = 1/0.043182;
num_steps =angle*step_to_angle
duration = 10;
acceleration = 5;
vmax= round(0.5*(acceleration*duration-sqrt((acceleration*duration)^2-(angle*acceleration*4))));
t0 = round((vmax^2 * num_steps) / (2 * angle * acceleration));
t1 = num_steps - t0;
c0 = 10000 * sqrt(2/(acceleration*step_to_angle));
pw = c0;
apw = 0;

total_time = c0/10000; 
for c = 1:t0-1
    pw = pw - (2*pw)/(4*c+1);
    disp(pw);
   % total_time = total_time + pw/10000;
    %apw=apw+c0*( sqrt(c+1)-sqrt(c) )/10000;
end

total_time = total_time * 2; 
total_time = total_time + pw*(t1-t0)/10000;

total_time = 2*apw;
total_time = total_time + apw*(t1-t0)/10000;


