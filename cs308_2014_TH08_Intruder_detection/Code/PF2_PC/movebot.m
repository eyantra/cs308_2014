%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Movebot Function 
% Input: serial port object(obj1), command, time interval
% Moves the bot by taking action according to the given command for the given time interval
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function []=movebot(obj1,cmd,time)
    if(cmd=='f')
        % Communicating with instrument object, obj1.
        % moves the bot forward
        data1 = query(obj1, '8');
        pause(time);
        data1 = query(obj1, '5');
    elseif(cmd=='r')
        % rotate the bot right
        data1 = query(obj1, '6');
        pause(time);
        data1 = query(obj1, '5');
    elseif(cmd=='l')
        % rotate the bot left
        data1 = query(obj1, '4');
        pause(time);
        data1 = query(obj1, '5');
    elseif(cmd=='s')
        % stops the bot
        data1 = query(obj1, '5');
    end

end
