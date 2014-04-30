obj = videoinput('winvideo');
%obj.ROIPosition = [0,0,100,100];
set(obj, 'ReturnedColorSpace', 'RGB');
frame = getsnapshot(obj);
image(frame);
newName = sprintf('image_processig.jpg');
imwrite(frame, ['C:\Users\nikunj\Desktop\acads\IIT\sem 8\CS 308\Project\' newName,]);

I = imread('image_processig.jpg');
subplot(3,3,1);
imshow(I);
Ibw = im2bw(I,0.6);
subplot(3,3,2);
imshow(Ibw);
Ibw = 1-Ibw;
subplot(3,3,3);
imshow(Ibw);
Ibw = imfill(Ibw,'holes');
subplot(3,3,4);
imshow(Ibw);
Ilabel = bwlabel(Ibw);
subplot(3,3,5);
imshow(Ilabel);
stat = regionprops(Ilabel,'centroid');
subplot(3,3,6);
imshow(I); hold on;

for i = 1: numel(stat)
    plot(stat(i).Centroid(1),stat(i).Centroid(2),'ro'); 
end

holesx = [stat(1).Centroid(1), stat(2).Centroid(1), stat(3).Centroid(1), stat(4).Centroid(1), stat(5).Centroid(1), stat(6).Centroid(1)];
holesy = [stat(1).Centroid(2), stat(2).Centroid(2), stat(3).Centroid(2), stat(4).Centroid(2), stat(5).Centroid(2), stat(6).Centroid(2)];
scalingFactor1 = 747 / minDistance(holesx,holesy);
scalingFactor2 = 780 / minDistance(holesx,holesy);
display(minDistance(holesx,holesy));
[ox,oy] = getOrigin(holesx,holesy);
pause(10);

frame = getsnapshot(obj);
image(frame);
imwrite(frame, ['C:\Users\nikunj\Desktop\acads\IIT\sem 8\CS 308\Project\' newName,]);

I = imread('image_processig.jpg');
Ibw = im2bw(I,0.6);
Ibw = 1-Ibw;
Ibw = imfill(Ibw,'holes');
Ilabel = bwlabel(Ibw);
stat = regionprops(Ilabel,'centroid');
subplot(3,3,7);
imshow(I); 
%subplot(3,3,8);
%imshow(Ibw); 
hold on;

display('image taken');
display('Connecting serial');

s = serial('COM17');
set(s,'BaudRate',9600);
fopen(s);

display('Connected!');

fileID = fopen('C:\Users\nikunj\Desktop\acads\IIT\sem 8\CS 308\Project\positions.txt','w');

count=0;
for i = 1: numel(stat)
    %plot(stat(i).Centroid(1),stat(i).Centroid(2),'ro');
    if isHole(stat(i).Centroid(1),stat(i).Centroid(2),10,holesx,holesy)==0
        plot(stat(i).Centroid(1),stat(i).Centroid(2),'ro');
        x = (stat(i).Centroid(1)-ox)*(scalingFactor1);
        y = (stat(i).Centroid(2)-oy)*(scalingFactor2);
        sx = num2str(floor(x));
        sy = num2str(floor(y));
        for k = length(sx)+1:4
          sx = strcat('0',sx); 
        end
        for k = length(sy)+1:4
          sy = strcat('0',sy); 
        end
        
        fprintf(fileID,'%5s\n', sx);
        fprintf(fileID,'%5s\n', sy);
        
        for k=1:4
           fwrite(s,sx(k),'char');
           pause(0.2);
        end
        for k=1:4
           fwrite(s,sy(k),'char');
           pause(0.2);
        end
       
        count = count+1;
        
    end
end
display(count);
%fwrite(s,'1','char');

fclose(s);
delete(s);
clear s;