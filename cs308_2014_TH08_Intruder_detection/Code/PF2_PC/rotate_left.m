%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Rotate_Left Function 
% Input: World Coordinates of initial, final and intruder position
% Output: Angla Theta with which bot is to be rotated to reach towards intruder
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [ theta ] = rotate_left( init, final, intruder)
    theta1  = direction(init, final);
    theta2 = direction(final, intruder);
    theta = theta2 - theta1;
    if(theta > 180)
        theta = theta - 360;
    end
    if(theta < -180)
        theta = theta + 360;
    end
end

