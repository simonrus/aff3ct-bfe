clear all
load('result.mat')

theoryBer = 0.5*erfc(sqrt(10.^(ebn0/10)))
% plot
close all
figure
semilogy(ebn0,theoryBer,'b.-');
hold on
semilogy(ebn0,BER,'mx-');
axis([-3 10 10^-5 0.5])
grid on
legend('theory', 'simulation');
xlabel('Eb/No, dB');
ylabel('Bit Error Rate');
title('Bit error probability curve for BPSK modulation');

