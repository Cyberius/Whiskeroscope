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
P = (P-min(P))/(max(P)-min(P));
figure;
plot(time,P,'r');hold on;

P1 =findpeaksplot(time,P,0.004,-10,3,3,1); %max
P2 =findminpeaksplot(time,P,-0.004,-10,3,3,1); %min
n = min(size(P1,1),size(P2,1));
eps = 0.007;
P1_index_time_aligned = [];
P2_index_time_aligned = [];
for i =1:n
time_diff = P1(i,2) - P2(i,2);
    if(abs(time_diff) < eps)
    P1_index_time_aligned =[P1_index_time_aligned;i];
    P2_index_time_aligned =[P2_index_time_aligned;i];
    else
        if(time_diff < 0)
        P2 = [P2(1:i,:) ; P2(i,:); P2(i+1:end,:)];
        else
        P1 = [P1(1:i,:) ; P1(i,:); P1(i+1:end,:)];
        end
    end 
end

pk2pk =[];
P1_mag = P1(:,3);
P2_mag = P2(:,3);
m = min(size(P1_index_time_aligned,1),size(P2_index_time_aligned,1));
pk2pk = abs(P1_mag(P1_index_time_aligned(1:m))- P2_mag(P2_index_time_aligned(1:m)));
figure;
plot(P1_mag(P1_index_time_aligned(1:m)),'k');hold on; plot(P1_mag(P1_index_time_aligned(1:m)),'kx');
plot(P2_mag(P2_index_time_aligned(1:m)),'r');hold on; plot(P2_mag(P2_index_time_aligned(1:m)),'rx');
plot(pk2pk)

figure;
plot(1-P1_mag(P1_index_time_aligned(1:m)),pk2pk)