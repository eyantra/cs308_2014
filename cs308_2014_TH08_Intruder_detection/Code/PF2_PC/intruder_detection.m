%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% INTRUDER_DETECTION Function
% Input: serial port object(obj1)
% Does the main real time execution part of the project. Calls the
% different function taking the bot and intruder positions as input and
% moving the bot in the required direction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function intruder_detection(obj)

%% Other settings...
% Threshold at which to consider the motion something of interest.
motionThreshold = 50;         % Between 0 and 255

%% Create a video input object and determine the average frame rate.
vid = videoinput('winvideo', 1, 'RGB24_640x480');
vid.ReturnedColorspace = 'rgb';


%% Initialize the previous image.
baseimage=[];
c=[-1,-1];

%% Start the video input objects
start(vid);
pause(3.0);
% Get initialize image
baseimage = imread('baseimage.jpg');
flag=true;
while(flag)
    %% Get the most recent image and time.
    try
        imageCurrent = getsnapshot(vid);
        imshow(imageCurrent);
    catch
        % getsnapshot can fail if object is deleted while we are waiting.
        return;
    end
    %% Get position of Fire Bird bot
    stats=FbLoc(imageCurrent);
    if(size(stats,1)~=0)
        A=[stats.Area];
        C=[stats.Centroid];
        [~, maxindex0] = max(A);
        bc(1) = C(maxindex0);
        bc(2) = C(maxindex0+1);
    end
    
    % bot dimension approx (up, down, left and right)
    btu=20;
    btd=120;
    btl=70;
    btr=70;
    %% Compute the difference between the current and previous image
    if(size(stats,1)~=0)
        bc(1)=floor(bc(1));
        bc(2)=floor(bc(2));
        
        % find boundaries of rectangle
        left = max(1,bc(1)-btl);
        right = min(bc(1)+btr,size(baseimage,2));
        top = max(1,bc(2)-btu);
        bottom = min(bc(2)+btd,size(baseimage,1));
        imageCurrent(top:bottom,left:right,:)=baseimage(top:bottom,left:right,:);
    end
    
    %% Detect intruder by taking the difference between baseimage and imageCurrent
    imageDifference = abs(baseimage- imageCurrent);
    
    d=rgb2gray(imageDifference);
    thresh =0.2;
    bw = (d >= thresh * 255);
    bw2 = bwareaopen(bw,10,8);
    s = regionprops(logical(bw2),'BoundingBox','Centroid','Area');
    c = [s.Centroid];
    A=[s.Area];
    [c_max, maxindex1] = max(A);
    
    %% Look for motion.
    if (numel(c)~=0 && c_max > motionThreshold)
        motion = true;
        cx = c(maxindex1);
        rx = c(maxindex1+1);
        %% Call Bot Movement function
        % Send bot toward new postion
        [wx,wy]=worldRef(cx,rx);
        v=strcat('wx  : ',num2str(wx),'  wy : ',num2str(wy));
        disp(v);
        [bwx,bwy]=worldRef(bc(1),bc(2));
        dist=((bwy-wy)^2+(bwx-wx)^2)^(0.5);
        if dist > 20
            % move bot for t
            t=0.7;
            movebot(obj,'f',t);
            %% Get the most recent image and time.
            try
                imageCurrent = getsnapshot(vid);
                imshow(imageCurrent);
            catch
                % getsnapshot can fail if object is deleted while we are waiting.
                return;
            end
            % Get new position of Fire Bird bot
            stats=FbLoc(imageCurrent);
            if(size(stats,1)~=0)
                A=[stats.Area];
                C=[stats.Centroid];
                [s_max, maxindex] = max(A);
                bc2(1) = C(maxindex);
                bc2(2) = C(maxindex+1);
            end
            
            % rotate bot with an angle between intruder and bot
            [bwx2,bwy2]=worldRef(bc2(1),bc2(2));
            theta = rotate_left([bwx,bwy], [bwx2,bwy2], [wx, wy]);
            if(theta >=0 )
                time=(theta/360)*2.6;
                movebot(obj,'l',time);
            else
                time=(-theta/360)*2.6;
                movebot(obj,'r',time);
            end
            
            % Move bot for t
            t=0.5;
            movebot(obj,'f',t);
        else
            % Close the application and then start the web application
            t=1;
            movebot(obj,'s',t);
            disp(sprintf('reached'));
            imageCurrent = getsnapshot(vid);
            imshow(imageCurrent);
            stop(vid);
            delete(vid);
            fclose(obj);
            flag=false;
        end
    else
        motion = false;
    end
end
end
