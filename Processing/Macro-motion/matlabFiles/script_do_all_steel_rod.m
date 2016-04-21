signal1 = steel_rod; %thin
signal2 = steel_rod_with_tape; %medium

stem1 = steel_rod;
stem2 = steel_rod_with_tape;

[sig1_y1,sig1_y2] = getAvgAroundPeak(signal1);
[sig2_y1,sig2_y2] = getAvgAroundPeak(signal2);


value =0.05;
index_sig1_y1 = 4;%findIndexOfFromBeg(sig1_y1,value)
index_sig2_y1 = 1;%findIndexOfFromBeg(sig2_y1,value)


figure;
num_points_to_plot =20;
plot(sig1_y1(1,index_sig1_y1:index_sig1_y1+num_points_to_plot),'b'); hold on;
plot(sig2_y1(1,index_sig2_y1:index_sig2_y1+num_points_to_plot),'r');

grid on;
legend('steel rod','steel rod with tape')

value =0.01;
sig1_y2 = fliplr(sig1_y2);
sig2_y2 = fliplr(sig2_y2);

index_sig1_y2 = 1%findIndexOfFromBeg(sig1_y2,value)
index_sig2_y2 = 4%findIndexOfFromBeg(sig2_y2,value)


figure;
num_points_to_plot =20;
plot(sig1_y2(1,index_sig1_y2:index_sig1_y2+num_points_to_plot),'b'); hold on;
plot(sig2_y2(1,index_sig2_y2:index_sig2_y2+num_points_to_plot),'r');

grid on;
legend('steel rod','steel rod with tape')