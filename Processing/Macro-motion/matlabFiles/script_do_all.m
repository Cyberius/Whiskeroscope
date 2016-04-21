signal1 = stem1; %thin
signal2 = stem2; %medium
signal3 = stem3;  %thick

[sig1_y1,sig1_y2] = getAvgAroundPeak(signal1);
[sig2_y1,sig2_y2] = getAvgAroundPeak(signal2);
[sig3_y1,sig3_y2] = getAvgAroundPeak(signal3);

value =0.01;
index_sig1_y1 = findIndexOfFromBeg(sig1_y1,value)
index_sig2_y1 = findIndexOfFromBeg(sig2_y1,value)
index_sig3_y1 = findIndexOfFromBeg(sig3_y1,value)

figure;
num_points_to_plot =15;
plot(sig1_y1(1,index_sig1_y1:index_sig1_y1+num_points_to_plot),'b'); hold on;
plot(sig2_y1(1,index_sig2_y1:index_sig2_y1+num_points_to_plot),'r');
plot(sig3_y1(1,index_sig3_y1:index_sig3_y1+num_points_to_plot),'k');
grid on;
legend('stem 4','stem 5','stem 6')
 xlabel('motor rotation step')
 ylabel('Force between whisker and stem (scaled)');

value =0.15;
sig1_y2 = fliplr(sig1_y2);
sig2_y2 = fliplr(sig2_y2);
sig3_y2 = fliplr(sig3_y2);
index_sig1_y2 = findIndexOfFromBeg(sig1_y2,value)
index_sig2_y2 = findIndexOfFromBeg(sig2_y2,value)
index_sig3_y2 = findIndexOfFromBeg(sig3_y2,value)

figure;
num_points_to_plot =40;
plot(sig1_y2(1,index_sig1_y2:index_sig1_y2+num_points_to_plot),'b'); hold on;
plot(sig2_y2(1,index_sig2_y2:index_sig2_y2+num_points_to_plot),'r');
plot(sig3_y2(1,index_sig3_y2:index_sig3_y2+num_points_to_plot),'k');
grid on;
legend('stem 4','stem 5','stem 6')