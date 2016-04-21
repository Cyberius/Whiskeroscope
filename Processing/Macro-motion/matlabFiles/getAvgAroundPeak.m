function [y1,y2] = getAvgAroundPeak(input_signal)

data = input_signal;
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
%filtering to remove high pass frequencies
lowPassfreq = 0.07;
filterLength =7;
b = lowPassfreq*sinc(lowPassfreq*(-filterLength:filterLength));
P = filter(b,1,P);
%done filtering 
plot(time,P,'g');
curr_min_index=[];
eps = 0.02;
max_force =max(P);
prev_min_index =1;
for i =1:size(P,1)
  if(abs(P(i,1)-max_force) < eps)
    if(time(i,1)-time(prev_min_index,1)>2.5)
      curr_min_index = [curr_min_index; i];
     prev_min_index = i;  
    end
  end
 
end
%hold on;
%plot(time(curr_min_index,1),P(curr_min_index,1),'kx');
%figure;
numPastPoints =50;
avg = zeros(1,2*numPastPoints+1);
for i = 1:size(curr_min_index,1)
  for j = curr_min_index(i,1)-numPastPoints:curr_min_index(i,1)+numPastPoints
  %plot((j-curr_min_index(i,1)),P(j),'b');hold on;
  avg(1,j-curr_min_index(i,1)+numPastPoints+1) =  avg(1,j-curr_min_index(i,1)+numPastPoints+1)+P(j);
  end
end

avg(1,:) =avg(1,:)./(size(curr_min_index,1));
avg(1,:) = -1*(avg(1,3)) + avg(1,:);
%avg_shifted(1,:) =[0,0,avg(1,:)];
%plot(avg_shifted(1,:),'r');
%plot(avg(1,:),'bx');
%grid on

y1 = avg(1,1:numPastPoints);
y2 = avg(1,numPastPoints+1:end-1);

end
