data = NT_70;
E =1;
I =1;
x2 = 3;
x1 = 2;
tan_theta1 = data(:,3);
tan_theta2 = data(:,2);
time = data(:,1);
ChristieP = data(:,4);
k = tan_theta2./tan_theta1;
P = -1*2*E*I*((tan_theta2./x2 - tan_theta1./x1)./(x2-x1));
figure;
plot(time,P,'r');hold on;
