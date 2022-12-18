dt = 0.01;
t = 0:dt:3;
v0 = 2;
accel = -2;
y0 = 0.0;
v = 0*t + v0 + cumsum(ones(size(t))*dt * accel);
yReal = y0 + cumsum(dt * v);

yExact = y0 + v0 .* t + 0.5*accel*t.^2;
% dy / dt = v0 + accel*t
tPeak = -v0/accel;
peak = y0 + v0 * tPeak + 0.5*accel*tPeak^2;

close all;
figure(); hold on;
plot(t, yReal, 'b');
plot(t, yExact, 'k')
plot(tPeak, peak, 'x');
legend('finite step', 'equation', 'apex')