function [ ox,oy ] = getOrigin( xA,yA )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    min = xA(1)+yA(1);
    ox = xA(1);
    oy = yA(1);
    for i=2:length(xA)
       if xA(i)+yA(i) < min
          min = xA(i)+yA(i); 
          ox = xA(i);
          oy = yA(i); 
       end
    end
end

