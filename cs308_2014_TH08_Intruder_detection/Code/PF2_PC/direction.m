function [ theta ] = direction( init, final )
 slope = (final(2)-init(2))/(final(1) - init(1)+0.001);
 theta = atand(slope);
 if(final(1) < init(1))
     theta = theta + 180;
 end
end

