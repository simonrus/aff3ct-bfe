clear all
load('result.mat');

theoryBer = 0.5*erfc(sqrt(10.^(ebn0/10)));

aff3ct_ebn0 = [0 1 2 3 4];
aff3ct_BER = [1.43e-01, 9.87e-02,5.63e-02, 2.48e-02 7.13e-03];

% plot
close all
figure
semilogy(ebn0,theoryBer,'b.-');
hold on
semilogy(ebn0,BER,'mx-');
hold on
semilogy(aff3ct_ebn0,aff3ct_BER,'g.-');

axis([-3 10 10^-5 0.5])
grid on
legend('theory', 'simulation', 'aff3ct');
xlabel('Eb/No, dB');
ylabel('Bit Error Rate');
title('Bit error probability curve for BPSK modulation');

