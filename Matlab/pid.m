MAX_INT = 10;
SCALER = 10;
P=5
max_error = MAX_INT / (P+1) * SCALER;

target_pos = 10; 
pos = 30;
error = (target_pos - pos) * SCALER;
pterm = P * error;

round(pterm/SCALER)
