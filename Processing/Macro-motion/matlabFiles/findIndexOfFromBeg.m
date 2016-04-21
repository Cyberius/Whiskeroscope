function index = findIndexOfFromBeg(signal,value)
eps = 0.01;
zero_force =value;
for i =1:size(signal,2)
  if(abs(signal(1,i)-zero_force) < eps)
       index = i;  
  end
end
end