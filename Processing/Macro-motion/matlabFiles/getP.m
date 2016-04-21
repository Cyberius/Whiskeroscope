data = dataSteel2;
E =1;
I =1;
x2 = 3;
x1 = 2;
tan_theta1 = data(:,3);
tan_theta2 = data(:,2);
time = data(:,1);
ChristieP = data(:,4);
k = tan_theta2./tan_theta1;
P = 1*2*E*I*((tan_theta2./x2 - tan_theta1./x1)./(x2-x1));
figure;
plot(tan_theta1,'x');title('sensors'); hold on;plot(tan_theta2,'rx');
figure;
plot(P,'x'); title('Force');hold on;plot(ChristieP,'rx');
%figure
d =(k*(x1^2) - x2^2)./(2*(k*x1-x2));
%plot(d);title('dist from contact');